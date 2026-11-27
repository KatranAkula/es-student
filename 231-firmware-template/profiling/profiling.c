#include "profiling.h" //заголовок модуля: типы и прототипы
#include "pico/stdlib.h"    //SDK: нужен time_us_64()

typedef struct {   //внутренняя структура одного секундомера
    const char *name;  //имя: "loop", "led", "pi"...
    uint32_t    count;    //сколько раз останавливали
    uint32_t    sum_us;  //сумма всех замеров, мкс (32 бита — до 71 мин)
    uint32_t    max_us;   //самый долгий замер, мкс
    uint64_t    start_us; //время старта текущего замера, мкс
    bool        used;  //секундомер заведён? (init вызывали)
} stopwatch_t;  //typedef: дальше пишем stopwatch_t, а не struct

static stopwatch_t stopwatches[PROFILING_MAX_STOPWATCHES]; //массив секундомеров, static = только этот файл

void profiling_stopwatch_init(uint32_t id, const char *name) //завести секундомер с номером id
{
    if (id >= PROFILING_MAX_STOPWATCHES) return; //номер вне массива — молча выходим

    stopwatches[id].name     = name;    //запоминаем имя
    stopwatches[id].count    = 0;   //замеров пока нет
    stopwatches[id].sum_us   = 0; //сумма ноль
    stopwatches[id].max_us   = 0;   //максимум ноль
    stopwatches[id].start_us = 0;   //старт не было
    stopwatches[id].used     = true; //помечаем: секундомер существует
}

void profiling_start(uint32_t id)   //    запустить замер
{
    if (id >= PROFILING_MAX_STOPWATCHES || !stopwatches[id].used) return; //нет такого — выходим
    stopwatches[id].start_us = time_us_64(); //запоминаем момент старта в мкс
}

void profiling_stop(uint32_t id)   //остановить замер и учесть его
{
    if (id >= PROFILING_MAX_STOPWATCHES || !stopwatches[id].used) return; //нет такого — выходим

    uint32_t spent = (uint32_t)(time_us_64() - stopwatches[id].start_us); //сколько прошло, мкс
    stopwatches[id].count++;    // ещё один замер
    stopwatches[id].sum_us += spent; //добавляем в сумму (переполнится через 71 мин)
    if (spent > stopwatches[id].max_us) { //если это самый долгий замер
        stopwatches[id].max_us = spent;  //обновляем максимум
    }
}

bool profiling_get(uint32_t id, profiling_result_t *result) //забрать результат по номеру
{
    if (id >= PROFILING_MAX_STOPWATCHES || !stopwatches[id].used) return false; //нет такого — false

    result->name    = stopwatches[id].name; //имя
    result->count   = stopwatches[id].count; //число замеров
    //result->mean_us = stopwatches[id].count ? stopwatches[id].sum_us / stopwatches[id].count : 0; //среднее (защита от /0)
    result->mean_us = stopwatches[id].count ? (double)stopwatches[id].sum_us / stopwatches[id].count//коррекция для вывода с дробью
    : 0.0;
    result->max_us  = stopwatches[id].max_us; //максимум
    return true;        ///результат есть
}

void profiling_reset(void)              //сбросить все секундомеры
{
    for (uint32_t i = 0; i < PROFILING_MAX_STOPWATCHES; i++) { // по всем номерам
        if (stopwatches[i].used) { //если секундомер заведён
            stopwatches[i].count  = 0; //замеров ноль
            stopwatches[i].sum_us = 0;  //сумма ноль
            stopwatches[i].max_us = 0; //максимум ноль
        }
    }
}