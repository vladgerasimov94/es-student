#include "platform/platform.h"

#include "pico/stdlib.h"

#define LED_PIN PICO_DEFAULT_LED_PIN
#define BUTTON_PIN 15u

void platform_init(void)
{
    stdio_init_all();
    gpio_init_mask((1u << LED_PIN) | (1u << BUTTON_PIN));
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_pull_up(BUTTON_PIN);
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on);
}

bool platform_button_read(void)
{
    return !gpio_get(BUTTON_PIN);
}
