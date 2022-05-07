#include "uslp_types.h"
#include "event_handler.h"

void __attribute__((weak)) uslp_event_handler(uslp_event_t event, const void* event_data) {
    (void)event;
    (void)event_data;
}