/*
 * task_recv.h
 *
 *  Created on: Apr 28, 2020
 *      Author: sereshotes
 */

#ifndef INC_TASK_RECV_H_
#define INC_TASK_RECV_H_

#define TRECV_MAX_CALLBACK_COUNT 5

#include "mavlink_help2.h"

void task_recv_init(void *arg);
void task_recv_update(void *arg);



#endif /* INC_TASK_RECV_H_ */
