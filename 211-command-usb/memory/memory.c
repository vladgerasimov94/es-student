#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "command.h"
#include "device.h"
#include "led.h"

#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

int main(void);

uint32_t data_variable = 100;
uint32_t bss_variable;

#define VECTOR_TABLE 0x10000100

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    // шапка таблицы: область, начало, конец, размер

    row("flash", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("sram",  SRAM_BASE, SRAM_BASE + (264 * 1024));
    row("rom",   ROM_BASE,  ROM_BASE + (16 * 1024));

    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    // row("free", (uintptr_t)&__flash_binary_end, (uintptr_t)&__boot2_start__);
    row("free", (uintptr_t)&__flash_binary_end, XIP_BASE + PICO_FLASH_SIZE_BYTES);

    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);

    // row("data flash", (uintptr_t)&__etext, (uintptr_t)&__data_start__);
    row("data flash", (uintptr_t)&__etext, (uintptr_t)&__etext + ((uintptr_t)&__data_end__ - (uintptr_t)&__data_start__));

    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    // Вычисляем размеры компонентов (в байтах)
    uint32_t flash_total = PICO_FLASH_SIZE_BYTES;
    uint32_t boot2_sz    = (uintptr_t)&__boot2_end__ - (uintptr_t)&__boot2_start__;
    uint32_t text_sz     = (uintptr_t)&__etext - (uintptr_t)&__boot2_end__;
    uint32_t data_sz     = (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__;
    uint32_t bss_sz      = (uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__;
    
    // Вычисляем итоги
    uint32_t flash_image = boot2_sz + text_sz + data_sz;
    uint32_t flash_free  = flash_total - flash_image;
    uint32_t ram_used    = data_sz + bss_sz;
    
    // В RP2040 размер ОЗУ 264 КБ = 270336 байт
    uint32_t ram_total   = 264 * 1024; 
    uint32_t stack_sz    = (uintptr_t)&__StackTop - (uintptr_t)&__StackBottom;
    // Свободно под кучу — это всё, что осталось в RAM между BSS и началом стека
    // uint32_t heap_free   = ram_total - ram_used - stack_sz; 
    uint32_t heap_free   = (uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__;


    // Печатаем блок total
    printf("\ntotal\n");
    printf("  flash image  %7u = boot2 %u + text %u + data %u\n", 
           (unsigned)flash_image, (unsigned)boot2_sz, (unsigned)text_sz, (unsigned)data_sz);
    
    printf("  flash free   %7u of %u\n", 
           (unsigned)flash_free, (unsigned)flash_total);
    
    printf("  ram used     %7u = data %u + bss %u\n", 
           (unsigned)ram_used, (unsigned)data_sz, (unsigned)bss_sz);
    
    printf("  ram free     %7u for heap and %u for stack\n", 
           (unsigned)heap_free, (unsigned)stack_sz);

}

void fw_info(void)
{
    // шапка: объект, адрес, значение

    // считаем вызов: data_variable и bss_variable на единицу больше
    data_variable++;
    bss_variable++;

    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));

    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }

    // main, fw_info  — адрес с признаком Thumb и два байта по сброшенному адресу
    uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    uint16_t *fw_info_code = (uint16_t *)((uintptr_t)fw_info & ~1u);
    printf("main            0x%08x  0x%04x\n", (unsigned)(uintptr_t)main, (unsigned)*main_code);
    printf("fw_info         0x%08x  0x%04x\n", (unsigned)(uintptr_t)fw_info, (unsigned)*fw_info_code);

    printf("commands        0x%08x\n", (unsigned)(uintptr_t)commands);

    for (uint i = 0; i < command_count; i++)
    {
        printf("- %-14s 0x%08x\n",
            commands[i].name,
            (unsigned)(uintptr_t)commands[i].handler);
    }

    printf("DEVICE_PROJECT  0x%08x %s\n", (unsigned)(uintptr_t)&DEVICE_PROJECT, DEVICE_PROJECT);
    printf("DEVICE_BOARD    0x%08x %s\n", (unsigned)(uintptr_t)&DEVICE_BOARD, DEVICE_BOARD);

    printf("data_variable   0x%08x %u\n", (unsigned)(uintptr_t)&data_variable, (unsigned)data_variable);
    printf("bss_variable    0x%08x %u\n", (unsigned)(uintptr_t)&bss_variable, (unsigned)bss_variable);

    printf("stack_variable  0x%08x %u\n", (unsigned)(uintptr_t)&stack_variable, (unsigned)stack_variable);

    if (heap_variable != NULL)
    {
        printf("heap_variable   0x%08x %u\n", (unsigned)(uintptr_t)heap_variable, (unsigned)*heap_variable);

        free(heap_variable);
    }
}

void boot_info(void)
{
    const uint32_t *vectors = (const uint32_t *)VECTOR_TABLE;

    uint32_t stack_top = vectors[0];
    uint32_t reset_handler = vectors[1];
    uint32_t reset_handler_even = reset_handler & ~1u;

    volatile uint32_t *gpio_in = (uint32_t *)(SIO_BASE + SIO_GPIO_IN_OFFSET);
    uint32_t level = (*gpio_in >> led_pin()) & 1u;
    uint32_t sdk_level = gpio_get(led_pin());

    printf("vector table 0x%08x\n", (unsigned)(uintptr_t)vectors);

    printf("  stack top 0x%08x\n", (unsigned)stack_top);
    printf("  reset 0x%08x\n", (unsigned)reset_handler);
    printf("  reset (even) 0x%08x\n", (unsigned)(uintptr_t)reset_handler_even);

    printf("gpio in 0x%08x\n", (unsigned)(uintptr_t)gpio_in);
    printf("  led bit %u\n", (unsigned)level);
    printf("  gpio_get %u\n", (unsigned)sdk_level);
}
