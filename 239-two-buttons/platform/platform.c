#include "platform/platform.h"

#include "pico/stdlib.h"

#define LED_PIN PICO_DEFAULT_LED_PIN
#define BUTTON_1_PIN 15u
#define BUTTON_2_PIN 14u

static const uint button_pins[PLATFORM_BUTTON_COUNT] = {
    [PLATFORM_BUTTON_1] = BUTTON_1_PIN,
    [PLATFORM_BUTTON_2] = BUTTON_2_PIN,
};

void platform_init(void)
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    for (platform_button_t button = PLATFORM_BUTTON_1; button < PLATFORM_BUTTON_COUNT; button++)
    {
        gpio_init(button_pins[button]);
        gpio_pull_up(button_pins[button]);
    }
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on);
}

bool platform_button_read(platform_button_t button)
{
    return !gpio_get(button_pins[button]);
}
