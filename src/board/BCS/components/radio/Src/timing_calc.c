#include "../Inc_private/timing_calc.h"

#define RADIO_TIMING_PANIC 5

#define RADIO_TX_COUNT_LOWER_BOUND 30
#define RADIO_BUFFERED_COUNT 5
#define RADIO_WAIT_PERIOD_START 10 //ms
#define BUFFERED_COUNT_RECALCULATE_PERIOD 3

void timing_calc_init(radio_t* server, radio_timings_t* state) {
    server->wait_period = RADIO_WAIT_PERIOD_START;
    state->buffered_packet_count = RADIO_BUFFERED_COUNT;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
void timing_calc_finish_all_tx(radio_t* server, radio_timings_t* state) {
	uint32_t cur_left_count = ring_buffer_get_avail(&server->radio_ring_buffer);
	uint32_t last_left_count = state->last_left_count;

	uint32_t tx_count = RADIO_TX_COUNT_LOWER_BOUND;
	if (tx_count < state->tx_count) {
		tx_count = state->tx_count;
	}
	uint32_t wait_period = 0;
	int32_t expected_left_count_next = cur_left_count - tx_count;
	if (expected_left_count_next >= state->buffered_packet_count) {
		wait_period = 0xFFFFFFFF;
	} else {
		uint32_t to_fill = RADIO_BUFFERED_COUNT - expected_left_count_next;
		wait_period = (RADIO_TX_PERIOD + RADIO_RX_PERIOD) / to_fill;
	}
	server->wait_period = wait_period;
	state->tx_count = 0;
    if (state->big_tx_count < BUFFERED_COUNT_RECALCULATE_PERIOD) {
        state->big_tx_count++;
    } else {
        int32_t buffer_count_new = state->buffered_packet_count + RADIO_BUFFERED_COUNT - state->min_tx_left;
        buffer_count_new = MAX(buffer_count_new, 0);
        log_trace("RADIO timing: buffered_packet_count was = %d now = %d min left = %d", state->buffered_packet_count, buffer_count_new, state->min_tx_left);
        state->buffered_packet_count = buffer_count_new;

        state->big_tx_count = 0;
        state->min_tx_left = RADIO_TX_COUNT;
    }
}


void timing_calc_finish_one_tx(radio_t* server, radio_timings_t* state) {
	uint32_t cur_left_count = ring_buffer_get_avail(&server->radio_ring_buffer);
    state->tx_count++;
    
    state->min_tx_left = MIN(state->min_tx_left, cur_left_count);
}