#pragma once // чтобы заголовок не подключался дважды

#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "134-led-module"
#define DEVICE_REPO "https://github.com/KatranAkula/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"  // макрос с именем DEVICE_BOARD. везде, где встретишь DEVICE_BOARD, подставь "unknown"
#endif                          // endif всегда закрывает ifndef

void device_info(void);