#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "vc.h"
#include "uslp_mx.h"
#include "map.h"



void sap_clear(sap_t* sap) {
    sap->is_empty = true;
}

void sap_set_data(sap_t* sap) {
    mx_node* sapmx = &sap->mx;
    mx_node* mapmx = mx_get_parent(sapmx);
    mx_node* vcmx = mx_get_parent(mapmx);
    mx_node* mcmx = mx_get_parent(vcmx);
    map_t* map = mx_get_map(mapmx);

    mx_try_push_to_parent_updated(sapmx);
    mx_try_push_to_parent_updated(mapmx);
    mx_try_push_to_parent_updated(vcmx);
    mx_try_push_to_parent_updated(mcmx);
}


void vc_pull_everything_from_bottom(vc_t* vc) {
    // Получаем текущие верщины дерева мастер канала и виртуал канала
    mx_node* vcmx = &vc->mx;
    mx_node* mcmx = mx_get_parent(vcmx);
 
    
    map_data_t md = {0};
    bool release_sap = false;
    bool release_map = false;
    while (true) {
        // Смотрим, есть map канал с данными
        mx_node* mapmx = mx_current_updated(vcmx);
        if (mapmx == 0) {  
            break;
        }
        map_t* map = mx_get_map(mapmx);
        sap_t* sap = 0;
        bool md_legit = false;
        // Получаем sap канал с данными
        mx_node* sapmx = mx_current_updated(mapmx);
        if (sapmx != 0) {
            sap = mx_get_sap(sapmx);
            map_set_source(map, sap->data.data, sap->data.size);
        }
        // Получаем данные из map канала. Если sap нету, то он выдаст md, если он есть,
        // и не выдаст, если его нет. 
        // md_legit - переменная md содержит в себе md.
        // release_sap - означает, что sap израсходован, там больше нет данных и его можно очистить,
        // и выкинуть из очередей
        // release_map - означает, что нам разрешено отпустить map.
        md_legit = map_pull_data(map, &md, &release_sap, &release_map);

        // У нас есть какие-то данные. Удостоверимся, что все вершины есть в очередях.
        // Особый случай с MAPA, так как он не может отдавать данные по частям,
        // поэтому мы не имеем право насильно просить TFDF
        if ((!release_sap || !release_map || md_legit) && map->map_type != MAP_TYPE_ACCESS) {
            mx_try_push_to_parent_updated(mapmx);
            mx_try_push_to_parent_updated(vcmx);
            mx_try_push_to_parent_updated(mcmx);
        }
        // Очищаем sap и буферы внутри map, если sap опустел
        if (release_sap && sapmx) {
            sap_clear(sap);
            map_clear_source(map);
            mx_remove_from_parent_updated(sapmx);
        }

        // Данных map не дает, и не остпускает. Всё.
        if (!release_map && !md_legit) {
            return;
        }
        // А если md у нас есть, то попроуем закинуть в VC
        if (md_legit) {
            if (!vc_push_new_frame(vc, &md)) {
                // Аха!!! VC полон, мы не можем закинуть md.
                return;
            }
            // Обновляем очереди готовности
            
            
            if (!mx_is_in_ready(vcmx)) {
                mx_remove_from_parent_updated(vcmx);
                mx_push_to_parent_ready(vcmx);
                if (!mx_is_in_ready(mcmx)) {
                    mx_remove_from_parent_updated(mcmx);
                    mx_push_to_parent_ready(mcmx);
                }
            }
            // Очищаем map
            map_clear_tfdf(map);
        }
        

        // Если нам рарешено отпускать map, то отпускаем его. Если при этом в нем есть данные,
        // или внутри его sap-ов есть данные, то возвращаем его в очередь. Таким образом,
        // в VC сменится map, если там кто-то есть еще, или если в этом map есть еще данные,
        // то он снова станет текущим в очереди.
        if (release_map) {
            mx_remove_from_parent_updated(mapmx);
            if (!release_sap || md_legit || mx_current_updated(mapmx)) {
                mx_push_to_parent_updated(mapmx);
            }
        }
        
    }
}


size_t sap_send(sap_t* sap, uint8_t const* data, size_t size) {
    if (!sap->is_empty || sap->data.capacity < size ||
        (sap->policy == POLICY_SIZE_STRICT && size != sap->data.capacity))
    {
        return 0;
    }

    sap->is_empty = false;
    memcpy(sap->data.data, data, size);
    sap->data.size = size;
    if (sap->service == SERVICE_MAP_OCTET || sap->service == SERVICE_MAPP || sap->service == SERVICE_MAPA) {
        sap_set_data(sap);
        vc_t* vc = mx_get_vc(mx_get_parent(mx_get_parent(&sap->mx)));
        vc_pull_everything_from_bottom(vc);
    }

    return size;
}

