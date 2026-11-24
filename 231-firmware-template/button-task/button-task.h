// button-task.h — задача кнопки: подавление дребезга машиной состояний
#pragma once

#include <stdbool.h>
#include <stdint.h>

void button_task_init(void);//инициализация GPIO кнопки, один раз из main до суперцикла
void button_task_handle(void); //обработчик машины состояний, зовёт суперцикл на каждой итерации
bool button_task_is_pressed(void); //нажата ли кнопка — для команды button
uint32_t button_task_get_press_count(void); //сколько нажатий засчитано — для команды button
