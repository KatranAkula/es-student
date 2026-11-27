#include "systime/systime.h"
#include "pico/stdlib.h" //time_us_64 — SDK только реализации

uint64_t systime_us(void)
{
    return time_us_64(); //часы микроконтроллера
}

uint32_t systime_ms(void)
{
    return (uint32_t)(time_us_64() / 1000); //мс с момента сброса
}


//тут обработка 
bool systime_period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    uint64_t now_us = time_us_64(); //читаем часы один раз

    if (now_us - *last_us < period_us) { //период ещё не истёк?
        return false;                    //нет — выходим
    }

    *last_us += period_us; //следующее — от прошлой метки (+= ищет проверка)

    if (now_us - *last_us >= period_us) { //отстали больше чем на период?
        *last_us = now_us;                //да — не догоняем, начинаем заново
    }

    return true; //период истёк
}
