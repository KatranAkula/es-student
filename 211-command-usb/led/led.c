#include "led.h"                  // указание компилятору что нужно вставить (объявления led_init, led_set, led_toggle, led_is_on)
#include "hardware/gpio.h"        // заголовок Pico SDK: нужен для gpio_init, gpio_set_dir, gpio_put
#include <stdio.h>                // стандартная библиотека ввода-вывода, в этом файле напрямую не используется
#include "log.h"                  // нужен для макросов LOG_INF (не функция — макрос, раскрывается в printf)

static const uint LED_PIN = 25;   // номер пина светодиода (25 — встроенный LED на Pico); static — «виден только в этом файле»
static bool led_state = false;    // текущее состояние светодиода (false = выключен); static — «виден только в этом файле»

void led_init(void)               // инициализация модуля светодиода (вызывается один раз из main)
{
    gpio_init(LED_PIN);           // включить GPIO-пин LED_PIN (настроить его как цифровой вход/выход)
    gpio_set_dir(LED_PIN, GPIO_OUT); // сделать пин выходом  буду сами управлять уровнем
    gpio_put(LED_PIN, led_state); // выставить начальное состояние на пине (по умолчанию false = низкий уровень)
}

void led_set(bool on)             // включить (on = true) или выключить (on = false) светодиод
{
    led_state = on;               // запомнить новое состояние в переменной (чтобы led_is_on() потом вернула его)
    gpio_put(LED_PIN, led_state); // выставить это состояние на физическом пине
    //printf("led %s\n", led_state ? "on" : "off"); //  из предыдущего задания
    LOG_INF("led %s\n", led_state ? "on" : "off"); // макрос, объявленный в log.h НЕ ФУНКЦИЯ!!! (раскрывается в log_prefix + printf)
}

void led_toggle(void)             // переключить светодиод (вкл → выкл, выкл → вкл)
{
    led_state = !led_state;       // инвертировать состояние: было true → стало false, было false → стало true
    gpio_put(LED_PIN, led_state); // выставить новое состояние на пине
    //printf("led %s\n", led_state ? "on" : "off"); //  функция из <stdio.h> (старый вариант, закомментирован)
    LOG_INF("led %s\n", led_state ? "on" : "off");  // макрос, объявленный в log.h НЕ ФУНКЦИЯ!!!
}

bool led_is_on(void)              // узнать текущее состояние светодиода (true = включён, false = выключен)
{
    return led_state;             // вернуть сохранённое состояние
}