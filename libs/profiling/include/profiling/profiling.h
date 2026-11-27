#pragma once
#include <stdbool.h>
#include <stdint.h>

// Максимум секундомеров
#define PROFILING_MAX_STOPWATCHES 8



//суммы замеров хранятся в 32 битах.
//сумма одного секундомера переполнится только через 71 минуту непрерывных замеров (2^32 мкс ≈ 71 мин).
typedef struct {
    const char *name;//имя секундомера
    uint32_t    count; //число замеров
    //uint32_t    mean_us;//среднее арифметическое, мкс - замена на double
    double      mean_us; //для выводы дроби
    uint32_t    max_us;//максимум, мкс
} profiling_result_t;

void profiling_stopwatch_init(uint32_t id, const char *name);

void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);



//false — секундомер с таким номером не запущен
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);