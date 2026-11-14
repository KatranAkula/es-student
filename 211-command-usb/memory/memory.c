#include "memory.h"                         // мой заголовок: объявление mem_info

#include <stdio.h>                          // для printf
#include <stdint.h>                         // для uintptr_t (целое, вмещающее адрес)
#include <stdlib.h>                         // для malloc/free (2.1.4)
#include "hardware/regs/addressmap.h"       // для XIP_BASE, SRAM_BASE, ROM_BASE
#include "pico/stdlib.h"                    // для PICO_FLASH_SIZE_BYTES (размер флеш платы)
#include "command.h"                        // для commands[], command_count (2.1.4)
#include "device.h"                         // для DEVICE_PROJECT, DEVICE_BOARD (2.1.4)

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

// =========================================================
// 2.1.4: fw_info — адреса кода, констант, переменных, стека и кучи
// =========================================================

// объявление main из другого файла — чтобы взять его адрес
int main(void);

// глобальные переменные для опыта .data / .bss
uint32_t data_variable = 100;   // попадёт в .data (значение хранится во флеше и копируется в ОЗУ)
uint32_t bss_variable;          // попадёт в .bss (в образе нет, startup обнуляет)

void fw_info(void)
{
    // ===== счётчики вызова =====
    data_variable++;
    bss_variable++;

    // ===== адреса функций: сброс Thumb-бита и чтение первого полуслова =====
    // имя функции без скобок — адрес её первой команды (с единицей в младшем разряде)
    // для чтения памяти единицу надо сбросить, иначе HardFault
    uint16_t *main_code   = (uint16_t *)((uintptr_t)main    & ~1u);
    uint16_t *fw_info_code = (uint16_t *)((uintptr_t)fw_info & ~1u);

    // ===== локальная переменная на стеке =====
    uint32_t stack_variable = 1946;

    // ===== блок из кучи =====
    uint32_t *heap_variable = malloc(sizeof(uint32_t));
    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }

    // ===== шапка таблицы =====
    printf("%-15s %-12s %s\n", "object", "address", "value");

    // main и fw_info — адрес с признаком Thumb и два байта по сброшенному адресу
    printf("%-15s 0x%08x  0x%04x\n", "main",    (unsigned)(uintptr_t)main,    (unsigned)*main_code);
    printf("%-15s 0x%08x  0x%04x\n", "fw_info", (unsigned)(uintptr_t)fw_info, (unsigned)*fw_info_code);

    // commands — адрес массива
    printf("%-15s 0x%08x\n", "commands", (unsigned)(uintptr_t)commands);

    // обработчики — имя команды и адрес обработчика, строкой на команду
    for (uint i = 0; i < command_count; i++)
    {
        printf("- %-12s 0x%08x\n",
               commands[i].name,
               (unsigned)(uintptr_t)commands[i].handler);
    }

    // константы паспорта из device.h — адрес и значение строк
    printf("%-15s 0x%08x  %s\n", "DEVICE_PROJECT", (unsigned)(uintptr_t)DEVICE_PROJECT, DEVICE_PROJECT);
    printf("%-15s 0x%08x  %s\n", "DEVICE_BOARD",   (unsigned)(uintptr_t)DEVICE_BOARD,   DEVICE_BOARD);

    // data_variable и bss_variable — адрес и значение
    printf("%-15s 0x%08x  %u\n", "data_variable", (unsigned)(uintptr_t)&data_variable, (unsigned)data_variable);
    printf("%-15s 0x%08x  %u\n", "bss_variable",  (unsigned)(uintptr_t)&bss_variable,  (unsigned)bss_variable);

    // stack_variable — адрес и значение
    printf("%-15s 0x%08x  %u\n", "stack_variable", (unsigned)(uintptr_t)&stack_variable, (unsigned)stack_variable);

    // heap_variable — адрес БЛОКА (значение указателя), а не адрес самого указателя
    if (heap_variable != NULL)
    {
        printf("%-15s 0x%08x  %u\n", "heap_variable", (unsigned)(uintptr_t)heap_variable, (unsigned)*heap_variable);
    }

    // ===== вернуть блок кучи =====
    free(heap_variable);
}