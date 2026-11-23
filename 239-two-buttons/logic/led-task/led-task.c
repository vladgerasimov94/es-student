#include "led-task/led-task.h"

#include "platform/platform.h"
#include "systime/systime.h"

#define LED_DEFAULT_PERIOD_MS 1000u
#define US_PER_HALF_PERIOD_MS 500u

static led_state_t led_state;
static bool led_on;
static uint32_t led_period_ms;
static uint64_t last_toggle_us;

static void set_output(bool on)
{
    led_on = on;
    platform_led_set(on);
}

void led_task_init(void)
{
    led_period_ms = LED_DEFAULT_PERIOD_MS;
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;

    switch (led_state)
    {
    case LED_STATE_OFF:
        set_output(false);
        break;
    case LED_STATE_ON:
        set_output(true);
        break;
    case LED_STATE_BLINK:
        set_output(!led_on);
        last_toggle_us = systime_us();
        break;
    }
}

void led_task_next_state(void)
{
    switch (led_state)
    {
    case LED_STATE_OFF:
        led_task_set_state(LED_STATE_ON);
        break;
    case LED_STATE_ON:
        led_task_set_state(LED_STATE_BLINK);
        break;
    case LED_STATE_BLINK:
        led_task_set_state(LED_STATE_OFF);
        break;
    }
}

void led_task_handle(void)
{
    switch (led_state)
    {
    case LED_STATE_OFF:
    case LED_STATE_ON:
        break;
    case LED_STATE_BLINK:
        if (systime_period_elapsed(&last_toggle_us, (uint64_t)led_period_ms * US_PER_HALF_PERIOD_MS))
        {
            set_output(!led_on);
        }
        break;
    }
}

led_state_t led_task_get_state(void)
{
    return led_state;
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    if (period_ms == 0)
    {
        return false;
    }

    led_period_ms = period_ms;
    return true;
}

void led_task_next_period(void)
{
    switch (led_period_ms)
    {
    case 250u:
        led_task_set_period_ms(500u);
        break;
    case 500u:
        led_task_set_period_ms(1000u);
        break;
    default:
        led_task_set_period_ms(250u);
        break;
    }
}

uint32_t led_task_get_period_ms(void)
{
    return led_period_ms;
}
