#include "api.h"
#include "pico/stdlib.h"
#include "device.h"
#include "profiling.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"

//набор команд

// ── info: печатает паспорт из структуры ───────────────────
static void cmd_info(const command_t *command) //обработчик команды info
{
    (void)command;  //аргументы не нужны

    device_info_t info;//структура со сведениями о плате
    device_get_info(&info);  //заполнить её по указателю

    printf("device: %s %s\n", DEVICE_NAME, FIRMWARE_VERSION); // имя и версия прошивки
    printf("project: %s\n", DEVICE_PROJECT);//имя проекта
    printf("repo: %s\n", DEVICE_REPO);//адрес репозитория
    printf("board: %s\n", info.board);//тип платы (из структуры)
    printf("serial: %s\n", info.serial); //серийный номер (из структуры)
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n",
           (unsigned)info.manufacturer,  //производитель, hex
           (unsigned)info.part, //изделие, hex
           (unsigned)info.revision);  //ревизия, dec
    printf("pico-sdk: %s\n", info.sdk_version);//версия SDK (из структуры)
}

static void cmd_uptime(const command_t *command)  //обработчик uptime
{
    (void)command;  //аргументы не нужны
    printf("uptime: %llu ms\n", time_us_64() / 1000);//время с момента сброса в мс
}




//команды светодиода состояние и период
static void print_led_state(void)  //общий ответ для команд светодиода
{
    static const char *names[] = { //имена состояний по индексу enum
        [LED_STATE_OFF]   = "off", //индекс 0 → "off"
        [LED_STATE_ON]    = "on",  //индекс 1 → "on"
        [LED_STATE_BLINK] = "blink",  //индекс 2 → "blink"
    };
    printf("led: %s, period %u ms\n", //шаблон ответа
           names[led_task_get_state()],  //текущее состояние
           (unsigned)led_task_get_period_ms());//текущий период
}



static void cmd_led_enable(const command_t *command) //обработчик led_enable
{
    (void)command;  //аргументы не нужны
    led_task_set_state(LED_STATE_ON); //включить светодиод
    print_led_state(); //напечатать состояние и период
}

static void cmd_led_disable(const command_t *command)//обработчик led_disable
{
    (void)command;  //аргументы не нужны
    led_task_set_state(LED_STATE_OFF);//выключить светодиод
    print_led_state(); //напечатать состояние и период
}




static void cmd_led_blink(const command_t *command)  //обработчик led_blink
{
    (void)command;  //аргументы не нужны
    led_task_set_state(LED_STATE_BLINK); //перевести в режим мигания
    print_led_state(); //напечатать состояние и период
}




//разбирает числа без знака
static bool parse_u32(const char *text, uint32_t *value)//разобрать строку в uint32_t
{
    if (!isdigit((unsigned char)text[0]))//первый символ не цифра?
    {
        return false;  //отказ (в т.ч. для "-1")
    }

    char *end;//куда strtoul запишет конец числа
    errno = 0;//сбросить errno перед strtoul
    unsigned long number = strtoul(text, &end, 10);  //разобрать десятичное число

    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX) // остались символы / переполнение / больше uint32
    {
        return false;  //отказ
    }

    *value = (uint32_t)number;  //записать результат по указателю
    return true; //успех
}



static void cmd_led_period(const command_t *command) //обработчик led_period
{
    uint32_t period_ms;//сюда разберём аргумент

    if (command->argc != 1 || !parse_u32(command->argv[0], &period_ms)) // ровно 1 аргумент и это число?
    {
        printf("error: usage led_period <period_ms>\n");//нет — подсказка
        return;    
    }

    if (!led_task_set_period_ms(period_ms)) //задача не приняла период?
    {
        printf("error: period_ms must be greater than 0\n"); // ноль — отдельное сообщение
        return;    
    }

    print_led_state(); //напечатать состояние и период
}





//команда кнопки: состояние и число нажатий
static void cmd_button(const command_t *command)  //обработчик button
{
    (void)command;  //аргументы не нужны
    printf("button: %s, presses %lu\n",  //шаблон ответа
           button_task_is_pressed() ? "pressed" : "released", // нажата или отпущена
           (unsigned long)button_task_get_press_count()); // число нажатий
}

//profiling: вывести все секундомеры
static void cmd_profiling(const command_t *command)  //обработчик profiling
{
    (void)command;  //аргументы не нужны

    for (uint32_t i = 0; i < PROFILING_MAX_STOPWATCHES; i++) // по всем номерам секундомеров
    {
        profiling_result_t r;//результат по одному секундомеру
        if (!profiling_get(i, &r)) continue;//не заведён — пропускаем

        printf("%-10s count %-10u mean %.2f us  max %u us\n", // шаблон строки
               r.name, //имя секундомера
               (unsigned)r.count,  //число замеров
               //(double)r.mean_us,  //среднее, мкс выводит баз дроби - потому что приводит uint32_t к double!!!!
               r.mean_us,  //чтобы вывод был с дробью!!!
               (unsigned)r.max_us);//максимум, мкс
    }
}

//profiling_reset: сбросить все секундомеры!!!!!!
static void cmd_profiling_reset(const command_t *command)// обработчик profiling_reset
{
    (void)command;  //аргументы не нужны
    profiling_reset(); //сбросить все счётчики
    printf("profiling reset\n");//подтвердить сброс
}





//команды расчёта Пи
static void cmd_pi_start(const command_t *command)//обработчик pi_start
{
    uint32_t terms = 1000000;//значение по умолчанию

    if (command->argc > 0)//есть аргумент?
    {
        if (command->argc > 1 || !parse_u32(command->argv[0], &terms)) // больше 1 или не число?
        {
            printf("error: usage pi_start [<terms>]\n");//подсказка
            return;   
        }
        if (terms == 0)//ноль членов?
        {
            printf("error: terms must be greater than 0\n"); // отдельное сообщение
            return;   
        }
    }

    if (!pi_task_start(terms))  //задача уже считает?
    {
        printf("error: pi is busy\n");//отказ
        return;    
    }

    printf("pi: started, %u terms\n", (unsigned)terms); //запущено
}




static void cmd_pi(const command_t *command)//обработчик pi
{
    (void)command;  //аргументы не нужны

    double result;  //сюда результат
    uint32_t spent_ms; //сюда время расчёта

    if (pi_task_get_result(&result, &spent_ms))//результат готов?
    {
        printf("pi: %.9f in %u ms\n", result, (unsigned)spent_ms); // напечатать
        return;    
    }


    if (pi_task_get_state() == PI_STATE_RUNNING)  //ещё считает?
    {
        printf("pi: running\n");//сообщить


    }
    else      //не запускался
    {
        printf("pi: not started\n");  //сообщить
    }
}

//таблица команд!!!

typedef void (*api_callback_t)(const command_t *command);// тип обработчика

typedef struct//запись таблицы команд
{
    const char *name;  //имя команды
    const char *help;  //справка для help
    api_callback_t callback; //обработчик
} api_command_t;

static const api_command_t commands[] = {//таблица команд

    { "info",            "device passport", cmd_info },  //паспорт
    { "uptime",          "time since reset", cmd_uptime },  // время с момента сброса    
    { "led_enable",      "turn led on",           cmd_led_enable  }, //светодиод вкл
    { "led_disable",     "turn led off",          cmd_led_disable }, //светодиод выкл
    { "led_blink",       "blink led",             cmd_led_blink   }, //светодиод мигает
    { "led_period",      "set blink period, ms",  cmd_led_period  }, //период мигания
    { "button",          "button state and press count", cmd_button },// кнопка
    { "pi_start",        "start pi calculation: pi_start [terms]", cmd_pi_start }, // запуск π
    { "pi",              "pi result or state",    cmd_pi          }, //результат π
    { "profiling",       "show all stopwatches",  cmd_profiling   }, //секундомеры
    { "profiling_reset", "reset stopwatches",     cmd_profiling_reset }, // сброс




};

static const uint32_t commands_count = sizeof(commands) / sizeof(commands[0]); // число команд


void api_handle(const command_t *command)//главный обработчик
{
 //переполнение буфера (truncated)
    if (command->truncated)  //строка не поместилась?
    {
        printf("error: command longer than 63 characters or 4 arguments\n"); // ошибка
        return;    
    }

 //help
    if (strcmp(command->name, "help") == 0) //команда help?
    {
        printf("help             list of commands\n");  //заголовок


        for (uint32_t i = 0; i < commands_count; i++)//по всем командам
        {
            printf("%-16s %s\n", commands[i].name, commands[i].help); //имя и справка
        }
        return;    
    }



 //поиск в таблице
    for (uint32_t i = 0; i < commands_count; i++) //по всем командам
    {
        if (strcmp(command->name, commands[i].name) == 0)// имя совпало?
        {
            commands[i].callback(command);  //вызвать обработчик
            return;   
        }


    }

 
    printf("error: unknown command '%s', try help\n", command->name); //неизвестная команда - ошибка

}


