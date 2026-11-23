#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PI_STATE_NOT_STARTED,
    PI_STATE_RUNNING,
    PI_STATE_READY,
} pi_state_t;

bool pi_task_start(uint32_t n);
void pi_task_handle(void);
pi_state_t pi_task_get_state(void);
bool pi_task_get_result(double *result, uint64_t *spent_us);
