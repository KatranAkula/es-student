#include "pico/stdlib.h"
//#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>        // для strcmp()
#include "led.h"
#include "log.h"
#include "device.h"        // для device_info()
#include "memory.h"        // для mem_info()
#include "command.h"   // доьавка 2.1.4
#include "clock.h"   // для clk_info 2,2,1
#include "profiling.h"

//const uint BUTTON_PIN = 15;
//const uint DEBOUNCE_MS = 20;

//буфер строки и его длина (из прошлого задания) 
#define LINE_SIZE 32

char line[LINE_SIZE];
uint line_length = 0;

// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с
const uint CALC_PI_TERMS = 1000000;
//один проход: 175 + 110 + 190 + 110 = 585 тактов
//миллион: 1 000 000 × 585 = 585 000 000 тактов
//такты в секунды: Частота процессора — 125 МГц (125 000 000 тактов в секунду) 585 000 000 / 125 000 000 = 4,68 секунды


double calc_pi(uint terms)
{
    double sum = 0.0;      // накопитель суммы
    double sign = 1.0;     // знак очередного члена: +1, -1, +1...

    for (uint k = 0; k < terms; k++)
    {
        sum += sign / (2.0 * k + 1.0);  // прибавляем sign / (2k+1)
        sign = -sign;                    // меняем знак на противоположный
    }

    return sum * 4.0;      // ряд Лейбница даёт π/4, умножаем на 4
}

/*
bool get_button_debounce(uint pin) //обработка дребезга
{
    bool state = gpio_get(pin); //объявляем переменную и присваеваем статуса кнопки 1 или 0
    sleep_ms(DEBOUNCE_MS);      //ждёт задержки в миллисек
    return state && gpio_get(pin); // присваивает state  1 при повторной 1
}
*/
// тип указатель на функцию без аргументов и возврата

// вынести тела команд в отдельные функции 

/*
void cmd_enable(void)
{
    led_set(true);
}

void cmd_disable(void)
{
    led_set(false);
}
*/
void cmd_uptime(void) // вместо cmd_enable и cmd_disable
{
    uptime();
}
void cmd_info(void)
{
    device_info();
}

void cmd_version(void)
{
    log_version();
}

//команда ping
void cmd_ping(void)
{
    printf("pong\n");
}

void cmd_mem_info(void)
{
    mem_info();
}

void cmd_dev_info(void)          //обработчик команды 
{
    dev_info();                  //вызывает функцию из device.c
}

//команда fw_info 2.1.4
void cmd_fw_info(void)
{
    fw_info();
}

void cmd_boot_info(void) //обработчик по заданию 2.1.6
{
    boot_info();
}

void cmd_clk_info(void) //обработчик по заданию 2,2,1 (частоты)
{
    clk_info();
}

void cmd_main_time_exec(void)
{
    printf("iteration avg %.2f us, max %u us\n",
           profiling_avg_us(),
           (unsigned)profiling_max_us());
}

void cmd_main_time_reset(void)
{
    profiling_reset_max();
    printf("max reset\n");
}

volatile double pi_result;

void cmd_calc_pi(void)
{
    uint64_t start_us = time_us_64();       //снимаее время до расчёта
    pi_result = calc_pi(CALC_PI_TERMS);     //считаее (запись в volatile!)
    uint64_t spent_us = time_us_64() - start_us; //снимаем время после

    printf("pi: %.8f\n", pi_result);        //выводит результат
    printf("time: %llu ms\n", spent_us / 1000); //выводит время в мс
}

void cmd_clk_sys_low(void)
{
    clk_sys_low();
}

void cmd_clk_sys_default(void)
{
    clk_sys_default();
}


//  структура «имя + обработчик» и таблица команд 
const struct command_t commands[] = {
    //{ "enable",  cmd_enable  },
    //{ "disable", cmd_disable },
    { "uptime", cmd_uptime }, //вместо cmd_enable и cmd_disable
    { "info",    cmd_info    },
    { "version", cmd_version }, 
    { "ping",    cmd_ping    },
    { "mem_info", cmd_mem_info },
    {"fw_info",  cmd_fw_info},   //добавить новую команду 2.1.4
    { "dev_info", cmd_dev_info }, //новая строка в таблице команд
    { "boot_info", cmd_boot_info }, //по заданию 2.1.6
    { "clk_info", cmd_clk_info }, //накрутка для 2,2,1
    { "calc_pi", cmd_calc_pi }, //2,2,3
    { "main_time_exec", cmd_main_time_exec },
    { "main_time_reset", cmd_main_time_reset },
    { "clk_sys_low", cmd_clk_sys_low },
    { "clk_sys_default", cmd_clk_sys_default },
};

// длина таблицы вынесена наружу как константа
const uint command_count = sizeof(commands) / sizeof(commands[0]);

void handle_command(const char *command) //пришла строка
{
    for (uint i = 0; i < command_count; i++)
    {
        if (strcmp(command, commands[i].name) == 0)
        {
            if (commands[i].handler != NULL)
            {
                commands[i].handler();
            }

            return;
        }
    }

    LOG_ERR("unknown command: %s\n", command); //неизвестная команда идёт уровнем ERR
}

// приём строки из символов
void read_line(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return;
    }

    if (symbol == '\r' || symbol == '\n')
    {
        putchar('\n');
        line[line_length] = '\0';

        if (line_length > 0)
        {
            LOG_DBG("got %s\n", line);
            handle_command(line);
        }

        line_length = 0;
        return;
    }

    if (line_length + 1 < LINE_SIZE)
    {
        line[line_length] = (char)symbol;
        line_length = line_length + 1;
        putchar(symbol);
    }
}


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


int main()
{
    
    stdio_init_all(); // вместо обработок кнопок и команд
    led_init();

    profiling_init(); // ← добавь эту строку

    while (1)
    {
        profiling_iteration(); //замеряется вся итерация целиком:  мигание, приём команд
        blink();
        read_line();
    }


}
