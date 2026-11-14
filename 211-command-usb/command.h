#pragma once

#include "pico/types.h"

// тип «указатель на функцию без аргументов и возврата» =====
// (перенесено из main.c — 2.1.4)
typedef void (*command_handler_t)(void);

// ===== структура «имя + обработчик» =====
// (перенесено из main.c — 2.1.4)
struct command_t
{
    const char *name;
    command_handler_t handler;
};

//объявления для внешнего мира (2.1.4) extern обязателен: без него это определение, и каждый .c,
// который включит заголовок, создаст свою копию → multiple definition.
extern const struct command_t commands[];
extern const uint command_count;