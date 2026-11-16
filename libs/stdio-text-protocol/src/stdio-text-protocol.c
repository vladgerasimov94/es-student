#include "stdio-text-protocol/stdio-text-protocol.h"

#include <stddef.h>
#include <stdio.h>
#include "pico/stdlib.h"

static char line[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static size_t line_length;
static bool overflow;
static command_t command;

void stdio_text_protocol_init(void)
{
    line_length = 0;
    overflow = false;
}

static const command_t *finish_line(void)
{
    line[line_length] = '\0';
    command = (command_t){ .truncated = overflow };

    for (size_t i = 0; i < line_length; i++)
    {
        if (line[i] == ' ')
        {
            line[i] = '\0';
        }
        else if (i == 0 || line[i - 1] == '\0')
        {
            if (command.name == NULL)
            {
                command.name = &line[i];
            }
            else if (command.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS)
            {
                command.argv[command.argc++] = &line[i];
            }
            else
            {
                command.truncated = true;
            }
        }
    }

    line_length = 0;
    overflow = false;
    return command.name != NULL || command.truncated ? &command : NULL;
}

const command_t *stdio_text_protocol_handle(void)
{
    int symbol = getchar_timeout_us(0);

    if (symbol == '\r' || symbol == '\n')
    {
        if (line_length != 0)
        {
            putchar('\n');
            return finish_line();
        }
    }
    else if (symbol == '\b' || symbol == 0x7f)
    {
        if (line_length != 0)
        {
            line_length--;
            overflow = false;
            printf("\b \b");
        }
    }
    else if (symbol >= 0x20 && symbol <= 0x7e)
    {
        if (line_length < sizeof(line) - 1)
        {
            line[line_length++] = (char)symbol;
            putchar(symbol);
        }
        else
        {
            overflow = true;
        }
    }

    return NULL;
}
