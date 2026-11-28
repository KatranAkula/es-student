#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>  //NULL для типа callback

//что вызвать при нажатии кнопки — решает main.c
typedef void (*button_task_callback_t)(void);

void button_task_init(button_task_callback_t on_press); //инициализация, один раз из main
void button_task_handle(void); //обработчик, зовёт суперцикл на каждой итерации

bool button_task_is_pressed(void); //кнопка нажата?
uint32_t button_task_get_press_count(void); //число подтверждённых нажатий
