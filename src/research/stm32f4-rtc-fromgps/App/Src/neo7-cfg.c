/*! Конфигурация gps модуля, сгенерированная из конфигурации в
    формате ucenter.
    Файл сгенерирован автоматически, поэтому менять его не рекомендуется,
    так как все изменения могут быть затерты перегенерацией */

#include <stdint.h>

/*! Статические переменные для отдельных сообщений.
    Эти массивы содержат сообщение полностью, без синхрослова в начале
    и контрольной суммы в конце.
    Однако такие поля как cls_id, msg_id, len в сообщениях содержаться */

static const uint8_t cfg_ant_0[] = {0x06, 0x13, 0x04, 0x00, 0x1B, 0x00, 0xF0, 0xB9};
// static const uint8_t cfg_dat_1[] = {0x06, 0x06, 0x02, 0x00, 0x00, 0x00};
static const uint8_t cfg_gnss_2[] = {0x06, 0x3E, 0x24, 0x00, 0x00, 0x16, 0x16, 0x04, 0x00, 0x04, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x08, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_inf_3[] = {0x06, 0x02, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_inf_4[] = {0x06, 0x02, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87};
static const uint8_t cfg_inf_5[] = {0x06, 0x02, 0x0A, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_itfm_6[] = {0x06, 0x39, 0x08, 0x00, 0xF3, 0xAC, 0x62, 0x2D, 0x1E, 0x03, 0x00, 0x00};
static const uint8_t cfg_logfilter_7[] = {0x06, 0x47, 0x0C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_8[] = {0x06, 0x01, 0x08, 0x00, 0x0B, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_9[] = {0x06, 0x01, 0x08, 0x00, 0x0B, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_10[] = {0x06, 0x01, 0x08, 0x00, 0x0B, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_11[] = {0x06, 0x01, 0x08, 0x00, 0x0B, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_12[] = {0x06, 0x01, 0x08, 0x00, 0x0B, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_13[] = {0x06, 0x01, 0x08, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_14[] = {0x06, 0x01, 0x08, 0x00, 0x21, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_15[] = {0x06, 0x01, 0x08, 0x00, 0x0A, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_16[] = {0x06, 0x01, 0x08, 0x00, 0x0A, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_17[] = {0x06, 0x01, 0x08, 0x00, 0x0A, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_18[] = {0x06, 0x01, 0x08, 0x00, 0x0A, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_19[] = {0x06, 0x01, 0x08, 0x00, 0x0A, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_20[] = {0x06, 0x01, 0x08, 0x00, 0x0A, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_21[] = {0x06, 0x01, 0x08, 0x00, 0x0A, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_22[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_23[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_24[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_25[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_26[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_27[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_28[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_29[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_30[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_31[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_32[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_33[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_34[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_35[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_36[] = {0x06, 0x01, 0x08, 0x00, 0x01, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_37[] = {0x06, 0x01, 0x08, 0x00, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_38[] = {0x06, 0x01, 0x08, 0x00, 0x0D, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_39[] = {0x06, 0x01, 0x08, 0x00, 0x0D, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_40[] = {0x06, 0x01, 0x08, 0x00, 0x0D, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_41[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01};
static const uint8_t cfg_msg_42[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01};
static const uint8_t cfg_msg_43[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01};
static const uint8_t cfg_msg_44[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01};
static const uint8_t cfg_msg_45[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01};
static const uint8_t cfg_msg_46[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01};
static const uint8_t cfg_msg_47[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_48[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_49[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_50[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_51[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_52[] = {0x06, 0x01, 0x08, 0x00, 0xF0, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_53[] = {0x06, 0x01, 0x08, 0x00, 0xF1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_54[] = {0x06, 0x01, 0x08, 0x00, 0xF1, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_msg_55[] = {0x06, 0x01, 0x08, 0x00, 0xF1, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_nav5_56[] = {0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_navx5_57[] = {0x06, 0x23, 0x28, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x4F, 0x00, 0x00, 0x00, 0x03, 0x02, 0x03, 0x16, 0x07, 0x00, 0x00, 0x01, 0x00, 0x00, 0x9B, 0x06, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64, 0x64, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_nmea_58[] = {0x06, 0x17, 0x0C, 0x00, 0x00, 0x23, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_pm2_59[] = {0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x90, 0x02, 0x00, 0xE8, 0x03, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x86, 0x02, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00};
static const uint8_t cfg_prt_60[] = {0x06, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_prt_61[] = {0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
// static const uint8_t cfg_prt_62[] = {0x06, 0x00, 0x14, 0x00, 0x02, 0x00, 0x00, 0x00, 0xC0, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_prt_63[] = {0x06, 0x00, 0x14, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_prt_64[] = {0x06, 0x00, 0x14, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t cfg_rate_65[] = {0x06, 0x08, 0x06, 0x00, 0xE8, 0x03, 0x01, 0x00, 0x01, 0x00};
static const uint8_t cfg_rinv_66[] = {0x06, 0x34, 0x18, 0x00, 0x00, 0x4E, 0x6F, 0x74, 0x69, 0x63, 0x65, 0x3A, 0x20, 0x6E, 0x6F, 0x20, 0x64, 0x61, 0x74, 0x61, 0x20, 0x73, 0x61, 0x76, 0x65, 0x64, 0x21, 0x00};
static const uint8_t cfg_rxm_67[] = {0x06, 0x11, 0x02, 0x00, 0x08, 0x00};
static const uint8_t cfg_sbas_68[] = {0x06, 0x16, 0x08, 0x00, 0x01, 0x03, 0x03, 0x00, 0xD1, 0xA2, 0x06, 0x00};
static const uint8_t cfg_tp5_69[] = {0x06, 0x31, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x00, 0x00, 0x00};
static const uint8_t cfg_tp5_70[] = {0x06, 0x31, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x48, 0xE8, 0x01, 0x00, 0xA0, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00};
static const uint8_t cfg_usb_71[] = {0x06, 0x1B, 0x6C, 0x00, 0x46, 0x15, 0xA7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x02, 0x01, 0x75, 0x2D, 0x62, 0x6C, 0x6F, 0x78, 0x20, 0x41, 0x47, 0x20, 0x2D, 0x20, 0x77, 0x77, 0x77, 0x2E, 0x75, 0x2D, 0x62, 0x6C, 0x6F, 0x78, 0x2E, 0x63, 0x6F, 0x6D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x2D, 0x62, 0x6C, 0x6F, 0x78, 0x20, 0x37, 0x20, 0x2D, 0x20, 0x47, 0x50, 0x53, 0x2F, 0x47, 0x4E, 0x53, 0x53, 0x20, 0x52, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*! Эта переменная не статическая и предполагается её использование в
    прочих файлах проекта. Эта переменная является этаким
    "двумерным массивом" конфигурационных сообщений.

    Конец массива терменирован нулевым указателем */
const uint8_t * ublox_cfg_msgs[] = {cfg_ant_0, /*cfg_dat_1,*/ cfg_gnss_2, cfg_inf_3, cfg_inf_4, cfg_inf_5, cfg_itfm_6, cfg_logfilter_7, cfg_msg_8, cfg_msg_9, cfg_msg_10, cfg_msg_11, cfg_msg_12, cfg_msg_13, cfg_msg_14, cfg_msg_15, cfg_msg_16, cfg_msg_17, cfg_msg_18, cfg_msg_19, cfg_msg_20, cfg_msg_21, cfg_msg_22, cfg_msg_23, cfg_msg_24, cfg_msg_25, cfg_msg_26, cfg_msg_27, cfg_msg_28, cfg_msg_29, cfg_msg_30, cfg_msg_31, cfg_msg_32, cfg_msg_33, cfg_msg_34, cfg_msg_35, cfg_msg_36, cfg_msg_37, cfg_msg_38, cfg_msg_39, cfg_msg_40, cfg_msg_41, cfg_msg_42, cfg_msg_43, cfg_msg_44, cfg_msg_45, cfg_msg_46, cfg_msg_47, cfg_msg_48, cfg_msg_49, cfg_msg_50, cfg_msg_51, cfg_msg_52, cfg_msg_53, cfg_msg_54, cfg_msg_55, cfg_nav5_56, cfg_navx5_57, cfg_nmea_58, cfg_pm2_59, cfg_prt_60, cfg_prt_61, /*cfg_prt_62,*/ cfg_prt_63, cfg_prt_64, cfg_rate_65, cfg_rinv_66, cfg_rxm_67, cfg_sbas_68, cfg_tp5_69, cfg_tp5_70, cfg_usb_71, 0};
