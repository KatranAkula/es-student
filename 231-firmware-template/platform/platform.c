#include "platform/platform.h"   //свой заголовок первым

#include "pico/stdlib.h"     //stdio_init_all — единственное место проекта с SDK
#include "hardware/gpio.h"

//номера выводов знает только этот файл
#define LED_PIN    PICO_DEFAULT_LED_PIN
#define BUTTON_PIN 15                 //подставь свой номер из button-task.c

void platform_init(void)
{
    stdio_init_all();                 //USB/UART

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);         //кнопка замыкает на землю
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on);
}

bool platform_button_read(void)
{
    return !gpio_get(BUTTON_PIN) == 0;     //нажатая кнопка даёт ноль — знает только platform
}
