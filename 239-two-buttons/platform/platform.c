#include "platform/platform.h"

#include "pico/stdlib.h"

#define LED_PIN PICO_DEFAULT_LED_PIN
#define BUTTON_PIN 15
#define BUTTON2_PIN 14

static const uint button_pins[PLATFORM_BUTTON_COUNT] = {
    [PLATFORM_BUTTON_1] = BUTTON_PIN,
    [PLATFORM_BUTTON_2] = BUTTON2_PIN,
};

void platform_init(void)
{
    stdio_init_all();
    gpio_init_mask((1u << LED_PIN) | (1u << BUTTON_PIN) | (1u << BUTTON2_PIN));
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_pull_up(BUTTON_PIN);
    gpio_pull_up(BUTTON2_PIN);
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on);
}

bool platform_button_read(platform_button_t button)
{
    return !gpio_get(button_pins[button]);
}
