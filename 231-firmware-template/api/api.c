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
#include "led-task.h"
#include "button-task.h" // для button_task_is_pressed() и счётчика нажатий

//набор команд

static void cmd_info(const command_t *command)
{
    (void)command; //аргументы не нужны
    device_info();
}

static void cmd_uptime(const command_t *command)
{
    (void)command;
    printf("uptime: %llu ms\n", time_us_64() / 1000);
}

static void cmd_main_time_exec(const command_t *command)
{
    (void)command;
    printf("iteration avg %.2f us, max %u us\n",
           profiling_avg_us(),
           (unsigned)profiling_max_us());
}

static void cmd_main_time_reset(const command_t *command)
{
    (void)command;
    profiling_reset_max();
    printf("max reset\n");
}

//команды светодиода состояние и период
static void print_led_state(void)
{
    static const char *names[] = {
        [LED_STATE_OFF]   = "off",
        [LED_STATE_ON]    = "on",
        [LED_STATE_BLINK] = "blink",
    };
    printf("led: %s, period %u ms\n",
           names[led_task_get_state()],
           (unsigned)led_task_get_period_ms());
}

static void cmd_led_enable(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_ON);
    print_led_state();
}

static void cmd_led_disable(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_OFF);
    print_led_state();
}

static void cmd_led_blink(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_BLINK);
    print_led_state();
}



//считает Пи

// Прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с
const uint CALC_PI_TERMS_DEFAULT = 1000000;

static double calc_pi(uint terms)
{
    double sum = 0.0;
    double sign = 1.0;

    for (uint k = 0; k < terms; k++)
    {
        sum += sign / (2.0 * k + 1.0);
        sign = -sign;
    }

    return sum * 4.0;
}

//разбирает числа без знака
static bool parse_u32(const char *text, uint32_t *value)
{
    if (!isdigit((unsigned char)text[0]))
    {
        return false;
    }

    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);

    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return false;
    }

    *value = (uint32_t)number;
    return true;
}



static void cmd_led_period(const command_t *command)
{
    uint32_t period_ms;

    if (command->argc != 1 || !parse_u32(command->argv[0], &period_ms))
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }

    if (!led_task_set_period_ms(period_ms))
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }

    print_led_state();
}

//команда кнопки: состояние и число нажатий
static void cmd_button(const command_t *command)
{
    (void)command; //аргументы не нужны
    printf("button: %s, presses %lu\n",
           button_task_is_pressed() ? "pressed" : "released",
           (unsigned long)button_task_get_press_count());
}


static void cmd_calc_pi(const command_t *command) //считает Пи с n членами ряда, замеряет время и выводит в терминал
{
    uint32_t terms = CALC_PI_TERMS_DEFAULT;

    if (command->argc > 0)
    {
        if (command->argc > 1 || !parse_u32(command->argv[0], &terms))
        {
            printf("error: usage calc_pi [terms]\n");
            return;
        }
        if (terms == 0)
        {
            printf("error: terms must be greater than 0\n");
            return;
        }
    }

    uint64_t start_us = time_us_64();
    volatile double pi_result = calc_pi(terms);
    uint64_t spent_us = time_us_64() - start_us;

    printf("pi: %.9f\n", pi_result);
    printf("time: %llu ms\n", spent_us / 1000);
}

//таблица команд!!!

typedef void (*api_callback_t)(const command_t *command);

typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;

static const api_command_t commands[] = {
    { "info",            "device passport", cmd_info },
    { "uptime",          "time since reset", cmd_uptime },
    { "calc_pi",         "calculate pi: calc_pi [terms]", cmd_calc_pi },
    { "main_time_exec",  "superloop iteration: average and maximum", cmd_main_time_exec },
    { "main_time_reset", "reset maximum iteration", cmd_main_time_reset },
    { "led_enable",      "turn led on",           cmd_led_enable  },    
    { "led_disable",     "turn led off",          cmd_led_disable },    
    { "led_blink",       "blink led",             cmd_led_blink   },   
    { "led_period",      "set blink period, ms",  cmd_led_period  },  
    { "button",          "button state and press count", cmd_button }, 
};

static const uint32_t commands_count = sizeof(commands) / sizeof(commands[0]);



void api_handle(const command_t *command)  //обработчик
{
    //переполнение буфера (truncated)
    if (command->truncated)
    {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }

    //help
    if (strcmp(command->name, "help") == 0)
    {
        printf("help             list of commands\n");
        for (uint32_t i = 0; i < commands_count; i++)
        {
            printf("%-16s %s\n", commands[i].name, commands[i].help);
        }
        return;
    }

    //поиск в таблице
    for (uint32_t i = 0; i < commands_count; i++)
    {
        if (strcmp(command->name, commands[i].name) == 0)
        {
            commands[i].callback(command);
            return;
        }
    }

    //неизвестная команда
    printf("error: unknown command '%s', try help\n", command->name);
}