#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PROFILING_STOPWATCH_COUNT 5

typedef struct
{
    const char *name;
    uint32_t count;
    float mean_us;
    uint32_t max_us;
} profiling_result_t;

// Имя должно оставаться доступным, пока используется секундомер.
void profiling_stopwatch_init(uint32_t id, const char *name);

// по умолчанию профилирование включено
#ifndef PROFILING_ENABLED
#define PROFILING_ENABLED 1
#endif

#if PROFILING_ENABLED
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
#else
// профилирование выключено: вызовы ничего не делают
#define profiling_start(id) ((void)0)
#define profiling_stop(id) ((void)0)
#endif

// false — секундомер с таким номером не заведён
bool profiling_get(uint32_t id, profiling_result_t *result);

// 32-битная сумма переполняется примерно через 71 минуту накопленного времени.
// Сброс сохраняет имена секундомеров и отменяет незавершённые замеры.
void profiling_reset(void);
