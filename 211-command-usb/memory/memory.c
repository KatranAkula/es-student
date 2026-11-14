#include "memory.h"                         // мой заголовок: объявление mem_info

#include <stdio.h>                          // для printf
#include <stdint.h>                         // для uintptr_t (целое, вмещающее адрес)
#include "hardware/regs/addressmap.h"       // для XIP_BASE, SRAM_BASE, ROM_BASE
#include "pico/stdlib.h"                    // для PICO_FLASH_SIZE_BYTES (размер флеш платы)

// ===== ШАГ 2: символы линкера (только объявления, памяти под ними нет) =====
extern char __flash_binary_start;           // начало образа во флеш
extern char __flash_binary_end;             // конец образа во флеш объявление без & в mem_info
extern char __boot2_start__;                // начало загрузчика второй стадии
extern char __boot2_end__;                  // конец загрузчика второй стадии
extern char __etext;                        // конец кода и констант (начало хранения .data)
extern char __data_start__;                 // начало .data в ОЗУ
extern char __data_end__;                   // конец .data в ОЗУ
extern char __bss_start__;                  // начало .bss в ОЗУ
extern char __bss_end__;                    // конец .bss в ОЗУ
extern char __HeapLimit;                    // верхняя граница кучи
extern char __StackBottom;                  // нижняя граница стека
extern char __StackTop;                     // вершина стека

// ===== Вспомогательная функция: печать одной строки таблицы =====
static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

// ===== ШАГ 3: mem_info — таблица + итог =====
void mem_info(void)
{
    // шапка таблицы: область, начало, конец, размер
    printf("%-10s %-10s %-10s %8s\n", "area", "start", "end", "size");

    // --- три области, заданные чипом (не линкером) ---
    // flash: базовый адрес XIP_BASE, размер из PICO_FLASH_SIZE_BYTES
    row("flash", (uintptr_t)XIP_BASE, (uintptr_t)XIP_BASE + PICO_FLASH_SIZE_BYTES);

    // sram: базовый адрес SRAM_BASE, размер 264 КБ (из datasheet)
    row("sram", (uintptr_t)SRAM_BASE, (uintptr_t)SRAM_BASE + (264 * 1024));

    // rom: базовый адрес ROM_BASE, размер 16 КБ (из datasheet)
    row("rom", (uintptr_t)ROM_BASE, (uintptr_t)ROM_BASE + (16 * 1024));

    // --- области, заданные линкером ---
    // image: весь образ во флеш
    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end); //здесь & — берём адрес, а не значение

    // free: свободное место во флеш (от конца образа до конца флеш)
    row("free", (uintptr_t)&__flash_binary_end, (uintptr_t)XIP_BASE + PICO_FLASH_SIZE_BYTES);

    // boot2: загрузчик второй стадии (ровно 256 байт)
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);

    // text: код и константы (от конца boot2 до __etext)
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);

    // data flash: хранение .data во флеш (от __etext, длиной с .data)
    row("data flash", (uintptr_t)&__etext,
        (uintptr_t)&__etext + ((uintptr_t)&__data_end__ - (uintptr_t)&__data_start__));

    // data ram: работа .data в ОЗУ
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);

    // bss: неинициализированные данные в ОЗУ
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);

    // heap: куча (от конца .bss до __HeapLimit)
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);

    // stack: стек (от __StackBottom до __StackTop)
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    // --- итог ---
    unsigned boot2_size = (unsigned)(&__boot2_end__ - &__boot2_start__);
    unsigned text_size  = (unsigned)(&__etext - &__boot2_end__);
    unsigned data_size  = (unsigned)(&__data_end__ - &__data_start__);
    unsigned bss_size   = (unsigned)(&__bss_end__ - &__bss_start__);

    unsigned image_size = (unsigned)(&__flash_binary_end - &__flash_binary_start);
    unsigned flash_size = (unsigned)PICO_FLASH_SIZE_BYTES;

    printf("\ntotal\n");
    printf("  flash image  %7u = boot2 %u + text %u + data %u\n",
           image_size, boot2_size, text_size, data_size);
    printf("  flash free   %7u of %u\n",
           flash_size - image_size, flash_size);
    printf("  ram used     %7u = data %u + bss %u\n",
           data_size + bss_size, data_size, bss_size);
    printf("  ram free     %7u for heap and %u for stack\n",
           (unsigned)(&__HeapLimit - &__bss_end__),
           (unsigned)(&__StackTop - &__StackBottom));
}