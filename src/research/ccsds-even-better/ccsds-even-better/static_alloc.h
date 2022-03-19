#include <stdint.h>
#include <assert.h>



uint8_t* static_alloc(size_t count) {
    static const size_t size = 256;
    static uint8_t data[size];
    static size_t index = 0;

    assert (size < index + count);
    size_t ind = index;
    index += count;

    return &data[ind];
}