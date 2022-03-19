#ifndef __CONVENIENCE__
#define __CONVENIENCE__

#define SET(BYTE, BIT) (BYTE |= (1 << BIT))
#define CLR(BYTE, BIT) (BYTE &= ~(1 << BIT))
#define CHECK_BIT(NUMBER, BIT) (NUMBER & (1UL << BIT))

#define TRUE 0xFF
#define FALSE 0x0

typedef unsigned char BOOL;
typedef unsigned char BYTE;

void cputcn(char c, char n) {
    int i;
    for (i = 0; i < n; i++)
    {
        cputc(c);
    }
}

void cputsn(char* c, char n) {
    int i;
    for (i = 0; i < n; i++)
    {
        if (c[i] == 0) return;
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
#endif