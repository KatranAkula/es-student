#pragma once // чтобы заголовок не подключался дважды (забор от двойного включения)

#define DEVICE_NAME "es-led-module"   // имя устройства (используется в log_version для печати «device: ...»)
#define FIRMWARE_VERSION "1.0.0"      //версия прошивки (тоже печатается в log_version)

#define DEVICE_PROJECT "211-command-usb"                       //печатается в info как «project: 211-command-usb»
#define DEVICE_REPO "https://github.com/KatranAkula/es-student" //адрес репозитория: печатается в info как «repo: ...»

#ifndef DEVICE_BOARD                  //если DEVICE_BOARD ещё не определён снаружи (например, из CMakeLists.txt)
#define DEVICE_BOARD "unknown"        // макрос с именем DEVICE_BOARD. везде, где встретишь DEVICE_BOARD, подставь "unknown"
#endif                                //endif всегда закрывает ifndef

void device_info(void);               //объявление функции: она есть в device.c, а вызывать её можно отсюда