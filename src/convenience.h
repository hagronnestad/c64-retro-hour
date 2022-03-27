#define SET(BYTE, BIT) (BYTE |= (1 << BIT))
#define CLR(BYTE, BIT) (BYTE &= ~(1 << BIT))
#define CHECK_BIT(NUMBER, BIT) (NUMBER & (1UL << BIT))

#define TRUE 0xFF
#define FALSE 0x0

typedef unsigned char BOOL;
typedef unsigned char BYTE;

void cputcn(char c, char n);
void cputsn(char *c, char n);
void c_sleep(unsigned int d);
void print_wrapped2(char *s, unsigned char x, unsigned char y, unsigned char max_width);
