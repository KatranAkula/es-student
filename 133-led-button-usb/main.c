#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

// объявляем константы
const uint BUTTON_PIN = 15;
const uint LED_PIN = 25;
const uint DEBOUNCE_MS = 20; //константа задержки


//константа дребезга
bool get_button_debounce(uint pin)
{
    bool state = gpio_get(pin);
    sleep_ms(DEBOUNCE_MS);
    return state && gpio_get(pin);
}

void set_led(bool on) // описываю метод вкл/выкл светодиода
{
    gpio_put(LED_PIN, on);
    printf("led %s\n", on ? "on" : "off");
}

int main()
{
    // инициализируем вывот в терминал
    stdio_init_all();

    // инициализируем пин светодиода
    // настраиваем пин светодиода на выход

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // инициализируем пин кнопки
    // настраиваем пин кнопки на вход
    //притягиваем питание на кнопку

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    //объявляем переменные

    bool led = false;
    bool previous = false;

   // цикл
    while (1)
    {   
        bool current = get_button_debounce(BUTTON_PIN); // читаем состояние пина кнопки с задержкой
        
        if (previous == true && current == false) // проверяем, нужно ли переключить светодиод
        {
            led = !led;
            //gpio_put(LED_PIN, led);
            set_led(led); //вызов метода set_led для переменной led
        }
        
        previous = current; // запоминаем текущее состояние пина кнопки, как предыдущее
    }
}