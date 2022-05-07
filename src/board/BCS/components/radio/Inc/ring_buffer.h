/*
 * ring_buffer.h
 *
 *  Created on: May 7, 2022
 *      Author: sereshotes
 */

#ifndef COMPONENTS_RADIO_INC_RING_BUFFER_H_
#define COMPONENTS_RADIO_INC_RING_BUFFER_H_

#include <stddef.h>
#include <stdbool.h>

typedef struct _ring_buffer_t {
	size_t count;
	size_t put;
	size_t get;
	bool is_full;
	void* data;
	size_t one_size;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t* rb, void* data, size_t count, size_t one_size);

size_t ring_buffer_put_avail(ring_buffer_t* rb);

size_t ring_buffer_get_avail(ring_buffer_t* rb);

void ring_buffer_get_advance(ring_buffer_t* rb);

void ring_buffer_put_advance(ring_buffer_t* rb);

void* ring_buffer_put(ring_buffer_t* rb);

void* ring_buffer_get(ring_buffer_t* rb);


#endif /* COMPONENTS_RADIO_INC_RING_BUFFER_H_ */
