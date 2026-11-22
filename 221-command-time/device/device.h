#pragma once // чтобы заголовок не подключался дважды (забор от двойного включения)

#include <stdint.h>   //нужен для uint8_t и uint32_t в struct info_t - без этого не работает!!!!

#define DEVICE_NAME "es-led-module"   // имя устройства (используется в log_version для печати «device: ...»)
#define FIRMWARE_VERSION "1.1.0"      //версия прошивки (тоже печатается в log_version)

#define DEVICE_PROJECT "221-command-time"                       //печатается в info как «project: 221-command-time»
#define DEVICE_REPO "https://github.com/KatranAkula/es-student" //адрес репозитория: печатается в info как «repo: ...»

#ifndef DEVICE_BOARD     //если DEVICE_BOARD ещё не определён снаружи (например, из CMakeLists.txt)
#define DEVICE_BOARD "unknown"   // макрос с именем DEVICE_BOARD. везде, где встретишь DEVICE_BOARD, подставь "unknown"
#endif      //endif всегда закрывает ifndef


/*//не нормально
struct info_t
{
    uint8_t  revision;// 1 байт,  выравнивание 1
    uint32_t version;  // 4 байта, выравнивание 4
    char     name[13];  // 13 байт, выравнивание 1
};
*/

// Нормально
struct info_t
{
    uint32_t version;  //4 байта, выравнивание 4
    char     name[13]; //13 байт, выравнивание 1 (12 символов + '\0')
    uint8_t  revision;  //1 байт,  выравнивание 1
};

extern struct info_t device_card; //Глобальная переменная паспорта, видимая другим модулям

void device_info(void);  //объявляю функции: она есть в device.c, а вызывать её можно отсюда
void dev_info(void);  //объявляю новую команду
