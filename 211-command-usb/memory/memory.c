#include <stdio.h>
#include <stdint.h>
#include "hardware/regs/addressmap.h"
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
