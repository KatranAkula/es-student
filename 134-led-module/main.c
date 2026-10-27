#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include "led.h"
#include "log.h"

const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;

bool get_button_debounce(uint pin) //обработка дребезга
{
    bool state = gpio_get(pin); //объявляем переменную и присваеваем статуса кнопки 1 или 0
    sleep_ms(DEBOUNCE_MS);      //ждёт задержки в миллисек
    return state && gpio_get(pin); // присваивает state  1 при повторном 1
}

bool handle_command(int command) //пришел символ 
{
    if (command == 'e') //если символ е
    {
        led_set(true);
    }
    else if (command == 'd') //
    {
        led_set(false);
    }
        else if (command == 'v') //
    {
        log_version();
    }
    else
    {
        printf("unknown command: %c\n", command);
    }
    

    return led_is_on();
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

        int command = getchar_timeout_us(0);

        if (command == PICO_ERROR_TIMEOUT)
        {
            continue;
        }

        LOG_DBG("got %c\n", command);
        handle_command(command);
    }
}