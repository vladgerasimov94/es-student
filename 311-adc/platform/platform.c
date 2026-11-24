#include "platform/platform.h"

#include "hardware/adc.h"
#include "pico/stdlib.h"

#define LED_PIN PICO_DEFAULT_LED_PIN
#define BUTTON_PIN 15u

// вход АЦП: вывод GP26, канал 0
#define ADC_PIN 26
#define ADC_CHANNEL 0

// опорное напряжение АЦП на плате Pico и число уровней 12-разрядного кода
#define ADC_VREF_V 3.3f
#define ADC_LEVELS 4096

void platform_init(void)
{
    stdio_init_all();
    gpio_init_mask((1u << LED_PIN) | (1u << BUTTON_PIN));
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_pull_up(BUTTON_PIN);

    adc_init();
    adc_gpio_init(ADC_PIN);
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on);
}

bool platform_button_read(void)
{
    return !gpio_get(BUTTON_PIN);
}

float platform_voltage_read(void)
{
    adc_select_input(ADC_CHANNEL);
    return adc_read() * ADC_VREF_V / ADC_LEVELS;
}
