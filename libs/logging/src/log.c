
#include "logging/logging.h"


void log_prefix(const char *level, const char *function, int line) //напечатать префикс перед сообщением лога
{
    printf("%s %s:%d ", level, function, line); //напечатать «<уровень> <функция>:<строка> » — например, «inf led_set:24 »
}
