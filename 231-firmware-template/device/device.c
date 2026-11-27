#include "device.h"   // мой заголовок: там лежат DEVICE_PROJECT, DEVICE_REPO, DEVICE_BOARD

#include <stddef.h>    //нужен для работы offsetof
#include <stdint.h> //нужен для uintptr_t

#include <string.h>   // для strncpy

#include <stdio.h>    // стандартная библиотека: нужна для printf()
#include "pico/unique_id.h"   // заголовок Pico SDK: нужен для pico_get_unique_board_id_string()
#include "hardware/regs/addressmap.h" // карта адресов: там SYSINFO_BASE — адрес блока SYSINFO в памяти чипа
#include "hardware/regs/sysinfo.h"  // описание регистров SYSINFO: там OFFSET и маски полей CHIP_ID
#include "pico/version.h"   // для задания 2.1.1 добавил для PICO_SDK_VERSION_STRING (строка версии SDK)


void device_get_info(device_info_t *info)
{
    //тип платы
    strncpy(info->board, DEVICE_BOARD, sizeof(info->board) - 1);
    info->board[sizeof(info->board) - 1] = '\0';

    //серийный номер
    pico_get_unique_board_id_string(info->serial, sizeof(info->serial));

    //CHIP_ID
    volatile uint32_t *chip_id =
        (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);

    uint32_t id = *chip_id;

    info->manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    info->part         = (id & SYSINFO_CHIP_ID_PART_BITS)         >> SYSINFO_CHIP_ID_PART_LSB;
    info->revision     = (id & SYSINFO_CHIP_ID_REVISION_BITS)     >> SYSINFO_CHIP_ID_REVISION_LSB;

    //Версия SDK
    strncpy(info->sdk_version, PICO_SDK_VERSION_STRING, sizeof(info->sdk_version) - 1);
    info->sdk_version[sizeof(info->sdk_version) - 1] = '\0';
}