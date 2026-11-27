#pragma once

#include <stdint.h>

//Константы прошивки
#define DEVICE_NAME       "231-firmware_:-)"
#define FIRMWARE_VERSION  "1.0.1.0.1"
#define DEVICE_PROJECT    "231-firmware-template"
#define DEVICE_REPO       "https://github.com/KatranAkula/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

//Структура со сведениями о плате
#define DEVICE_SERIAL_SIZE 17   // 16 hex + '\0'
#define DEVICE_BOARD_SIZE  32
#define DEVICE_SDK_SIZE    16

typedef struct {
    char     board[DEVICE_BOARD_SIZE];
    char     serial[DEVICE_SERIAL_SIZE];
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    char     sdk_version[DEVICE_SDK_SIZE];
} device_info_t;

// Заполнить структуру сведениями о плате. Не печатает.
void device_get_info(device_info_t *info);