#include "button-task/button-task.h"

#include <stddef.h>
#include "systime/systime.h"

#define BUTTON_DEBOUNCE_US 20000
#define BUTTON_LONG_PRESS_US 1000000

typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESS_PENDING,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASE_PENDING,
} button_state_t;

typedef struct
{
    button_state_t button_state;
    uint64_t level_changed_us;
    uint64_t pressed_us;
    uint32_t press_count;
    button_task_callback_t press_callback;
    button_task_callback_t long_press_callback;
    bool long_press_reported;
} button_data_t;

static button_data_t buttons[PLATFORM_BUTTON_COUNT];

void button_task_init(platform_button_t button, button_task_callback_t on_press,
                      button_task_callback_t on_long_press)
{
    buttons[button] = (button_data_t){
        .button_state = BUTTON_STATE_RELEASED,
        .press_callback = on_press,
        .long_press_callback = on_long_press,
    };
}

static void handle_button(button_data_t *data, bool pressed, uint64_t now_us)
{
    switch (data->button_state)
    {
    case BUTTON_STATE_RELEASED:
        if (pressed)
        {
            data->level_changed_us = now_us;
            data->button_state = BUTTON_STATE_PRESS_PENDING;
        }
        break;
    case BUTTON_STATE_PRESS_PENDING:
        if (!pressed)
        {
            data->button_state = BUTTON_STATE_RELEASED;
        }
        else if (now_us - data->level_changed_us >= BUTTON_DEBOUNCE_US)
        {
            data->button_state = BUTTON_STATE_PRESSED;
            data->pressed_us = now_us;
            data->long_press_reported = false;
            data->press_count++;
            if (data->long_press_callback == NULL && data->press_callback != NULL)
            {
                data->press_callback();
            }
        }
        break;
    case BUTTON_STATE_PRESSED:
        if (!pressed)
        {
            data->level_changed_us = now_us;
            data->button_state = BUTTON_STATE_RELEASE_PENDING;
        }
        else if (data->long_press_callback != NULL && !data->long_press_reported &&
                 now_us - data->pressed_us >= BUTTON_LONG_PRESS_US)
        {
            data->long_press_reported = true;
            data->long_press_callback();
        }
        break;
    case BUTTON_STATE_RELEASE_PENDING:
        if (pressed)
        {
            data->button_state = BUTTON_STATE_PRESSED;
        }
        else if (now_us - data->level_changed_us >= BUTTON_DEBOUNCE_US)
        {
            data->button_state = BUTTON_STATE_RELEASED;
            if (data->long_press_callback != NULL && !data->long_press_reported &&
                data->press_callback != NULL)
            {
                data->press_callback();
            }
        }
        break;
    }
}

void button_task_handle(void)
{
    uint64_t now_us = systime_us();
    for (platform_button_t button = PLATFORM_BUTTON_1; button < PLATFORM_BUTTON_COUNT; button++)
    {
        handle_button(&buttons[button], platform_button_read(button), now_us);
    }
}

bool button_task_is_pressed(platform_button_t button)
{
    button_state_t state = buttons[button].button_state;
    return state == BUTTON_STATE_PRESSED || state == BUTTON_STATE_RELEASE_PENDING;
}

uint32_t button_task_get_press_count(platform_button_t button)
{
    return buttons[button].press_count;
}
