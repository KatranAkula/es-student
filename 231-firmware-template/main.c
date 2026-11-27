#include "pico/stdlib.h"
#include <stdio.h>
#include "led-task.h" //вместо "led.h"
//#include "led.h"
#include "log.h"
#include "device.h" //для device_info()
#include "profiling.h"
#include "stdio-text-protocol.h" //для command_t и stdio_text_protocol_handle()
#include "api.h" //для api_handle()
#include "button-task.h" //задача кнопки: дребезг машиной состояний
#include "pi-task.h"

// #define LINE_SIZE, char line[], uint line_length
//Всё, что связано с приёмом строки, теперь в stdio-text-protocol.

// const uint CALC_PI_TERMS и функция calc_pi
// volatile double pi_result
// cmd_calc_pi, cmd_uptime, cmd_info, cmd_main_time_exec, cmd_main_time_reset
//Всё это переехало в api/api.c

// Таблица commands[] и функция handle_command()
// Функция read_line()
//Всё это переехало в api/api.c и stdio-text-protocol/stdio-text-protocol.c


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

//номера секундомеров (enum, имена из приложения Б)
typedef enum {
    STOPWATCH_LOOP,//вся итерация суперцикла
    STOPWATCH_COMMAND, //приём символа и выполнение команды
    STOPWATCH_LED, //задача светодиода
    STOPWATCH_BUTTON,//задача кнопки
    STOPWATCH_PI,//задача расчёта π
} stopwatch_id_t;

int main()
{
    stdio_init_all(); //инициализация ввода-вывода
    LOG_INF("%s %s\n", DEVICE_NAME, FIRMWARE_VERSION); //имя и версия прошивки при запуске

    led_task_init();//вместо led_init() — задача сама настроит пин
    /* profiling_init(); */ //УДАЛЕНО: заменено на profiling_stopwatch_init() ниже

    stdio_text_protocol_init(); //инициализация протокола

    
    button_task_init(); //GPIO кнопки: вход с подтяжкой к питанию

//завести секундомеры (имя + номер)
    profiling_stopwatch_init(STOPWATCH_LOOP,    "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED,     "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON,  "button");
    profiling_stopwatch_init(STOPWATCH_PI,      "pi");


    while (1)
    {
        //profiling_iteration(); //заменено на profiling_start/stop
        profiling_start(STOPWATCH_LOOP); //старт замера всей итерации
//blink(); //уехало 

// read_line();
//Получает команду от протокола и передаём в api
        profiling_start(STOPWATCH_COMMAND); //старт замера приёма команды
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command); //выполняем команду
        }
        profiling_stop(STOPWATCH_COMMAND); //стоп замера приёма команды

        profiling_start(STOPWATCH_BUTTON); //старт замера кнопки
        button_task_handle(); //машина кнопки — каждый цикл, без пауз
        profiling_stop(STOPWATCH_BUTTON); //стоп замера кнопки

        profiling_start(STOPWATCH_LED); //старт замера светодиода
        
        led_task_handle();//задача светодиода — каждый цикл
        profiling_stop(STOPWATCH_LED); //стоп замера светодиода

        profiling_start(STOPWATCH_PI); //старт замера π
        pi_task_handle();
        profiling_stop(STOPWATCH_PI); //стоп замера π

        profiling_stop(STOPWATCH_LOOP); //стоп замера всей итерации
    }
}


