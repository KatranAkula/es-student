#include "log.h"                  //Заголовок: там объявления log_version, log_prefix и макросы LOG_ERR/INF/DBG
#include "device.h"               // нужен для DEVICE_NAME и FIRMWARE_VERSION (используются в log_version)

void log_version(void)            // печать версии прошивки и дату сборки (вызывается по команде "version")
{
    printf("device: %s %s, built %s %s, log level %d\n", // шаблон строки: %s — строка, %d — целое
    DEVICE_NAME, FIRMWARE_VERSION, __DATE__, __TIME__, LOG_LEVEL); // подставить: имя устройства, версия прошивки, дата сборки, время сборки, уровень лога
}

void log_prefix(const char *level, const char *function, int line) // напечатать префикс перед сообщением лога
{
    printf("%s %s:%d ", level, function, line); // напечатать «<уровень> <функция>:<строка> » — например, «inf led_set:24 »
}