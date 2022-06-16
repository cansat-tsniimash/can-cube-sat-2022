 /*
 * ubx_packets.h
 *
 *  Created on: Apr 4, 2020
 *      Author: snork
 */

#ifndef INC_UBX_PACKET_PARSER_H_
#define INC_UBX_PACKET_PARSER_H_


#include <stdint.h>

//! Синхрослово пакета ubx
#define UBX_SYNCWORD_VALUE 0xb562

//! Длина заголовка пакета UBX
#define UBX_HEADER_SIZE 4

//! Длина контрольной суммы UBX пакета
#define UBX_CRC_SIZE 2


//! Идентификатор пакета
/*! Состоит из class и id пакета */
typedef enum ubx_pid_t
{
	UBX_PID_NAV_SOL = 0x0106,
	UBX_PID_NAV_TIMEGPS = 0x0120,
	UBX_PID_NAV_SVINFO = 0x0130,
	UBX_PID_TIM_TP  = 0x0d01,
	UBX_PID_CFG_NACK = 0x0500,
	UBX_PID_CFG_ACK  = 0x0501,
	UBX_PID_MON_HW2 = 0x0A0B,
	UBX_PID_MON_HW = 0x0A09,
	UBX_PID_RXM_SVSI = 0x0220,
} ubx_pid_t;


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// GPS-TIME, TIM-TP
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//! Флаги валидности времени из пакета gpstime
typedef enum ubx_nav_gpstime_flags_t
{
	//! время недели GPS достоверно (iTOW и fTOW)
	UBX_NAVGPSTIME_FLAGS__TOW_VALID = 0x01 << 0,
	//! номер недели GPS достоверен
	UBX_NAVGPSTIME_FLAGS__WEEK_VALID = 0x01 << 1,
	//!  скачок секунд GPS достоверен
	UBX_NAVGPSTIME_FLAGS__LEAPS_VALID = 0x01 << 2
} ubx_nav_gpstime_flags_t;


//! Пакет GPS-TIME
typedef struct ubx_gpstime_packet_t
{
	//! Время недели GPS для эпохи навигации
	uint32_t tow_ms;
	//! Дробная часть iTOW (диапазон ±500000).
	//! Точность времени недели GPS: (iTOW · 1e-3) + (fTOW · 1e-9)
	int32_t ftow;
	//! Номер недели эпохи навигации
	uint16_t week;
	//! Скачок секунд GPS (GPS-UTC)
	int8_t leaps;
	//! Флаги достоверности (смотри ubx_nav_gpstime_flags_t)
	uint8_t valid_flags;
	//! Оценка точности времени
	uint32_t t_acc;
} ubx_gpstime_packet_t;


//! Пакет TIM-TP
typedef struct ubx_timtp_packet_t
{
	//! Номер недели эпохи навигации
	uint16_t week;
	//! Время недели GPS для эпохи навигации
	uint32_t tow_ms;
} ubx_timtp_packet_t;


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// NAV-SOL
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//! тип FIX
typedef enum ubx_fix_type_t
{
	//! No Fix (нет фиксации)
	UBX_FIX_TYPE__NO_FIX = 0x00,
	//! только Dead Reckoning
	UBX_FIX_TYPE__DEAD_RECKONING = 0x01,
	//! 2D-Fix
	UBX_FIX_TYPE__2D = 0x02,
	//! 3D-Fix
	UBX_FIX_TYPE__3D = 0x03,
	//! комбинация GNSS + dead reckoning
	UBX_FIX_TYPE__COMBO = 0x04,
	//! только фиксация времени
	UBX_FIX_TYPE__TIME_ONLY = 0x05,

	// Прочие значения зарезервированы
} ubx_fix_type_t;


//! Битовые флаги из поля flags сообщения ubx_nav_sol_packet_t
typedef enum ubx_nav_sol_flags_t
{
	//!  фиксация в определенных пределах
	UBX_NAVSOL_FLAGS__GPS_FIX_OK	= 0x01 << 0,
	//! использовалось DGPS
	UBX_NAVSOL_FLAGS__DIFF_SOL_IN	= 0x01 << 1,
	//! достоверный номер недели GPS
	UBX_NAVSOL_FLAGS__WKN_SET		= 0x01 << 2,
	//!  достоверно время недели GPS (iTOW и fTOW)
	UBX_NAVSOL_FLAGS__TOW_SET		= 0x01 << 3
} ubx_nav_sol_flags_t;


//! NAV-SOL
typedef struct ubx_navsol_packet_t
{
	//! Время недели GPS для эпохи навигации
	uint32_t i_tow;
	//! Дробная часть iTOW (диапазон ±500000).
	int32_t f_tow;
	//! Номер недели эпохи навигации
	uint16_t week;
	//! Тип фиксации позиции GPS. Значения описаны в ubx_fix_type_t
	uint8_t gps_fix;
	//! Флаги состояния фиксации. Значения описаны в ubx_nav_sol_flags_t
	uint8_t flags;
	//! Координаты в ECEF
	int32_t pos_ecef[3];
	//! Оценка точности положения в ECEF
	uint32_t p_acc;
	//! Скорость в ECEF
	int32_t vel_ecef[3];
	//! Оценка точности скорости в ECEF
	uint32_t s_acc;
	//! DOP позиционирования
	uint16_t p_dop;
	// Количество спутников
	uint8_t num_sv;
} ubx_navsol_packet_t;


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// ACK, NACK
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//! Подтверждение получения валидного конфигурационного пакета
typedef struct ubx_ack_packet_t
{
	//! Идентификатор пакета
	ubx_pid_t packet_pid;
} ubx_ack_packet_t;


//! Сообщение об ошибке в полученном конфигурационном пакете
typedef struct ubx_nack_packet_t
{
	//! Идентификатор пакета
	ubx_pid_t packet_pid;
} ubx_nack_packet_t;


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// MON-HW2
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//! Сообщение об ошибке в полученном конфигурационном пакете
typedef struct ubx_monhw2_packet_t
{
	/* Дисбаланс I-составляющей комплексного сигнала, масштабированный
	(-128 = max. отрицательный дисбаланс, 127 = max. положительный дисбаланс).*/
	int8_t ofsI;
	/* Магнитуда I-составляющей комплексного сигнала, масштабированная
	(0 = нет сигнала, 255 = max. магнитуда).*/
	uint8_t magI;
	/* Дисбаланс Q-составляющей комплексного сигнала, масштабированный
	(-128 = max. отрицательный дисбаланс, 127 = max. положительный дисбаланс).*/
	int8_t ofsQ;
	/* Магнитуда Q-составляющей комплексного сигнала, масштабированная
	(0 = нет сигнала, 255 = max. магнитуда).*/
	uint8_t magQ;
	/* Источник низкоуровневой конфигурации:
	   114 ROM
	   111 OTP
	   112 ножки для конфигурирования
	   102 образ flash*/
	uint8_t cfgSource;
	/* Низкоуровневая конфигурация*/
	uint32_t lowLevCfg;
	/* Слово состояния теста POST*/
	uint32_t postStatus;
} ubx_monhw2_packet_t;


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// MON-HW
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//! Сообщение об ошибке в полученном конфигурационном пакете
typedef struct ubx_monhw_packet_t
{
	/* Маска набора выводов в качестве периферии/PIO. */
	uint32_t pinSel;
	/* Маска набора выводов в качестве банков A/B */
	uint32_t pinBank;
	/* Маска набора выводов в качестве ввода/вывода (Input/Output, I/O) */
	uint32_t pinDir;
	/* Маска уровня выводов лог. 0 / лог. 1. */
	uint32_t pinVal;
	/* Уровень шума, измеренный ядром GPS */
	uint16_t noisePerMS;
	/* Монитор АРУ (AGC Monitor, считает SIGHI xor SIGLO, диапазон 0..8191). */
	uint16_t agcCnt;
	/* Статус машины состояний супервизора антенны (0=INIT, 1=DONTKNOW, 2=OK, 3=SHORT, 4=OPEN). */
	uint8_t aStatus;
	/* Текущее состояние питания антенны */
	uint8_t aPower;
	/* Флаги */
	uint8_t flags;
	/* Маска выводов, которые использует Virtual Pin Manager */
	uint32_t usedMask;
	/* Массив привязок каждого из 17 физических выводов. */
	uint8_t VP[25];
	/* Индикатор пропадания несущей (CW Jamming indicator), масштабированный
	(0 = нет нарушения CW, 255 очень сильные нарушения CW). */
	uint8_t jamInd;
	/* Маска значения выводов, использующих PIO Irq */
	uint32_t pinIrq;
	/* Маска значения выводов, использующих верхний
	подтягивающий резистор PIO (PullUp High). */
	uint32_t pullH;
	/* Маска значения выводов, использующих нижний
	подтягивающий резистор PIO (PullUp Low). */
	uint32_t pullL;
} ubx_monhw_packet_t;


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// RXM-SVSI
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//! Различные флаги из RXM-SVSI
/*! Эти флаги делят поле на пару со значением URA */
typedef enum ubx_rxm_svsi_flags_t
{
	//! Спутник здоров!
	UBX_RXMSVSI_FLAGS__HEALTHY		= 0x01 << 4,
	//! Эфемериды спутника валидны
	UBX_RXMSVSI_FLAGS__EPH_VAL		= 0x01 << 5,
	//! Альманах спутника валиден
	UBX_RXMSVSI_FLAGS__ALM_VAL		= 0x01 << 6,
	//! Спутник не доступен
	UBX_RXMSVSI_FLAGS__NOT_AVAIL	= 0x01 << 7,
} ubx_rxm_svsi_flags_t;


//! SV Status Info
typedef struct ubx_rxmsvsi_packet_t
{
	/* Время недели GPS для эпохи навигации (см. "6.2. Navigation Epoch" и
	"6.3. Метки времени iTOW" [2] для получения дополнительной информации).*/
	uint32_t iTOW;
	/* Номер недели эпохи навигации (см. "6.2. Navigation Epoch" [2]).*/
	int16_t week;
	/* Количество видимых спутников*/
	uint8_t numVis;
	/* Количество блоков данных, следующих далее, по одному блоку на спутник.*/
	uint8_t numSV;
	/* Начало повторяющихся блоков(повторяются numSV раз) */
	const uint8_t* SVbuf_ptr;
} ubx_rxmsvsi_packet_t;


// Возвращает количество блоков данных, следующих далее, по одному блоку на спутник.
uint8_t ubx_parse_rxm_svsi_SV_num(const ubx_rxmsvsi_packet_t * packet);


// SV Status Info информация о спутниуах
typedef struct ubx_rxmsvsi_SV_packet_t
{
	/* Идентификатор спутника */
	uint8_t svid;
	/* Информационные флаги и URA (ubx_rxm_svsi_flags_t) */
	uint8_t svFlag;
	/* Азимут */
	int16_t azim;
	/* Возвышение */
	int8_t elev;
	/* Возврат альманаха и эфемерид */
	uint8_t age;
} ubx_rxmsvsi_SV_packet_t;


/*
	Функция читает из сообщения RXM-SVSI данные по конкретному спутнику
	Аргументы:
	packet    - пакет формата RXM-SVSI
	SV_index  - индекс спутника (от 0 до numSV-1). Количество спутников может быть прочитано
	функцией ubx_parse_rxm_svsi_SV_num
	SV_packet - указатель на структуру, в которую будет записан пакет.
	Возвращает: 0 если чтение произошло успеншно и EADDRNOTAVAIL если индекс был задан неверно.
*/
int ubx_parse_rxm_svsi_SV(const ubx_rxmsvsi_packet_t * packet, uint8_t SV_index, ubx_rxmsvsi_SV_packet_t* SV_packet);


// Возвращает показатель качества (URA) в диапазоне 0..15 из блока данных о спутнике пакета RXM-SVSI
uint8_t ubx_parse_rxm_svsi_SV_ura(const ubx_rxmsvsi_SV_packet_t * SV_packet);

// Возвращает флаг работоспособности спутника из блока данных о спутнике пакета RXM-SVSI
uint8_t ubx_parse_rxm_svsi_SV_healthy(const ubx_rxmsvsi_SV_packet_t * SV_packet);

// Возвращает флаг достоверности эфемерид из блока данных о спутнике пакета RXM-SVSI
uint8_t ubx_parse_rxm_svsi_SV_ephVal(const ubx_rxmsvsi_SV_packet_t * SV_packet);

// Возвращает флаг достоверности альманаха из блока данных о спутнике пакета RXM-SVSI
uint8_t ubx_parse_rxm_svsi_SV_almVal(const ubx_rxmsvsi_SV_packet_t * SV_packet);

// Возвращает флаг доступности спутника из блока данных о спутнике пакета RXM-SVSI
uint8_t ubx_parse_rxm_svsi_SV_notAvail(const ubx_rxmsvsi_SV_packet_t * SV_packet);

// Возвращает возраст ALM (альманах) в днях со смещением 4 из блока данных о спутнике пакета RXM-SVSI
uint8_t ubx_parse_rxm_svsi_SV_almAge(const ubx_rxmsvsi_SV_packet_t * SV_packet);

// Возвращает возраст EPH (эфемериды) в часах со смещением 4 из блока данных о спутнике пакета RXM-SVSI
uint8_t ubx_parse_rxm_svsi_SV_ephAge(const ubx_rxmsvsi_SV_packet_t * SV_packet);


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// NAV-SVSI
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//! Флаги разного использования спутника в сообщении NAV_SV_INFO
typedef enum ubx_nav_svinfo_flags_t
{
	//! Спутник используется для навигации
	UBX_NAVSVINFO_FLAGS__SV_USED	= 0x01 << 0,
	//! Для спутника есть параметры диф коррекции
	UBX_NAVSVINFO_FLAGS__DIFF_COR	= 0x01 << 1,
	//! Для спутника есть параметры орбиты
	UBX_NAVSVINFO_FLAGS__ORB_AVAIL	= 0x01 << 2,
	//! Параметры орбиты из эфемирид
	UBX_NAVSVINFO_FLAGS__ORB_EPH	= 0x01 << 3,
	//! Спутник вреден для здоровья и не должен использоваться
	UBX_NAVSVINFO_FLAGS__UNHEALTHY	= 0x01 << 4,
	//! Параметры орбиты из альманаха плюс
	UBX_NAVSVINFO_FLAGS__ORB_ALM	= 0x01 << 5,
	//! Параметры орбиты из автономного AssistNow
	UBX_NAVSVINFO_FLAGS__ORB_AOP	= 0x01 << 6,
	//! Используется какое-то сглаживание несущей (пишут смотри при Precise Point Smoothing)
	UBX_NAVSVINFO_FLAGS__SMOOTH		= 0x01 << 7,
} ubx_nav_svinfo_flags_t;


// Space Vehicle Information
typedef struct ubx_navsvinfo_packet_t
{
	/* Время недели GPS для эпохи навигации (см. "6.2. Navigation Epoch" и
	"6.3. Метки времени iTOW" [2] для получения дополнительной информации).*/
	uint32_t iTOW;
	/* Количество блоков данных, следующих далее, по одному блоку на канал.*/
	uint8_t numCh;
	/* Количество видимых спутников*/
	uint8_t globalFlags;
	/* Начало повторяющихся блоков(повторяются numSV раз) */
	const uint8_t* CHbuf_ptr;
} ubx_navsvinfo_packet_t;


// Space Vehicle Information информация о каналах
typedef struct ubx_navsvinfo_CH_packet_t
{
	/* Номер канала*/
	uint8_t chn;
	/* Идентификатор спутника */
	uint8_t svid;
	/* Информационные флаги (ubx_nav_svinfo_flags_t)*/
	uint8_t flags;
	/* Информационные флаги */
	uint8_t quality;
	/* Мощность сигнала */
	uint8_t cno;
	/* Возвышение */
	int8_t elev;
	/* Азимут */
	int16_t azim;
	/* Pseudo range residual in centimetres */
	int32_t prRes;
} ubx_navsvinfo_CH_packet_t;


// Возвращает количество блоков данных, следующих далее, по одному блоку на канал.
uint8_t ubx_parse_nav_svinfo_CH_num(const ubx_navsvinfo_packet_t * packet);

/*
	Функция читает из сообщения NAV-SVINFO данные по конкретному каналу
	Аргументы:
	packet    - пакет формата NAV-SVINFO
	SV_index  - индекс канала (от 0 до numCh-1). Количество каналов может быть прочитано
	функцией ubx_parse_nav_svinfo_CH_num
	CH_packet - указатель на структуру, в которую будет записан пакет.
	Возвращает: 0 если чтение произошло успеншно и EADDRNOTAVAIL если индекс был задан неверно.
*/
int ubx_parse_nav_svinfo_CH(const ubx_navsvinfo_packet_t * packet, uint8_t CH_index, ubx_navsvinfo_CH_packet_t* CH_packet);

uint8_t ubx_parse_nav_svinfo_chipGen(const ubx_navsvinfo_packet_t * packet);

uint8_t ubx_parse_nav_svinfo_CH_svUsed(const ubx_navsvinfo_CH_packet_t * CH_packet);

uint8_t ubx_parse_nav_svinfo_CH_diffCorr(const ubx_navsvinfo_CH_packet_t * CH_packet);

uint8_t ubx_parse_nav_svinfo_CH_orbitAvail(const ubx_navsvinfo_CH_packet_t * CH_packet);

uint8_t ubx_parse_nav_svinfo_CH_orbitEph(const ubx_navsvinfo_CH_packet_t * CH_packet);

uint8_t ubx_parse_nav_svinfo_CH_unhealthy(const ubx_navsvinfo_CH_packet_t * CH_packet);

uint8_t ubx_parse_nav_svinfo_CH_orbitAlm(const ubx_navsvinfo_CH_packet_t * CH_packet);

uint8_t ubx_parse_nav_svinfo_CH_orbitAop(const ubx_navsvinfo_CH_packet_t * CH_packet);

uint8_t ubx_parse_nav_svinfo_CH_smoothed(const ubx_navsvinfo_CH_packet_t * CH_packet);

uint8_t ubx_parse_nav_svinfo_CH_quality(const ubx_navsvinfo_CH_packet_t * CH_packet);


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//! Структура, включающая данные любого пакета
typedef struct ubx_any_packet_t
{
	ubx_pid_t pid;

	union
	{
		ubx_gpstime_packet_t gpstime;
		ubx_timtp_packet_t timtp;
		ubx_navsol_packet_t navsol;
		ubx_ack_packet_t ack;
		ubx_nack_packet_t nack;
		ubx_monhw2_packet_t monhw2;
		ubx_monhw_packet_t monhw;
		ubx_rxmsvsi_packet_t rxmsvsi;
		ubx_navsvinfo_packet_t navsvinfo;
	} packet;
} ubx_any_packet_t;


//! Извлечение идентификатора пакета из заголовка собщения
uint16_t ubx_packet_pid(const uint8_t * packet_header);

//! Извлечение длины пакета из заголовка
uint16_t ubx_packet_payload_size(const uint8_t * packet_header);


//! Вычисление контрольной суммы по ubx алгоритму
uint16_t ubx_packet_checksum(const uint8_t * data_start, int data_size);


//! Сборка uint16_t crc значения из отдельных crc-a и crc-b значений
uint16_t ubx_uint16crc_make(uint8_t crc_a, uint8_t crc_b);

//! Выделение crca из 16ти битного представления crc
uint8_t ubx_uint16crc_get_crca(uint16_t crc16);

//! Выделение crcb из 16ти битного представления crc
uint8_t ubx_uint16crc_get_crcb(uint16_t crc16);


//! Сборка PID значения из отдельных значений class и packet_id
uint16_t ubx_make_pid(uint8_t packet_class, uint8_t packet_id);


//! Разбор тела пакета
int ubx_parse_any_packet(const uint8_t * packet_start, ubx_any_packet_t * packet);


#endif /* INC_UBX_PACKET_PARSER_H_ */
