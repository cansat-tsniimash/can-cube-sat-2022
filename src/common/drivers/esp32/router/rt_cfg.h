/*
 * rt_cfg.h
 *
 *  Created on: 9 апр. 2020 г.
 *      Author: sereshotes
 */

#ifndef MAIN_ROUTER_RT_CFG_H_
#define MAIN_ROUTER_RT_CFG_H_

#include "mavlink/its/its.h"

#define RT_CFG_LIST(F) \
	F(MAVLINK_MSG_ID_THERMAL_STATE, 		0) \
	F(MAVLINK_MSG_ID_SINS_isc, 				1) \
	F(MAVLINK_MSG_ID_ELECTRICAL_STATE, 		2) \
	F(MAVLINK_MSG_ID_GPS_UBX_NAV_SOL, 		3) \
	F(MAVLINK_MSG_ID_TIMESTAMP,				4) \
	F(MAVLINK_MSG_ID_ROZE_ACTIVATE_COMMAND, 5) \
	F(MAVLINK_MSG_ID_IDLE_COMMAND,			6) \
	F(MAVLINK_MSG_ID_CMD_ACTIVATE_COMMAND,  7)


#endif /* MAIN_ROUTER_RT_CFG_H_ */
