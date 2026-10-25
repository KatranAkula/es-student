// объявления функций SDK: gpio_*, sleep_ms
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// объявляю константу лед_пин для 25 GPIO
const uint LED_PIN = 25;

int main()
{
    // подключить вывод к блоку GPIO
    gpio_init(LED_PIN);
    // настроить вывод на выход
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // суперцикл: повторяется, пока подано питание
    while (1)
    {
        // высокий уровень — светодиод горит полсекунды
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        // низкий уровень — светодиод погас на полсекунды
        gpio_put(LED_PIN, 0);
        sleep_ms(1000);
    }
}