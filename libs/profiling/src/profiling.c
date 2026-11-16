#include "profiling/profiling.h"

#include <stddef.h>
#include "pico/time.h"

typedef struct
{
    const char *name;
    uint32_t started_us;
    uint32_t count;
    uint32_t sum_us;
    uint32_t max_us;
    bool running;
} stopwatch_t;

static stopwatch_t stopwatches[PROFILING_STOPWATCH_COUNT];

void profiling_stopwatch_init(uint32_t id, const char *name)
{
    if (id < PROFILING_STOPWATCH_COUNT)
    {
        stopwatches[id] = (stopwatch_t){ .name = name };
    }
}

#if PROFILING_ENABLED
void profiling_start(uint32_t id)
{
    if (id < PROFILING_STOPWATCH_COUNT && stopwatches[id].name != NULL)
    {
        stopwatches[id].started_us = time_us_32();
        stopwatches[id].running = true;
    }
}

void profiling_stop(uint32_t id)
{
    uint32_t now_us = time_us_32();
    if (id >= PROFILING_STOPWATCH_COUNT || !stopwatches[id].running)
    {
        return;
    }

    stopwatch_t *stopwatch = &stopwatches[id];
    uint32_t elapsed_us = now_us - stopwatch->started_us;
    stopwatch->running = false;
    stopwatch->count++;
    stopwatch->sum_us += elapsed_us;
    if (elapsed_us > stopwatch->max_us)
    {
        stopwatch->max_us = elapsed_us;
    }
}
#endif

bool profiling_get(uint32_t id, profiling_result_t *result)
{
    if (id >= PROFILING_STOPWATCH_COUNT || stopwatches[id].name == NULL)
    {
        return false;
    }

    const stopwatch_t *stopwatch = &stopwatches[id];
    result->name = stopwatch->name;
    result->count = stopwatch->count;
    result->mean_us = stopwatch->count ? (float)stopwatch->sum_us / stopwatch->count : 0.0f;
    result->max_us = stopwatch->max_us;
    return true;
}

void profiling_reset(void)
{
    for (uint32_t id = 0; id < PROFILING_STOPWATCH_COUNT; id++)
    {
        stopwatches[id] = (stopwatch_t){ .name = stopwatches[id].name };
    }
}
