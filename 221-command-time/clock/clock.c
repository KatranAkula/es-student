#include "clock.h"

#include <stdio.h>    //printf()
#include "pico/stdlib.h"     //загружаемые типы SDK
#include "hardware/clocks.h"    //подключаю часть СДК  даёт имена clk_ref, clk_sys, clk_peri, clk_usb, clk_adc; прототип clock_get_hz(); прототип frequency_count_khz(); константы CLOCKS_FC0_SRC_VALUE_

static void row(const char *name, uint32_t set_khz, uint32_t measured_khz) //вывод в консоль одной строки таблицы: имя сигнала, настроенная частота, измеренная частота
{
    printf("%-8s %9u %12u\n", name, (unsigned)set_khz, (unsigned)measured_khz);
}

void clk_info(void)
{    
    printf("%-8s %9s %12s\n", "signal", "set_khz", "measured_khz"); //шапка таблицы: сигнал, настроенная частота, измеренная частота

    // clk_ref, clk_sys, clk_peri, clk_usb, clk_adc
    row("clk_ref",
        clock_get_hz(clk_ref) / 1000,
        frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_REF));

    row("clk_sys",
        clock_get_hz(clk_sys) / 1000,
        frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS));

    row("clk_peri",
        clock_get_hz(clk_peri) / 1000,
        frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI));

    row("clk_usb",
        clock_get_hz(clk_usb) / 1000,
        frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB));

    row("clk_adc",
        clock_get_hz(clk_adc) / 1000,
        frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC));

    
    printf("%-8s %9s %12u\n", "rosc", "-",  //для ROSC - настроенной частоты нет — прочерк, только измеренная
           (unsigned)frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC));
}