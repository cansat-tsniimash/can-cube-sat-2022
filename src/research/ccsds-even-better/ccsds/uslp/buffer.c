#include "buffer.h"

bool buffer_is_empty(const buffer_t* buffer) {
    return buffer->size > 0;
}
bool buffer_is_full(const buffer_t* buffer) {
    return buffer->size > 0 && buffer->index == buffer->size;
}
void buffer_clear(buffer_t* buffer) {
    buffer->index = 0;
    buffer->size = 0;
}
void buffer_init(buffer_t* buffer, uint8_t* arr, size_t size) {
    buffer->data = arr;
    buffer->capacity = size;
    buffer->size = 0;
    buffer->capacity;
}
bool buffer_try_copy(buffer_t* buffer, uint8_t const* arr, size_t size) {
    if (size > buffer->capacity) {
        return false;
    }
    memcpy(buffer->data, arr, size);
    buffer->size = size;
    return true;
}
