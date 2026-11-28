// pi-task.h — задача расчёта Пи шажками
#pragma once

#include <stdbool.h>
#include <stdint.h>


typedef enum {  // Состояния задачи
    PI_STATE_IDLE,// не запускался
    PI_STATE_RUNNING,  // считает
    PI_STATE_READY,     // готово
} pi_state_t;

// Запустить расчёт на n членов ряда
// false — уже идёт расчёт, повторный запуск отклонён
bool pi_task_start(uint32_t n);

// обработчик считает не больше PI_TERMS_PER_CALL членов за вызов
void pi_task_handle(void);

// Забрать результат.
// false — результат ещё не готов.
// *result — значение Пи, *spent_ms — время расчёта в мс.
bool pi_task_get_result(double *result, uint32_t *spent_ms);

// Текущее состояние (для команды pi)
pi_state_t pi_task_get_state(void);
