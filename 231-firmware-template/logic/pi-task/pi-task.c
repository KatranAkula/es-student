// pi-task.c — расчёт π по порциям, не блокируя суперцикл
#include "pi-task.h"
#include "systime/systime.h"  //время — из библиотеки

//цена члена ряда около 585 тактов при 125 МГц - 4,7 мкс порция должна уложиться в 0,5 мс: 500 мкс / 4,7 мкс примерно 106 — берём 100 — с запасом.
#define PI_TERMS_PER_CALL 100

//переменные задачи
static pi_state_t pi_state;

static uint32_t pi_k;  //номер текущего члена
static uint32_t pi_n;   //всего членов
static double   pi_sum;
static double   pi_sign;
static double   pi_result;

static uint64_t pi_start_us;  //время начала
static uint32_t pi_spent_ms;  //время расчёта в мс

bool pi_task_start(uint32_t n)
{
    if (pi_state == PI_STATE_RUNNING) {
        return false;  //уже считает
    }

    pi_n        = n;
    pi_k        = 0;
    pi_sum      = 0.0;
    pi_sign     = 1.0;
    pi_result   = 0.0;
    pi_spent_ms = 0;
    pi_start_us = systime_us();
    pi_state    = PI_STATE_RUNNING;
    return true;
}

void pi_task_handle(void)
{
    if (pi_state != PI_STATE_RUNNING) { //не запущен - выходим
        return;
    }

    //конец порции, но не дальше последнего члена возле UINT32_MAX pi_k + PI_TERMS_PER_CALL переполняется — тогда концом порции станет последний член
    uint32_t end = pi_k + PI_TERMS_PER_CALL; //конец порции: 100 членов, но не дальше pi_n
    if (end < pi_k || end > pi_n) {
        end = pi_n;
    }

    for (; pi_k < end; pi_k++) { //теперь вычисление Пи тут
        pi_sum += pi_sign / (2.0 * pi_k + 1.0); //формула Лейбница
        pi_sign = -pi_sign; //смена знака
    }

    if (pi_k >= pi_n) {
        pi_result   = 4.0 * pi_sum;
        pi_spent_ms = (uint32_t)((systime_us() - pi_start_us) / 1000);
        pi_state    = PI_STATE_READY;
    }
}

bool pi_task_get_result(double *result, uint32_t *spent_ms)
{
    if (pi_state != PI_STATE_READY) {
        return false;
    }
    *result   = pi_result;
    *spent_ms = pi_spent_ms;
    return true;
}

pi_state_t pi_task_get_state(void)
{
    return pi_state;
}
