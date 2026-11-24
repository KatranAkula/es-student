// led-task.h — задача светодиода: выключен, горит или мигает
#pragma once //забор для единожды

#include <stdbool.h>
#include <stdint.h>

typedef enum {  //три состояния по машине OFF/ON/BLINK
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINK,
} led_state_t; //закрывает enum и даёт типу имя led_state_t

void led_task_init(void); //прототип функции инициализации задачи, вызывается один раз из main() до суперцикла
void led_task_handle(void); //прототип обработчика задачи. Вызывается суперциклом на каждой итерации

void led_task_set_state(led_state_t state);//прототип функции смены состояния, внутри — запись состояния и действие при входе - гасим/зажигаем/сбрасываем метку
led_state_t led_task_get_state(void);//прототип функции чтения состояния, возвращает led_state_t для команд api, чтобы напечатать led: blink и т.д., не имея доступа к static-переменной led_state.

void led_task_next_state(void); //прототип перехода в следующее состояние по кругу OFF-ON-BLINK-OFF, через led_task_set_state

// false — период не принят (нулевой)
bool led_task_set_period_ms(uint32_t period_ms); //прототип установки периода в миллисекундах. bool — результат: true — принят, false — отклонён (ноль)
uint32_t led_task_get_period_ms(void);//прототип чтения периода, возвращает период в миллисекундах — то, что задали командой led_period
