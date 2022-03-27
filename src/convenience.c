#include <stdlib.h>
#include <c64.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void cputcn(char c, char n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        cputc(c);
    }
}

void cputsn(char *c, char n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (c[i] == 0)
            return;
        cputc(c[i]);
    }
}

void c_sleep(unsigned int d)
{
    int i;
    for (i = 0; i < d; i++)
    {
    }
}

void print_wrapped2(char *s, unsigned char x, unsigned char y, unsigned char max_width)
{
    int i = 0;

    char *res;
    int len = strlen(s);
    unsigned char line = 0;
    unsigned int line_index = 0;
    unsigned int next_space_index = 0;
    unsigned int next_space_len = 0;

    for (i = 0; i < len; i++)
    {
        if (s[i] == '\n')
        {
            line++;
            line_index = 0;
            continue;
        }

        if (s[i] == ' ')
        {
            res = strchr(s + i + 1, ' ');
            if (res != NULL)
            {
                next_space_index = (int)(res - s);
                next_space_len = next_space_index - i;
            }
            else
            {
                next_space_len = len - i;
            }

            if ((line_index + next_space_len) > max_width)
            {
                line++;
                line_index = 0;
                i++;
            }
        }

        if (line_index > max_width)
        {
            line++;
            line_index = 0;
        }

        gotoxy(x + line_index, y + line);
        cputc(s[i]);
        line_index++;

        // gotoxy(0, 19);
        // printf(" i: %d, line_index: %d         \n", i, line_index);
        // printf(" line: %d, next_space_index: %d     ", line, next_space_index);
        // gotoxy(x + line_index, y + line);
        // // cgetc();
    }
}
