#pragma once
#include <stdbool.h>


void platform_init(void); // запускает стандартный ввод-вывод и настраивает выводы

void platform_led_set(bool on);


bool platform_button_read(void); // нажата ли кнопка сейчас, без подавления дребезга
