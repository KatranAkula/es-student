#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>        // для strcmp()
#include "led.h"
#include "log.h"
#include "device.h"        // для device_info()

const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;

// ===== ШАГ 2: буфер строки и его длина (из прошлого задания) =====
#define LINE_SIZE 32

char line[LINE_SIZE];
uint line_length = 0;

bool get_button_debounce(uint pin) //обработка дребезга
{
    bool state = gpio_get(pin); //объявляем переменную и присваеваем статуса кнопки 1 или 0
    sleep_ms(DEBOUNCE_MS);      //ждёт задержки в миллисек
    return state && gpio_get(pin); // присваивает state  1 при повторном 1
}

// ===== ШАГ 1: тип «указатель на функцию без аргументов и возврата» =====
typedef void (*command_handler_t)(void);

// ===== ШАГ 2: вынести тела команд в отдельные функции =====
void cmd_enable(void)
{
    led_set(true);
}

void cmd_disable(void)
{
    led_set(false);
}

void cmd_info(void)
{
    device_info();
}

void cmd_version(void)
{
    log_version();
}

// ===== ШАГ 5: новая команда ping =====
void cmd_ping(void)
{
    printf("pong\n");
}

// ===== ШАГ 3: структура «имя + обработчик» и таблица команд =====
struct command_t
{
    const char *name;
    command_handler_t handler;
};

const struct command_t commands[] = {
    { "enable",  cmd_enable  },
    { "disable", cmd_disable },
    { "info",    cmd_info    },
    { "version", cmd_version },
    { "ping",    cmd_ping    },
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

// ===== ШАГ 4: разбор строки через таблицу =====
//void handle_command(const char *command) //пришла строка
//{
//    if (strcmp(command, "enable") == 0)
//    {
//        led_set(true);
//    }
//    else if (strcmp(command, "disable") == 0)
//    {
//        led_set(false);
//    }
//    else if (strcmp(command, "version") == 0)
//    {
//        log_version();        
//    }
//    else if (strcmp(command, "info") == 0)
//    {
//        device_info();
//    }
//    else
//    {
//        LOG_ERR("unknown command: %s\n", command); //неизвестная команда идёт уровнем ERR
//    }
//}

void handle_command(const char *command) //пришла строка
{
    for (uint i = 0; i < COMMAND_COUNT; i++)
    {
        if (strcmp(command, commands[i].name) == 0)
        {
            if (commands[i].handler != NULL)
            {
                commands[i].handler();
            }

            return;
        }
    }

    LOG_ERR("unknown command: %s\n", command); //неизвестная команда идёт уровнем ERR
}

// ===== ШАГ 3 (из прошлого задания): приём строки из символов =====
void read_line(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return;
    }

    if (symbol == '\r' || symbol == '\n')
    {
        putchar('\n');
        line[line_length] = '\0';

        if (line_length > 0)
        {
            LOG_DBG("got %s\n", line);
            handle_command(line);
        }

        line_length = 0;
        return;
    }

    if (line_length + 1 < LINE_SIZE)
    {
        line[line_length] = (char)symbol;
        line_length = line_length + 1;
        putchar(symbol);
    }
}

int main()
{
    stdio_init_all();

    led_init();  // модуль сам настроит пин 25 и поставит начальное состояние

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    bool previous = false;

    while (1)
    {
        bool current = get_button_debounce(BUTTON_PIN);

        // Переключаем по отпусканию кнопки
        if (previous == true && current == false)
        {
            led_toggle();
        }

        previous = current;

        read_line();   // приём строки вместо одного символа
    }
}