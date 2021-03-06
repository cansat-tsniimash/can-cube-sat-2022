#include "main.h"
#include "its-time.h"
#include <stdio.h>
#include <stdlib.h>


static its_time_t last_exti;
static volatile int is_intertupted = 0;

void its_sync_time_exti_cb() {
    is_intertupted = 1;
    its_gettimeofday(&last_exti);
}


void its_sync_time(its_time_t *from_bcs) {
    if (!is_intertupted) {
        return;
    };
    is_intertupted = 0;


    its_time_t now;
    its_gettimeofday(&now);


    static int bad_time = 0;
    static int64_t big_diff = 0;
    int64_t diff = -(last_exti.sec * 1000 + last_exti.msec) + (from_bcs->sec * 1000 + from_bcs->msec);

    if (llabs(diff) > 5000) {
        uint64_t new = now.sec * 1000 + now.msec + diff;

        ist_count__time_syncs_count();
        its_set_correction_delta_time(HAL_GetTick(), diff / 1000, (diff % 1000) * 1000);

        now.sec = new / 1000;
        now.msec = new % 1000;
        its_settimeofday(&now);

        bad_time = 0;
        big_diff = 0;
    } else if (llabs(diff) > 200) {
        bad_time++;
        big_diff += diff;

        its_count__time_sync_attempts();
    } else {
        diff = 0;
        bad_time = 0;
        its_count__time_sync_attempts();
    }
    if (bad_time >= 5) {
        uint64_t new = now.sec * 1000 + now.msec + big_diff / bad_time;

        ist_count__time_syncs_count();
        its_set_correction_delta_time(HAL_GetTick(), diff / 1000, diff % 1000);

        now.sec = new / 1000;
        now.msec = new % 1000;
        its_settimeofday(&now);


        bad_time = 0;
        big_diff = 0;
    }

}
