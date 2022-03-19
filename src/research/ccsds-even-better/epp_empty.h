#pragma once

#include <string.h>

void epp_set_empty_packet(uint8_t* data, size_t size) {
    static const uint8_t pattern[] = {0xFE, 0xED, 0xBA, 0xBE};
    size_t di = 0;
    size_t pi = 0;
    while (di < size) {
        data[di] = pattern[pi];
        di++;
        pi++;
        pi %= sizeof(pattern);
    }
}