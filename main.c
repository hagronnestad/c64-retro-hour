#include <stdlib.h>
#include <c64.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "splash.h"
#include "hosts.h"
#include "episodes.h"

#define VERSION "v1.0.0"

typedef unsigned char BOOL;
typedef unsigned char BYTE;

#define TRUE 0xFF
#define FALSE 0x0

#define SET(BYTE, BIT) (BYTE |= (1 << BIT))
#define CLR(BYTE, BIT) (BYTE &= ~(1 << BIT))
#define CHECK_BIT(NUMBER, BIT) (NUMBER & (1UL << BIT))

#define PAGE_HOME 0
#define PAGE_ABOUT 1
#define PAGE_HOSTS 2
#define PAGE_EPISODES 3
#define PAGE_RANDOM_EPISODE 4

unsigned char current_page_index = PAGE_HOME;
unsigned char current_host_index = HOST_DAN;
unsigned int current_episode_index = 0;
unsigned int i, j;

BOOL episode_search_active = 0;
unsigned char episode_search_query_index = 0;
unsigned char episode_search_query_index_max = 14;
char episode_search_query[16] = {0}; // 15 chars max + null termination


void switch_page(char page);


// *** Convenience Methods ***

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

// *** UI ***
void draw_header(char *sub_title)
{
    textcolor(COLOR_WHITE);
    revers(1);

    cputsxy(0, 0, "                                        ");
    cputsxy(0, 1, " THE RETRO HOUR >                       ");
    cputsxy(18, 1, sub_title);
    cputsxy(0, 2, "                                        ");
    revers(0);
    chlinexy(0, 3, 40);
}

void draw_footer()
{
    textcolor(COLOR_WHITE);

    cputsxy(0, 22, "________________________________________");
    revers(1);
    cputsxy(0, 23, "  CLR   F1     F3     F5        F7      ");
    cputsxy(0, 24, "  HOME  ABOUT  HOSTS  EPISODES  RANDOM  ");
    revers(0);
}

// Clear everything between header and footer
void clear_display()
{
    for (i = 4; i < 25 - 3; i++)
    {
        cputsxy(0, i, "                                        ");
    }
}

void page_home_draw()
{
    episodes_count = sizeof(episodes) / sizeof(char *);

    draw_header("C64 Episode Guide");
    clear_display();

    gotoxy(1, 5);
    printf("\xBF DATABASE STATISTICS");
    gotoxy(3, 6);
    cputcn('\xA3', 19);
    gotoxy(3, 7);
    printf("UPDATED:   %s", episodes_update_date);
    gotoxy(3, 8);
    printf("EPISODES:  %d", episodes_count);
    gotoxy(3, 9);
    printf("VERSION:   %s", VERSION);

    textcolor(COLOR_LIGHTGREEN);
    // gotoxy(1, 16);
    // printf("> CREATED BY");
    // gotoxy(3, 17);
    // cputcn('\xA3', 10);
    gotoxy(3, 19);
    printf("HAG'S LAB");
    gotoxy(3, 20);
    printf("hag.yt");

    // i = rand() % episodes_count;
    // gotoxy(1, 15);
    // printf("TODAYS RANDOM EPISODE");
    // gotoxy(1, 17);
    // printf("EPISODE #%d:", i);
    // print_wrapped2(episodes[i], 1, 19, 38);
}

void page_about_draw()
{
    draw_header("About");
    clear_display();
    gotoxy(1, 5);
    printf("\xBF ABOUT");
    gotoxy(3, 6);
    cputcn('\xA3', 5);
    print_wrapped2("The Retro Hour Podcast is an independent audio project. Running weekly since 2016 the aim is to capture voices and pioneers in the video game world.  In our lifetime we have seen huge leaps in technology, games development and culture. We feel it is important to capture these stories and voices for future generations.", 1, 7, 38);
}

void page_hosts_draw()
{
    draw_header("Hosts");
    clear_display();

    cputcxy(1, 5, '\xBF');
    print_wrapped2(hosts_name[current_host_index], 3, 5, 38);
    gotoxy(3, 6);
    cputcn('\xA3', strlen(hosts_name[current_host_index]));
    print_wrapped2(hosts_bio[current_host_index], 1, 7, 38);
    current_host_index++;
    if (current_host_index == 3)
    {
        current_host_index = 0;
    }
}

void page_episodes_draw()
{
    char c;
    char tc;
    long query_int;

    draw_header("Episode Lookup");
    clear_display();

    gotoxy(1, 5);
    printf("\xBF EPISODE LOOKUP");
    gotoxy(3, 6);
    cputcn('\xA3', 14);

    gotoxy(3, 8);
    printf("Episode # %d", current_episode_index + 1);
    print_wrapped2(episodes[current_episode_index], 3, 10, 36);

    tc = textcolor(COLOR_LIGHTGREEN);
    gotoxy(3, 19);
    printf("      RETURN: Search");
    gotoxy(3, 20);
    printf("CRSR UP/DOWN: Previous/Next Episode");
    textcolor(tc);

    if (episode_search_active == 1) {
        cursor(1);
        gotoxy(3, 14);
        printf("QUERY: ");
        gotoxy(10, 14);
        
        while (1)
        {
            c = cgetc();

            if (c == 20) // Delete
            {
                if (episode_search_query_index == 0) continue;
                gotoxy(10 + episode_search_query_index - 1, 14);
                cputc(' ');
                episode_search_query[episode_search_query_index - 1] = 0;
                episode_search_query_index--;

                gotoxy(10, 14);
                printf("%s", episode_search_query);
            }

            if (c == 13) // Return
            {
                gotoxy(3, 15);
                printf("Searching...        ");

                episode_search_active = 0;
                query_int = strtol(episode_search_query, NULL, 10);

                if (query_int > 0 && query_int <= episodes_count) {
                    current_episode_index = query_int - 1;
                    goto done;
                }
                
                if (query_int == 0 && episode_search_query_index > 0) {
                    for (i = current_episode_index + 1; i < episodes_count; i++)
                    {
                        if (strstr(episodes[i], episode_search_query) != NULL)
                        {
                            current_episode_index = i;
                            goto done;
                        }
                    }

                    current_episode_index = 0;
                }

                gotoxy(0, 15);
                printf("                                        ");
                gotoxy(3, 15);
                printf("No more results!");
                cgetc();
                current_episode_index = 0;

done:
                episode_search_query_index = 0;
                episode_search_query[0] = 0;

                switch_page(PAGE_EPISODES);

                break;
            }

            // Ignore input if we're at max query length
            if (episode_search_query_index > episode_search_query_index_max) continue;

            // Accept numeric input
            if (c >= '0' && c <= '9')
            {
                episode_search_query[episode_search_query_index] = c;
                episode_search_query[episode_search_query_index + 1] = 0;
                episode_search_query_index++;

                gotoxy(10, 14);
                printf("%s", episode_search_query);
            }

            // Accept alpha input
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                episode_search_query[episode_search_query_index] = c;
                episode_search_query[episode_search_query_index + 1] = 0;
                episode_search_query_index++;

                gotoxy(10, 14);
                printf("%s", episode_search_query);
            }
        }

    } else {
        cursor(0);
    }
}

void page_random_episodes_draw()
{
    draw_header("Random Episode");
    clear_display();
    gotoxy(1, 5);
    printf("\xBF RANDOM EPISODE");
    gotoxy(3, 6);
    cputcn('\xA3', 14);

    i = rand() % episodes_count;
    gotoxy(3, 8);
    printf("Episode # %d", i + 1);
    print_wrapped2(episodes[i], 3, 10, 36);
}

void switch_page(char page)
{
    current_page_index = page;

    switch (page)
    {
    case PAGE_HOME:
        page_home_draw();
        break;

    case PAGE_ABOUT:
        page_about_draw();
        break;

    case PAGE_HOSTS:
        page_hosts_draw();
        break;

    case PAGE_EPISODES:
        page_episodes_draw();
        break;

    case PAGE_RANDOM_EPISODE:
        page_random_episodes_draw();
        break;

    default:
        break;
    }
}

void handle_input_for_page(char key)
{
    switch (current_page_index)
    {
    case PAGE_HOSTS:
        switch (key)
        {
        case 13: // RETURN
        case 32: // SPACE
            switch_page(PAGE_HOSTS);
            break;

        default:
            break;
        }
        break;

    case PAGE_EPISODES:
        switch (key)
        {
        case 145: // Up
            if (current_episode_index > 0)
            {
                current_episode_index--;
            } else
            {
                current_episode_index = episodes_count - 1;
            }
            switch_page(PAGE_EPISODES);
            break;
        case 17: // Down
            current_episode_index++;
            if (current_episode_index >= episodes_count)
            {
                current_episode_index = 0;
            }
            switch_page(PAGE_EPISODES);
            break;

        case 13: // Return
            episode_search_active = (episode_search_active == 0 ? 1 : 0);
            switch_page(PAGE_EPISODES);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

BOOL handle_input_global(key)
{
    switch (key)
    {
    case 19: // CLR/HOME
        switch_page(PAGE_HOME);
        break;

    case 133: // F1
        switch_page(PAGE_ABOUT);
        break;

    case 134: // F3
        switch_page(PAGE_HOSTS);
        break;

    case 135: // F5
        switch_page(PAGE_EPISODES);
        break;

    case 136: // F7
        switch_page(PAGE_RANDOM_EPISODE);
        break;

    default:
        return FALSE;
    }

    return TRUE;
}

int main(void)
{
    char key;

    clrscr();
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_GREEN);
    bgcolor(COLOR_GREEN);

    c_sleep(2000);

    _randomize();

    // *(char*)0xD018 = 0x14; // Set upper case
    printf("\x8E");

    // Logo Splash
    for (i = 0; i < 1000; i++) {
        if (splash_chars[i] == ' ') continue;
        *((char*)0x0400 + i) = splash_chars[i];
        c_sleep(5);
    }

    for (i = 0; i < 1000; i++) {
        if (splash_chars[i] == ' ') continue;
        *((char*)0xD800 + i) = splash_colors[i];
        c_sleep(5);
    }

    cgetc();

    kbrepeat(KBREPEAT_ALL);

    bordercolor(COLOR_RED);

    // *(char*)0xD018 = 0x15; // Set lower case
    printf("\x0E");

    clrscr();
    draw_header("");
    draw_footer();
    clear_display();

    switch_page(PAGE_HOME);

    for (;;)
    {
        if (kbhit())
        {
            key = cgetc();

            if (!handle_input_global(key))
            {
                handle_input_for_page(key);
            }
        }
    }

    return 0;
}
