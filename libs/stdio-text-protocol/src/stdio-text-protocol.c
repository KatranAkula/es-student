#include "stdio-text-protocol/stdio-text-protocol.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

static char line_buffer[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static uint32_t line_length = 0;
static bool line_truncated = false;

void stdio_text_protocol_init(void)
{
    line_length = 0;
    line_truncated = false;
}

const command_t *stdio_text_protocol_handle(void)
{
    static command_t command; // сохраняет результат между вызовами

    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    // Конец строки
    if (symbol == '\r' || symbol == '\n')
    {
        if (line_length == 0)
        {
            return NULL; // пустая строка
        }

        //завершает строку нулевым символом
        line_buffer[line_length] = '\0';

        //инициализирует команду
        command.argc = 0;
        command.name = NULL;
        command.truncated = line_truncated;

        //делит строку на слова, заменяя пробелы на '\0'
        bool in_word = false;
        for (uint32_t i = 0; i <= line_length; i++)
        {
            if (line_buffer[i] == ' ' || line_buffer[i] == '\0')
            {
                if (in_word)
                {
                    line_buffer[i] = '\0';
                    in_word = false;
                }
            }
            else
            {
                if (!in_word)
                {
                    in_word = true;
                    if (command.name == NULL)
                    {
                        command.name = &line_buffer[i];
                    }
                    else if (command.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS)
                    {
                        command.argv[command.argc] = &line_buffer[i];
                        command.argc++;
                    }
                    else
                    {
                        // слишком много аргументов
                        command.truncated = true;
                    }
                }
            }
        }

        //сбрасывает буфер для следующей строки
        line_length = 0;
        line_truncated = false;

        if (command.name == NULL)
        {
            return NULL; //строка из одних пробелов
        }

        //так указано в А.4 - на конец кадра — CR LF, затем кадр ответа
        putchar('\r');
        putchar('\n');

        return &command;
    }

    //кнопка удаления символа
    if (symbol == 0x08 || symbol == 0x7f)
    {
        if (line_length > 0)
        {
            line_length--;
            line_truncated = false; //снимает пометку о переполнении
            putchar('\b');
            putchar(' ');
            putchar('\b');
        }
        return NULL;
    }

    //печатный символ
    if (symbol >= 0x20 && symbol <= 0x7e)
    {
        if (line_length < STDIO_TEXT_PROTOCOL_LINE_SIZE - 1)
        {
            line_buffer[line_length] = (char)symbol;
            line_length++;
            putchar(symbol); //эхо
        }
        else
        {
            line_truncated = true; //буфер полон
        }
    }

    return NULL;
}