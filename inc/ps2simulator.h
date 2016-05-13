#ifndef NULL
#define NULL 0
#endif

#define CLOCK_LENGTH_2 35
#define CLOCK_LENGTH 2*CLOCK_LENGTH_2

static unsigned char* data_pin = NULL;
static unsigned char data_bit = 0;
static unsigned char* clock_pin = NULL;
static unsigned char clock_bit = 0;
static unsigned char i = 0;
static unsigned char parity = 0;
static unsigned char tempbit = 0;

void ps2sim_init(unsigned char* datapin, unsigned char databit, unsigned char* clockpin, unsigned char clockbit);
void ps2sim_send(unsigned char buffer);
static void ps2sim_setdata(unsigned char value);
static void ps2sim_setclock(unsigned char value);