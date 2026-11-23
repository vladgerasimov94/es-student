#include "pi-task/pi-task.h"

#include "systime/systime.h"

// 585 тактов / 125 МГц ≈ 4,7 мкс на член; 100 членов ≈ 470 мкс.
#define PI_TERMS_PER_CALL 100

// Состояние расчёта: должно пережить возврат из обработчика
static pi_state_t pi_state = PI_STATE_NOT_STARTED;
static uint32_t pi_k;
static uint32_t pi_n;
static double pi_sum;
static double pi_sign;
static double pi_result;
static uint64_t pi_start_us;
static uint64_t pi_end_us;

bool pi_task_start(uint32_t n)
{
    if (pi_state == PI_STATE_RUNNING) {
        return false;
    }

    pi_n = n;
    pi_k = 0;
    pi_sum = 0.0;
    pi_sign = 1.0;
    pi_start_us = systime_us();
    pi_state = PI_STATE_RUNNING;
    return true;
}

void pi_task_handle(void)
{
    if (pi_state != PI_STATE_RUNNING) {
        return;
    }

    // Очередная порция членов ряда, но не дальше последнего
    uint32_t end = pi_k + PI_TERMS_PER_CALL;
    if (end < pi_k || end > pi_n) {
        end = pi_n;
    }
    for (; pi_k < end; pi_k++) {
        pi_sum += pi_sign / (2.0 * pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    if (pi_k == pi_n) {
        pi_result = 4.0 * pi_sum;
        pi_end_us = systime_us();
        pi_state = PI_STATE_READY;
    }
}

pi_state_t pi_task_get_state(void)
{
    return pi_state;
}

// Готов ли результат: если да, записать его по указателю
bool pi_task_get_result(double *result, uint64_t *spent_us)
{
    if (pi_state != PI_STATE_READY) {
        return false;
    }
    *result = pi_result;
    *spent_us = pi_end_us - pi_start_us;
    return true;
}
