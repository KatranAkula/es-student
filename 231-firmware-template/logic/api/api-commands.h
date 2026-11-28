#pragma once

#include "stdio-text-protocol/stdio-text-protocol.h" //command_t для типа обработчика

typedef void (*api_callback_t)(const command_t *command); //тип обработчика

typedef struct //запись таблицы команд
{
    const char *name;  //имя команды
    const char *help;  //справка для help
    api_callback_t callback; //обработчик
} api_command_t;

extern const api_command_t api_commands[]; //таблица определена в api-callbacks.c
extern const uint32_t api_commands_count;  //число команд
