#ifndef EVENT_HANDLER
#define EVENT_HANDLER

typedef enum {
    USLP_EVENT_SENT_FRAME,
    USLP_EVENT_VC_LOST_FRAMES,
    USLP_EVENT_MAPP_WRONG_PVN,
    USLP_EVENT_MAPP_BAD_LENGTH,
    USLP_EVENT_MAPP_NO_START,
    USLP_EVENT_MAPA_NO_FINISH
} uslp_event_t;

typedef struct {
    uint8_t* data;
    size_t size;
} uslp_event_mnf_t;

void uslp_event_handler(uslp_event_t event, const void* event_data);

#endif /* EVENT_HANDLER */
