#pragma once

#include "recieve.h"
#include "fifo.h"

typedef enum _vcr_state_t {
    VCR_STATE_SPAN,
    VCR_STATE_WAITING_SIZE,
} vcr_state_t;
 
typedef struct {
    demx_entry_t demx_entry;
    struct {
        upid_t upid;
        int map_id;
        pvn_t pvn;
    } mapr;
    cop_enum_t cop_type;
    buffer_t buffer;
    uint64_t ex_frame_count;
    uint64_t sc_frame_count;
    void (*map_cb)(void* arg, const uint8_t* data, size_t size);
    void *map_cb_arg;
} vc_receive_t;

bool farm_1_process_frame(const map_data_t* md, const vc_data_t* vd) {
    //TODO
    return true;
}

bool map_receive_octet(vc_receive_t* vcr, const map_data_t* md) {
    
    return true;
}

bool map_receive_mapp(vc_receive_t* vcr, const map_data_t* md) {
    
    return true;
}





static size_t consume(vc_receive_t* vcr, uint8_t* tfdz, size_t tfdz_start, size_t tfdz_end) {
    memcpy(vcr->buffer.data + vcr->buffer.index, tfdz + tfdz_start, tfdz_end - tfdz_start);
    vcr->buffer.index += tfdz_end;
    return tfdz_start + tfdz_end;
}

static uint8_t extract_pvn(uint8_t byte) {
    uint8_t val = 0;
    ccsds_endian_extract(&byte, 0, &val, 3);

    return val;
}
static bool try_get_size_from_packet(uint8_t* data, size_t data_size, size_t* size) {
    if (data_size < 1) {
        return false;
    }
    int pvn = extract_pvn(data[0]);
    if (pvn == PVN_EP) {
        epp_header_t h = {0};
        if (!epp_extract_header(&h, data, data_size)) {
            return false;
        }
        *size = h.packet_length;
        return true;
    }
    return false;
}

static void call_back(vc_receive_t* vcr) {
    if (vcr->buffer.size > 0 && extract_pvn(vcr->buffer.data[0]) == vcr->mapr.pvn) {
        vcr->map_cb(vcr->map_cb_arg, vcr->buffer.data, vcr->buffer.size);
    }
}


static void clear_buffer(vc_receive_t* vcr) {
    vcr->buffer.index = 0;
    vcr->buffer.size = 0;
}
static void tfdz_mapp_to_data(vc_receive_t* vcr, const map_data_t* md, const vc_data_t* vd) {

    //Это начало tfdf. У нас есть много информации, которой мы можем воспользоваться

    if (md->tfdf.pointer_fh_lo == 0xFFFF) {
        //Нет начала пакета
        if (vcr->buffer.index == 0) {
            //Буфер пуст и при этом нет начала пакета - скам
            return;
        }
        //Нет начала пакета, значит весь tfdf наш
        consume(vcr, md->tfdf.tfdz, 0, md->tfdf.size);
        if (vcr->buffer.size == 0) {
            size_t packet_size = 0;
            if (!try_get_size_from_packet(vcr->buffer.data, vcr->buffer.index, &packet_size)) {
                //Размер пакета не получен - значит пакет еще не закончен, продолжение в
                //следующем пакете, а значит весь этот tfdf - часть этого пакета. На этом закончим.
                return; 
            } 
            vcr->buffer.size = packet_size;
        }
        //Весь tfdf принадлежит текущему пакету. Но продолжается пакет дальше, или
        //он как раз тут и закончился?
        if (vcr->buffer.size < vcr->buffer.index) {
            //Пакет должен был закончится, а начала нового нет - не порядок
            clear_buffer(vcr);
        } else if (vcr->buffer.size == vcr->buffer.index) {
            //Мы собрали целый пакет
            call_back(vcr);
            clear_buffer(vcr);
        }
        //Данных для следующего пакета нет - выходим
        return;
    }

    //У нас есть начало пакета во фрейме.
    //Если буфер пуст - просто начнем новый пакет
    //А если нет - надо закончить старый
    if (vcr->buffer.index != 0) {
        //Буфер не пуст
        //У нас есть начало нового пакета. Значит, текущий пакет должен тут и закончится.
        consume(vcr, md->tfdf.tfdz, 0, md->tfdf.pointer_fh_lo);

        /**
         * Все ситуации:
         * 1. Размер не читается - пакет ломанный
         * 2. Размер не сходится - пакет ломанный
         * 3. Размер сходится - обрабатываем пакет
         * При любом раскладе сбрасываем в конце буфер, смотрим на следующие данные
         */
        if (vcr->buffer.size == 0) {
            size_t packet_size = 0;
            
            if (try_get_size_from_packet(vcr->buffer.data, vcr->buffer.index, &packet_size)) {
                vcr->buffer.size = packet_size;
            } 
        }
        if (vcr->buffer.size != 0 && vcr->buffer.size == vcr->buffer.index) {
            call_back(vcr);
        }
        clear_buffer(vcr);
    }
    size_t tfdz_index = md->tfdf.pointer_fh_lo;

    //Мы обработали начало tfdf. Теперь у нас нет какой-либо полезной информации
    while (tfdz_index < md->tfdf.size) {
        //Считаем пакет, начаиная с номера начала пакета
        consume(vcr, md->tfdf.tfdz, tfdz_index, md->tfdf.size);
        //Стоит попробовать получить размер пакета
        if (vcr->buffer.size == 0) {
            size_t packet_size = 0;
            if (!try_get_size_from_packet(vcr->buffer.data, vcr->buffer.index, &packet_size)) {
                //Размер пакета не получен - значит пакет еще не закончен, продолжение в
                //следующем пакете, а значит весь этот tfdf - часть текущего пакета. На этом закончим.
                return; 
            }
            vcr->buffer.size = packet_size;
        } 
        if (vcr->buffer.size > vcr->buffer.index) {
            //Пакет продолжится в следующем фрейме, а значит весь этот tfdf - часть этого пакета. На этом закончим.
            return;
        }
        //Рамер пакета меньше (или равен) количеству данных. Значит, пакет собран, и мы можем обработать
        //его и попробовать собрать ещё пару пакетов
        call_back(vcr);
        clear_buffer(vcr);
        tfdz_index += vcr->buffer.size;
    }
}


static void tfdz_mapa_to_data(vc_receive_t* vcr, const map_data_t* md, const vc_data_t* vd) {
    if (md->tfdf.tfdz_rule == TFDZ_RULE_MAPA_START) {
        clear_buffer(vcr);
    } else if(vcr->buffer.index == 0) {
        return;
    }
    if (md->tfdf.pointer_fh_lo == 0xFFFF) {
        consume(vcr, md->tfdf.tfdz, 0, md->tfdf.size);
    } else {
        consume(vcr, md->tfdf.tfdz, 0, md->tfdf.pointer_fh_lo);
        vcr->map_cb(vcr->map_cb_arg, vcr->buffer.data, vcr->buffer.index);
        clear_buffer(vcr);
    }
}


static void tfdz_to_data(vc_receive_t* vcr, const map_data_t* md, const vc_data_t* vd) {
    if (vcr->mapr.upid == UPID_SP_OR_EP) {
        tfdz_mapp_to_data(vcr, md, vd);
    } else if (vcr->mapr.upid == UPID_MAPA) {
        tfdz_mapa_to_data(vcr, md, vd);
    } else {
        clear_buffer(vcr);
    }
}

void vc_receive_cb(void* arg, const map_data_t* md, const vc_data_t* vd) {
    vc_receive_t* vcr = (vc_receive_t*)arg;

    if (md->qos == QOS_SEQ_CTRLD) {
        if (vcr->cop_type == COP_NONE) {
            //TODO: Send warning to user about receiving sequence controlled
            //frame and not having FARM for accepting it;
            goto drop_frame;
        }
        if (!farm_1_process_frame(md, vd)) {
            //TODO: Send FARM message to user
            goto drop_frame;
        }
    } 
    uint64_t frame_count = 0;
    if (md->qos == QOS_SEQ_CTRLD) {
        frame_count = vcr->sc_frame_count;
    } else {
        frame_count = vcr->ex_frame_count;
    }
    if ((vd->vc_frame_count - frame_count) % ((uint64_t)1 << vd->vc_frame_count_length) != 1) {
        clear_buffer(vcr);
        //TODO: warning about lost frames
    }    
    if (md->qos == QOS_SEQ_CTRLD) {
        vcr->sc_frame_count = vd->vc_frame_count;
    } else {
        vcr->ex_frame_count = vd->vc_frame_count;
    }
    
    //NOTE: What to about PCC packets?

    
    if (md->tfdf.upid == UPID_IDLE) {
        return;
    }
    if (md->tfdf.upid == UPID_PROX1_PPID1 || 
        md->tfdf.upid == UPID_PROX1_PPID2 || 
        md->tfdf.upid == UPID_PROX1_SPDU || 
        md->tfdf.upid == UPID_COP1 || 
        md->tfdf.upid == UPID_COPP || 
        md->tfdf.upid == UPID_SDLS) {
        // Unsupported upids
        return;
    }

    tfdz_to_data(vcr, md, vd);
    return;
drop_frame:
    clear_buffer(vcr);
}