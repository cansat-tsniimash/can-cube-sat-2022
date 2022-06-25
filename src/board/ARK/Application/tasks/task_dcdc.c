#include <stdio.h>
#include "main.h"

#include "assert.h"
#include "task.h"
#include <mp5515.h>
#include <task_dcdc.h>
#include <task_ina.h>

typedef enum {
    mp5515_error_state_ok,
    mp5515_error_state_bad,
} mp5515_error_state_t;

typedef struct {
    mp5515_error_state_t state;
    uint32_t last_time;
    uint32_t count;
} mp5515_error_log_t;

typedef enum {
    mp5515_error_io_fail                = mp5515_event_io_fail,
    mp5515_error_input_over_current     = mp5515_event_input_over_current,
    mp5515_error_input_over_voltage     = mp5515_event_input_over_voltage,
    mp5515_error_input_power_fail       = mp5515_event_input_power_fail,
    mp5515_error_pgs_not_ok             = mp5515_event_pgs_not_ok,
    mp5515_error_temp_warn              = mp5515_event_temp_warn,
    mp5515_error_die_temp_warn          = mp5515_event_die_temp_warn,
    mp5515_error_forced_buck            = mp5515_event_forced_buck,
    mp5515_error_sas_disable_supply     = mp5515_event_sas_disable_supply,
    mp5515_error_pgb_not_ok             = mp5515_event_pgb_not_ok,
    mp5515_error_isofet_off             = mp5515_event_isofet_off,
    mp5515_error_timeout                = mp5515_event_timeout,

    mp5515_error_pfi = mp5515_event_error_count,
    mp5515_error_pgb,
    mp5515_error_pgs,
    mp5515_error_int,

    mp5515_error_type_count
} mp5515_error_t;

const char* _error_name(mp5515_error_t error) {
    switch (error) {
    case mp5515_error_io_fail:                  return "io fail";
    case mp5515_error_input_over_current:       return "input over current";
    case mp5515_error_input_over_voltage:       return "input over voltage";
    case mp5515_error_input_power_fail:         return "input power fail";
    case mp5515_error_pgs_not_ok:               return "pgs not ok";
    case mp5515_error_temp_warn:                return "temp warn";
    case mp5515_error_die_temp_warn:            return "die temp warn";
    case mp5515_error_forced_buck:              return "forced buck";
    case mp5515_error_sas_disable_supply:       return "sas disable supply";
    case mp5515_error_pgb_not_ok:               return "pgb not ok";
    case mp5515_error_isofet_off:               return "isofet off";
    case mp5515_error_timeout:                  return "timeout";
    default:
        return 0;
    }
}

static mp5515_error_log_t errors[dcdc_type_count][mp5515_error_type_count];
static int need_to_get_events[dcdc_type_count] = {0};
static mp5515_t mp5515_dev[dcdc_type_count];

void task_dcdc_exti_cb(mp5515_pin_t pin, dcdc_type_t dcdc_type) {
    need_to_get_events[dcdc_type] = 1;

    mp5515_error_log_t* cur = 0;
    switch (pin) {
    case mp5515_pin_int:
        cur = &errors[dcdc_type][mp5515_error_int];
        break;
    case mp5515_pin_pfi:
        cur = &errors[dcdc_type][mp5515_error_pfi];
        break;
    case mp5515_pin_pgb:
        cur = &errors[dcdc_type][mp5515_error_pgb];
        break;
    case mp5515_pin_pgs:
        cur = &errors[dcdc_type][mp5515_error_pgs];
        break;
    default:
        assert(0 && "What a hell with mp5515 pin type?");
        break;
    }

    cur->count++;
    cur->last_time = HAL_GetTick();
    cur->state = mp5515_error_state_bad;
}

void task_dcdc_init(void* arg) {
    mp5515_init(&mp5515_dev[dcdc_type_charge]);
    mp5515_init(&mp5515_dev[dcdc_type_discharge]);
}

static void _event_to_error_log(dcdc_type_t dcdc_type, mp5515_event_t event) {
    for (int i = 0; i < mp5515_error_type_count; i++) {
        if (event & (1 << i)) {
            if (errors[dcdc_type][1 << i].state == mp5515_error_state_ok) {
                errors[dcdc_type][1 << i].state = mp5515_error_state_bad;
                errors[dcdc_type][1 << i].count++;
            }
            errors[dcdc_type][1 << i].last_time = HAL_GetTick();
        } else {
            errors[dcdc_type][1 << i].state = mp5515_error_state_ok;
        }
    }

}

void task_dcdc_update(void* arg) {
    if (need_to_get_events[dcdc_type_charge]) {
        need_to_get_events[dcdc_type_charge] = 0;
        mp5515_event_t event = 0;
        mp5515_get_events(&mp5515_dev[dcdc_type_charge], &event);
        _event_to_error_log(dcdc_type_charge, event);
    }
    if (need_to_get_events[dcdc_type_discharge]) {
        need_to_get_events[dcdc_type_discharge] = 0;
        mp5515_event_t event = 0;
        mp5515_get_events(&mp5515_dev[dcdc_type_discharge], &event);
        _event_to_error_log(dcdc_type_discharge, event);
    }
    tina_value_t* value = 0;
    int* valid = 0;
    int count = tina219_get_value(&value, &valid);
    if (count == 2) {
        // TODO: На основании напряжений, токов можно что-то сделать
    }
    // TODO: на основании плохости всего убивать DC-DC


    static uint32_t debug_last_time = 0;
    uint32_t now = HAL_GetTick();
    if (now - debug_last_time < 1000) {
        debug_last_time = now;
        printf("-----------------------------------\n");
        printf("--------- DC-DC DEBUG INFO --------\n");
        printf("-----------------------------------\n");
        printf("Time: %d\n", (int)now);
        printf("%-3s %-20s %-7s %-10s %-10s\n", "[T]", "[Name]", "[State]", "[Count]", "[Last time]");
        for (dcdc_type_t dcdc_type = 0; dcdc_type < dcdc_type_count; dcdc_type++) {
            for (int i = 0; i < mp5515_error_type_count; i++) {
                printf("%-3d %-20s %-7d %-10d %-10d\n", dcdc_type, _error_name(i),
                        errors[dcdc_type][i].state, (int)errors[dcdc_type][i].count, errors[dcdc_type][i].last_time);
            }
        }
    }
}

