#include "led.h"
#include "hardware/gpio.h"
#include <stdio.h>

static const uint LED_PIN = 25;
static bool led_state = false;

void led_init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, led_state);
}

void led_set(bool on)
{
    led_state = on;
    gpio_put(LED_PIN, led_state);
    printf("led %s\n", led_state ? "on" : "off");
}

void led_toggle(void)
{
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);
    printf("led %s\n", led_state ? "on" : "off");
}

bool led_is_on(void)
{
    return led_state;
}