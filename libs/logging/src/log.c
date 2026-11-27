//теперь libs\logging\include\logging\logging.h вместо  lids#include "лог.h"  
#include "logging/logging.h"
//#include "device.h"  //нужен для DEVICE_NAME и FIRMWARE_VERSION (используются в лог_version)

void log_prefix(const char *level, const char *function, int line) //напечатать префикс перед сообщением лога
{
    printf("%s %s:%d ", level, function, line); //напечатать «<уровень> <функция>:<строка> » — например, «inf led_set:24 »
}
