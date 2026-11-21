#include "profiling.h" //Подключаб заголовок модуля длякомпилятора
#include "pico/stdlib.h" //Подключаю SDK для функции time_us_32()


#define AVG_SHIFT 19 // итерация около 2 мкс, за секунду их около 500 000, ближайшая степень двойки 2^19 = 524 288, среднее помнит около секунды

#define FRACTION_SHIFT 8 // замер в 1/256 долях микросекунды, чтобы среднее не теряло дробную часть




static uint32_t previous_us = 0;
//static — переменную видит только этот файл (profiling.c)
//uint32_t — 32-битное беззнаковое число (микросекунды)
//previous_us — хранит показание часов с прошлой итерации
//= 0 — изначально ноль, пока profiling_init() не запустится

static uint32_t max_us = 0; //хранит самую долгую итерацию, видна только внутри profiling.c, снаружи — через функцию

static uint64_t avg_sum = 0;

void profiling_init(void) //«точка отсчёта» для первого замера, вызывается один раз в main() до суперцикла
{
    previous_us = time_us_32();
}

void profiling_iteration(void) //вызывается в начале каждой итерации суперцикла, обновляет максимум, запоминает время для следующего замера
{
    uint32_t now_us = time_us_32();
    uint32_t iteration_us = now_us - previous_us;
    previous_us = now_us;

    if (iteration_us > max_us)
    {
        max_us = iteration_us;
    }
    
    avg_sum = avg_sum - (avg_sum >> AVG_SHIFT) + ((uint64_t)iteration_us << FRACTION_SHIFT); // Обновление среднего (без float и деления)
}

float profiling_avg_us(void) //возвращает среднее время итерации
{
    return (float)(avg_sum >> AVG_SHIFT) / (1 << FRACTION_SHIFT);
}

uint32_t profiling_max_us(void) //отдаёт текущее значение max_us наружу
{
    return max_us;
}

void profiling_reset_max(void) //сбрасывает максимум в ноль по команде main_time_reset
{
    max_us = 0;
}