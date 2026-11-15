#include <stddef.h>
#include "pico/stdlib.h"
#include "api.h"
#include "button-task.h"
#include "led-task.h"
#include "log.h"
#include "profiling.h"
#include "stdio-text-protocol.h"

int main(void)
{
    stdio_init_all();
    log_version();
    led_task_init();
    button_task_init();
    profiling_init();
    stdio_text_protocol_init();

    while (1)
    {
        profiling_iteration();
        led_task_handle();
        button_task_handle();

        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
    }
}
