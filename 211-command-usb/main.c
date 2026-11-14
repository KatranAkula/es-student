#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>        // ← ДОБАВЛЕНО: для strcmp()
#include "led.h"
#include "log.h"
#include "device.h"        // ← ДОБАВЛЕНО: для device_info()

const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;

// ===== ШАГ 2: буфер строки и его длина =====
#define LINE_SIZE 32

char line[LINE_SIZE];
uint line_length = 0;

bool get_button_debounce(uint pin) //обработка дребезга
{
    bool state = gpio_get(pin); //объявляем переменную и присваеваем статуса кнопки 1 или 0
    sleep_ms(DEBOUNCE_MS);      //ждёт задержки в миллисек
    return state && gpio_get(pin); // присваивает state  1 при повторном 1
}

// ===== ШАГ 4: разбор строки через strcmp() =====
//bool handle_command(int command) //пришел символ 
void handle_command(const char *command) //пришла строка
{
    //if (command == 'e') //если символ е
    if (strcmp(command, "enable") == 0)
    {
        led_set(true);
    }
    //else if (command == 'd') //
    else if (strcmp(command, "disable") == 0)
    {
        led_set(false);
    }
    //else if (command == 'v') //
    else if (strcmp(command, "version") == 0)
    {
        log_version();        
    }
    //else if (command == 'i')          // ← вот здесь, до else
    else if (strcmp(command, "info") == 0)
    {
        device_info();
    }
    else
    {
        //LOG_ERR("unknown command: %c\n", command); //неизвестная команда идёт уровнем ERR
        LOG_ERR("unknown command: %s\n", command); //неизвестная команда идёт уровнем ERR
    }
    
    //return led_is_on();
}

// ===== ШАГ 3: приём строки из символов =====
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

        //int command = getchar_timeout_us(0);
        //
        //if (command == PICO_ERROR_TIMEOUT)
        //{
        //    continue;
        //}
        //
        //LOG_DBG("got %c\n", command);
        //handle_command(command);
        read_line();   // ← ЗАМЕНА: приём строки вместо одного символа
    }
}