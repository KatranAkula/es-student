#include "pico/stdlib.h"
#include <stdio.h>
#include "led-task.h" //  вместо "led.h"
//#include "led.h"
#include "log.h"
#include "device.h" // для device_info()
#include "profiling.h"
#include "stdio-text-protocol.h" //для command_t и stdio_text_protocol_handle()
#include "api.h"                 //для api_handle()
#include "button-task.h" // задача кнопки: дребезг машиной состояний

//УДАЛЕНО!!! #define LINE_SIZE, char line[], uint line_length
// Всё, что связано с приёмом строки, теперь в stdio-text-protocol.

//УДАЛЕНО!!! const uint CALC_PI_TERMS и функция calc_pi
//УДАЛЕНО!!! volatile double pi_result
//УДАЛЕНО!!! cmd_calc_pi, cmd_uptime, cmd_info, cmd_main_time_exec, cmd_main_time_reset
// Всё это переехало в api/api.c

//УДАЛЕНО!!! Таблица commands[] и функция handle_command()
//УДАЛЕНО!!! Функция read_line()
// Всё это переехало в api/api.c и stdio-text-protocol/stdio-text-protocol.c


/* !!!!!!!! Всё мигание переехало в led-task/led-task.c

const uint BLINK_HALF_PERIOD_MS = 500; //константа для периода мигания
uint64_t last_toggle_us = 0; //глобальная переменная хранит отметку последнего переключения  

void blink(void) //функция мигания, вызывается каждую итерацию суперцикла. Ничего не ждёт — только проверяет
{
    uint64_t now_us = time_us_64(); //функция ведёт МОНОТОННОЕ ВРЕМЯ!!!!

    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000) //проверка сколько времени прошло
    {
        last_toggle_us = now_us; //перезапись времени текущего переключения
        led_toggle(); //вкл/выкл светодиод
    } //если условие не сработало - не блокируем код и едем дальше по суперциклу
}
*/

int main()
{
    stdio_init_all(); // инициализация ввода-вывода
    led_task_init();  //вместо led_init() — задача сама настроит пин
    profiling_init(); // запоминает начальное показание часов для профилирования

    stdio_text_protocol_init(); //инициализация протокола

    
    button_task_init(); //GPIO кнопки: вход с подтяжкой к питанию


    while (1)
    {
        profiling_iteration(); //замеряется вся итерация целиком: мигание, приём команд
        //blink(); // уехало 

        //УДАЛЕНО!!! read_line();
        //Получает команду от протокола и передаём в api
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command); // выполняем команду
        }

        button_task_handle(); //машина кнопки — каждый цикл, без пауз
        led_task_handle();    //задача светодиода — каждый цикл


    }
}