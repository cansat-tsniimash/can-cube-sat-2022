#include "vc.h"
#include "map.h"
#include "uslp_mx.h"


static void _sap_clear(sap_t* sap) {
    sap->is_empty = true;
}

//TODO: COP
bool cop1_is_full(const cop1_t* cop) {
    return cop->is_full;
}

bool cop1_is_empty(const cop1_t* cop) {
    return cop->head == cop->tail && !cop->is_full;
}

vc_frame_t* cop1_new_frame(cop1_t* cop) {
    assert(!cop1_is_full(cop));
    vc_frame_t* frame = &cop->arr[cop->tail];
    cop->tail = (cop->tail + 1) % cop->size;
    if (cop->head == cop->tail) {
        cop->is_full = true;
    }
    return frame;
}

void cop1_pop(cop1_t* cop) {
    assert(!cop1_is_empty(cop));

    if (cop->is_full) {
        cop->is_full = false;
    }
    cop->head = (cop->head + 1) % cop->size;
}

bool cop1_is_there_next_frame(const cop1_t* cop) {
    return cop->cur != cop->tail;
}

vc_frame_t* cop1_get_next(cop1_t* cop) {
    assert(cop1_is_there_next_frame(cop));
    vc_frame_t* frame = &cop->arr[cop->cur];
    return frame;
}

void cop1_release_frame(cop1_t* cop) {
    cop->cur = (cop->cur + 1) % cop->size;
}

bool vc_push_new_frame(vc_t* vc, const map_data_t* md) {
    
    vc_frame_t* frame_to = 0;
    if (vc->cop_type == COP_NONE || md->qos == QOS_EXPEDITED) {
        assert(md->tfdf.size <= vc->container.expedited_frame.tfdz_size);
        if (vc->container.expedited_frame.is_full) {
            return false;
        }
        vc->container.expedited_frame.is_full = true;
        frame_to = &vc->container.expedited_frame.frame;

        frame_to->ttl = 0;
        frame_to->vc_data.vc_frame_count = vc->ex_frame_count++;
        frame_to->vc_data.vc_frame_count_length = vc->ex_frame_count_length;
    } else if (vc->cop_type == COP_1) {
        assert(md->tfdf.size <= vc->container.cop1.tfdz_max_size);
        frame_to = cop1_new_frame(&vc->container.cop1);
        if (frame_to == 0) {
            return false;
        }
        frame_to->ttl = vc->seq_ctrld_ttl;
        frame_to->vc_data.vc_frame_count = vc->sc_frame_count++;
        frame_to->vc_data.vc_frame_count_length = vc->sc_frame_count_length;
    } else {
        assert(0 && "Other cops are not implemented");//TODO
    }
    memcpy(frame_to->map_data.tfdf.tfdz, md->tfdf.tfdz, md->tfdf.size);
    uint8_t* tfdz = frame_to->map_data.tfdf.tfdz;
    frame_to->map_data = *md;
    frame_to->map_data.tfdf.tfdz = tfdz;
    
    frame_to->vc_data.contains_protocol_control_commands = 
        md->tfdf.upid == UPID_COP1 ||
        md->tfdf.upid == UPID_COPP ||
        md->tfdf.upid == UPID_SDLS; 
    //WARN: I have no idea about PROXIMITY packets, so some of them 
    //maybe should be included here
    frame_to->vc_data.frame_trancated = false;
    frame_to->vc_data.vc_id = vc->vc_id;
    if (vc->cop_type == COP_NONE) {
        frame_to->map_data.qos = QOS_EXPEDITED;
    }
    return true;
}

vc_frame_t* vc_get_frame(vc_t* vc) { 
    if (vc->container.expedited_frame.is_full) {
        return &vc->container.expedited_frame.frame;
    } else if (vc->cop_type == COP_1) {
        return cop1_get_next(&vc->container.cop1);
    } else {
        assert(0);
        abort();
    }
}

void vc_release_frame(vc_t* vc) {
    if (vc->container.expedited_frame.is_full) {
        vc->container.expedited_frame.is_full = false;
        //NOTE: Следующая строка в готовой версии будет не нужна
        memset(vc->container.expedited_frame.frame.map_data.tfdf.tfdz, 0, vc->container.expedited_frame.tfdz_size);
    } else if (vc->cop_type == COP_1) {
        cop1_release_frame(&vc->container.cop1);
    } else {
        assert(0);
        abort();
    }
}


void vc_pull_everything_from_bottom(vc_t* vc) {
    // Получаем текущие верщины дерева мастер канала и виртуал канала
    mx_node* vcmx = &vc->mx;
    mx_node* mcmx = mx_get_parent(vcmx);
 
    
    map_data_t md = {0};
    bool release_sap = false;
    bool release_map = false;
    while (true) {
        // Смотрим, есть ли map канал с данными
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
        // release_sap - означает, что sap израсходован, там больше нет данных и его можно очистить
        // и выкинуть из очередей
        // release_map - означает, что нам разрешено отпустить map.
        md_legit = map_pull_data(map, &md, &release_sap, &release_map);

        // У нас есть какие-то данные. Удостоверимся, что все вершины есть в очередях.

        if (md_legit) {
            mx_try_push_to_parent_updated(mapmx);
            mx_push_to_parent_ready_forced(vcmx);
            mx_push_to_parent_ready_forced(mcmx);
        } else if (!(release_sap && release_map)) {
            assert( map->map_type != MAP_TYPE_ACCESS);
            mx_try_push_to_parent_updated(mapmx);
            mx_try_push_to_parent_updated(vcmx);
            mx_try_push_to_parent_updated(mcmx);
        }
        // Очищаем sap и буферы внутри map, если sap опустел
        if (release_sap && sapmx) {
            _sap_clear(sap);
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
            // Очищаем map
            map_clear_tfdf(map);
        }
        

        // Если нам рарешено отпускать map, то отпускаем его. Если при этом в нем есть данные,
        // или внутри его sap-ов есть данные, то возвращаем его в очередь. Таким образом,
        // в VC сменится map, если там кто-то есть еще, или если в этом map есть еще данные,
        // то он снова станет текущим в очереди.
        if (release_map) {
            mx_remove_from_parent_updated(mapmx);
            
            if (mx_current_updated(mapmx)) {
                mx_push_to_parent_updated(mapmx);
            }
        }
        
    }
}