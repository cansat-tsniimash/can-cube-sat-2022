#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t size;
    size_t head;
    size_t tail;
    bool is_full;
} fifo_t;

void fifo_init(fifo_t* fifo, uint8_t* data, size_t size);
void fifo_push(fifo_t* fifo, uint8_t byte);
void fifo_pull(fifo_t* fifo, uint8_t* byte);
size_t fifo_fullness(const fifo_t* fifo);
size_t fifo_size(const fifo_t* fifo);
size_t fifo_free_space(const fifo_t* fifo);
bool fifo_push_arr(fifo_t* fifo, const uint8_t* data, size_t size);
bool fifo_pull_arr(fifo_t* fifo, uint8_t* data, size_t size);