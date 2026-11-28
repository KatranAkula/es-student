#include "platform/platform.h" //запуск железа — единственная точка с SDK внутри
#include "led-task/led-task.h" //задача светодиода

#include "logging/logging.h"   //вместо "лог.h"
#include "device/device.h"     //паспорт платы
#include "profiling/profiling.h"
#include "stdio-text-protocol/stdio-text-protocol.h"
#include "systime/systime.h"   //время и period_elapsed

#include "logic/api/api.h"



#include "logic/button-task/button-task.h"
#include "logic/pi-task/pi-task.h"

#include "firmware.h"


//номера секундомеров (enum, имена из приложения Б)
typedef enum {
    STOPWATCH_LOOP,   //вся итерация суперцикла
    STOPWATCH_COMMAND,//приём символа и выполнение команды
    STOPWATCH_LED,    //задача светодиода
    STOPWATCH_BUTTON, //задача кнопки
    STOPWATCH_PI,     //задача расчёта π
} stopwatch_id_t;

int main()
{
    platform_init(); //инициализация ввода-вывода и выводов светодиода и кнопки

    LOG_INF("%s %s\n", FIRMWARE_NAME, FIRMWARE_VERSION); //имя и версия прошивки при запуске

    led_task_init(); //задача светодиода: состояние и период

    stdio_text_protocol_init(); //инициализация протокола

    button_task_init(led_task_next_state); //нажатие переводит светодиод в следующее состояние


    //завести секундомеры (имя + номер)
    profiling_stopwatch_init(STOPWATCH_LOOP,    "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED,     "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON,  "button");
    profiling_stopwatch_init(STOPWATCH_PI,      "pi");

    while (1)
    {
        profiling_start(STOPWATCH_LOOP); //старт замера всей итерации

        //получаем команду от протокола и передаём в api
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
        led_task_handle(); //задача светодиода — каждый цикл
        profiling_stop(STOPWATCH_LED); //стоп замера светодиода

        profiling_start(STOPWATCH_PI); //старт замера π
        pi_task_handle();
        profiling_stop(STOPWATCH_PI); //стоп замера π

        profiling_stop(STOPWATCH_LOOP); //стоп замера всей итерации
    }
}
