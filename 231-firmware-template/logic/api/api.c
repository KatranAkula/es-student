#include "api.h"          //свой заголовок первым
#include "api-commands.h" //внутренний: таблица команд

#include <stdio.h>
#include <string.h>

//механизм api: ответ на ошибки кадра, help, поиск по таблице, неизвестная команда

void api_handle(const command_t *command)//главный обработчик
{
    //переполнение буфера (truncated)
    if (command->truncated)  //строка не поместилась?
    {
        printf("error: command longer than 63 characters or 4 arguments\n"); // ошибка
        return;
    }

    //help
    if (strcmp(command->name, "help") == 0) //команда help?
    {
        printf("help             list of commands\n");  //заголовок

        for (uint32_t i = 0; i < api_commands_count; i++)//по всем командам
        {
            printf("%-16s %s\n", api_commands[i].name, api_commands[i].help); //имя и справка
        }
        return;
    }

    //поиск в таблице
    for (uint32_t i = 0; i < api_commands_count; i++) //по всем командам
    {
        if (strcmp(command->name, api_commands[i].name) == 0)// имя совпало?
        {
            api_commands[i].callback(command);  //вызвать обработчик
            return;
        }
    }

    printf("error: unknown command '%s', try help\n", command->name); //неизвестная команда - ошибка
}
