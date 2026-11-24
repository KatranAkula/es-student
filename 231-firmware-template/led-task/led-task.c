// led-task.c — задача светодиода    
#include "led-task.h"   //свой заголовок первым
#include "pico/stdlib.h"  //SDK только реализации

static led_state_t led_state;   //текущее состояние, static = не видно снаружи
static uint32_t    period_ms;  //период в мс (для get)
static uint64_t    half_period_us; //полупериод в мкс (для сравнения с time_us_64)
static uint64_t    last_toggle_us;  //метка прошлого переключения
static bool        led_level;  //текущий уровень вывода

static bool period_elapsed(uint64_t *last_us, uint64_t period_us)//проверка периода, static = внутренняя
{
    uint64_t now_us = time_us_64();  //читаем часы один раз

    if (now_us - *last_us < period_us) {//период ещё не истёк?
        return false;           //нет — выходим
    }

    *last_us += period_us; //следующее — от прошлой метки (+= ищет проверка)

    if (now_us - *last_us >= period_us) { //отстали больше чем на период?
        *last_us = now_us;   //да — не догоняем, начинаем заново
    }

    return true;  //период истёк
}

void led_task_set_state(led_state_t state) //единственная точка смены состояния
{
    led_state = state;   //запоминаем новое

    switch (led_state) {  //действие при входе
    case LED_STATE_OFF:    //вход в «выключен»
        led_level = false;  //уровень = 0
        gpio_put(PICO_DEFAULT_LED_PIN, false);  //гасим
        break;
    case LED_STATE_ON:   //вход в «горит»
        led_level = true;   //уровень = 1
        gpio_put(PICO_DEFAULT_LED_PIN, true);//зажигаем
        break;
    case LED_STATE_BLINK:  //вход в «мигает»
        last_toggle_us = time_us_64() - half_period_us; // метка в прошлое — первое переключение сразу
        break;
    }
}

led_state_t led_task_get_state(void) //чтение состояния
{
    return led_state; //возвращаем static-переменную
}

bool led_task_set_period_ms(uint32_t new_period_ms)  //установка периода
{
    if (new_period_ms == 0) {  //ноль недопустим
        return false;  //отказ
    }
    period_ms      = new_period_ms;  //запоминаем в мс
    half_period_us = (uint64_t)new_period_ms * 1000 / 2; // в мкс, приведение до умножения
    return true;  //принят
}

uint32_t led_task_get_period_ms(void)  //чтение периода
{
    return period_ms;  //возвращаем мс
}

void led_task_init(void) //инициализация, один раз из main
{
    gpio_init(PICO_DEFAULT_LED_PIN); //включить GPIO
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);  //сделать выходом

    led_level      = false;  //начальный уровень — 0
    period_ms      = 1000;   //период 1 с по ТЗ
    half_period_us = (uint64_t)period_ms * 1000 / 2; //полупериод 500 000 мкс
    led_task_set_state(LED_STATE_BLINK);             //старт — «мигает», через set_state
}

void led_task_handle(void)  //обработчик, зовёт суперцикл
{
    switch (led_state) {  //что делать внутри состояния
    case LED_STATE_OFF:  //«выключен»
    case LED_STATE_ON:      //«горит» — обрабатываются одинаково
        break;    //вывод уже установлен при входе
    case LED_STATE_BLINK:  //«мигает»
        if (period_elapsed(&last_toggle_us, half_period_us)) { // пора переключать?
            led_level = !led_level;  //инвертируем уровень
            gpio_put(PICO_DEFAULT_LED_PIN, led_level); // пишем в вывод
        }
        break;
    }
}