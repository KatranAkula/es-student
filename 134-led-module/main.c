#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include "led/led.h"

const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;

bool get_button_debounce(uint pin)
{
    bool state = gpio_get(pin);
    sleep_ms(DEBOUNCE_MS);
    return state && gpio_get(pin);
}

bool handle_command(int command)
{
    if (command == 'e')
    {
        led_set(true);
    }
    else if (command == 'd')
    {
        led_set(false);
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

        handle_command(command);
    }
}