#include "device.h"                 // мой заголовок: там лежат DEVICE_PROJECT, DEVICE_REPO, DEVICE_BOARD

#include <stddef.h>                 //нужен для работы offsetof
#include <stdint.h>                 //нужен для uintptr_t

#include <stdio.h>                  // стандартная библиотека: нужна для printf()
#include "pico/unique_id.h"         // заголовок Pico SDK: нужен для pico_get_unique_board_id_string()
#include "hardware/regs/addressmap.h" // карта адресов: там SYSINFO_BASE — адрес блока SYSINFO в памяти чипа
#include "hardware/regs/sysinfo.h"  // описание регистров SYSINFO: там OFFSET и маски полей CHIP_ID
#include "pico/version.h"           // для задания 2.1.1 добавил для PICO_SDK_VERSION_STRING (строка версии SDK)

void device_info(void)              // функция без аргументов и без возврата: просто печатает паспорт
{
    // 1. серийный номер платы

    char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1]; // массив под строку с ID: каждый байт → 2 hex-символа, +1 под '\0'
    pico_get_unique_board_id_string(board_id, sizeof(board_id)); // заполнить board_id строкой с уникальным ID платы (размер передаём, чтобы не переполнить)

    // 2. CHIP_ID из SYSINFO

    volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET); // указатель на регистр CHIP_ID: адрес = база + смещение; volatile — «читай каждый раз, не кэшируй»
    uint32_t id = *chip_id;          // прочитать 32-битное значение из памяти по адресу chip_id

    uint32_t manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB; // вырезать биты производителя маской & и сдвинуть вправо, чтобы число начиналось с нуля
    uint32_t part         = (id & SYSINFO_CHIP_ID_PART_BITS)         >> SYSINFO_CHIP_ID_PART_LSB;         // то же для поля part (модель чипа)
    uint32_t revision     = (id & SYSINFO_CHIP_ID_REVISION_BITS)     >> SYSINFO_CHIP_ID_REVISION_LSB;     // то же для поля revision (ревизия чипа)

    // 3. печать паспорта

    printf("project: %s\n", DEVICE_PROJECT); // напечатать «project: <имя проекта>» и перевод строки (%s — подставить строку)
    printf("repo: %s\n", DEVICE_REPO);       // напечатать «repo: <адрес репозитория>»
    printf("board: %s\n", DEVICE_BOARD);     // напечатать «board: <имя платы>»
    printf("serial: %s\n", board_id);        // напечатать «serial: <серийный номер>» из шага 1
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", // напечатать поля чипа: %03x — hex минимум 3 цифры, %04x — hex минимум 4, %u — десятичное
           manufacturer, part, revision);   // подставить значения, полученные в шаге 2
    printf("pico-sdk: %s\n", PICO_SDK_VERSION_STRING);   // ← ДОБАВЛЕНО: напечатать «pico-sdk: <версия SDK>»
}


struct info_t device_card = {   //Глобальная переменная — заполнена, поэтому попадёт в .data
    .version  = 0x00010000,         //числовая версия
    .name     = "es-cmd-usb",       //имя устройства (10 символов + '\0' = 11, влезает в 13)
    .revision = 2,                  //ревизия платы
};


void dev_info(void) //Команда dev_info — печатает раскладку структуры в памяти
{
    printf("%-13s %-10s %6s %6s %s\n", "struct", "address", "size", "offset", "value"); //   шапка таблицы
    printf("%-13s 0x%08x %6u\n",                                                          //   строка самой структуры
           "device_card",
           (unsigned)(uintptr_t)&device_card,   //адрес структуры
           (unsigned)sizeof(device_card));      //sizeof структуры

    printf("- %-11s 0x%08x %6u %6u 0x%08x\n",  //поле version
           "version",
           (unsigned)(uintptr_t)&device_card.version,   //адрес поля
           (unsigned)sizeof(device_card.version),       //размер поля
           (unsigned)offsetof(struct info_t, version),  //смещение от начала структуры
           (unsigned)device_card.version);              //значение поля

    printf("- %-11s 0x%08x %6u %6u %s\n",               //поле name
           "name",
           (unsigned)(uintptr_t)device_card.name,       //у массива имя уже адрес, & не нужен
           (unsigned)sizeof(device_card.name),          //размер массива = 13
           (unsigned)offsetof(struct info_t, name),     //смещение
           device_card.name);                           //значение-строка

    printf("- %-11s 0x%08x %6u %6u %u\n",               //поле revision
           "revision",
           (unsigned)(uintptr_t)&device_card.revision,  //   адрес поля
           (unsigned)sizeof(device_card.revision),      //   размер поля
           (unsigned)offsetof(struct info_t, revision), //   смещение
           (unsigned)device_card.revision);             //   значение

    unsigned fields = sizeof(device_card.version)       //   сумма размеров полей
                    + sizeof(device_card.name)
                    + sizeof(device_card.revision);

    printf("fields %u, sizeof %u, padding %u\n",        //   итоговая строка
           fields,
           (unsigned)sizeof(device_card),               //   sizeof структуры
           (unsigned)(sizeof(device_card) - fields));   //   заполнение
}