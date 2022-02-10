#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

char cycles[] PROGMEM = {0x09, 0x05, 0x06, 0x0A};

char sensors_wheels_stat[] PROGMEM = {10, 11, 10, 01, 00, 01, 00}; 

int main(void)
{
    while (1)
    {
        
    }
}