#pragma once                                // чтобы заголовок не подключался дважды

#include <stdio.h>                          // нужен для printf (используется в макросах LOG_*)

//#define DEVICE_NAME "es-led-module"       // (закомментировано: перенесено в device.h)
//#define FIRMWARE_VERSION "1.0.0"          // (закомментировано: перенесено в device.h)

#define LOG_LEVEL_ERR 1                     // уровень «только ошибки» (самый важный, всегда печатается)
#define LOG_LEVEL_INF 2                     // уровень «ошибки + информация» (обычные сообщения)
#define LOG_LEVEL_DBG 3                     // уровень «ошибки + информация + отладка» (самый подробный)

#ifndef LOG_LEVEL                       // если снаружи LOG_LEVEL не задан (например, -DLOG_LEVEL=2 в CMake)
#define LOG_LEVEL LOG_LEVEL_DBG           // задай по умолчанию самый подробный уровень (DBG)
#endif                                  // endif закрывает ifndef

void log_version(void);                                 // объявление: напечатать версию прошивки и дату сборки
void log_prefix(const char *level, const char *function, int line); // объявление: напечатать префикс «<уровень> <функция>:<строка> »

// Макрос для сообщений уровня ERR.
// do { ... } while (0) — обёртка, чтобы макрос вёл себя как одна инструкция.
// __func__ — имя текущей функции (встроенный макрос компилятора).
// __LINE__ — номер текущей строки (встроенный макрос).
// __VA_ARGS__ — все аргументы, переданные в макрос (как в printf).
#define LOG_ERR(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_ERR)             \
        {                                           \
            log_prefix("err", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)

// Макрос для сообщений уровня INF (та же структура, что у LOG_ERR).
#define LOG_INF(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_INF)             \
        {                                           \
            log_prefix("inf", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)

// Макрос для сообщений уровня DBG (та же структура, что у LOG_ERR).
#define LOG_DBG(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_DBG)             \
        {                                           \
            log_prefix("dbg", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)
    