/*
 * ring_buffer.c
 *
 *  Created on: May 7, 2022
 *      Author: sereshotes
 */


#include "ring_buffer.h"


void ring_buffer_init(ring_buffer_t* rb, void* data, size_t count, size_t one_size) {
	rb->count = count;
	rb->one_size = one_size;
	rb->data = data;
	rb->put = 0;
	rb->get = 0;
	rb->is_full = 0;
}

size_t ring_buffer_put_avail(ring_buffer_t* rb) {
	if (rb->put == rb->get) {
		return rb->is_full ? 0 : rb->count;
	}
	return (rb->count + rb->get - rb->put) % rb->count;
}
size_t ring_buffer_get_avail(ring_buffer_t* rb) {
	if (rb->put == rb->get) {s
		return rb->is_full ? rb->count : 0;
	}
	return (rb->count + rb->put - rb->get) % rb->count;
}

void ring_buffer_get_advance(ring_buffer_t* rb) {
	rb->get++;
	rb->get %= rb->count;
	rb->is_full = 0;
}

void ring_buffer_put_advance(ring_buffer_t* rb) {
	assert(!rb->is_full);
	rb->put++;
	rb->put %= rb->count;
	if (rb->put == rb->get) {
		rb->is_full = 1;
	}
}

void* ring_buffer_put(ring_buffer_t* rb) {
	return rb->data + rb->one_size * rb->put;
}

void* ring_buffer_get(ring_buffer_t* rb) {
	return rb->data + rb->one_size * rb->get;
}

void ring_buffer_reset(ring_buffer_t* rb) {
	rb->put = 0;
	rb->get = 0;
	rb->is_full = 0;
}

