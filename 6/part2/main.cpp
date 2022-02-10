#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define degree_sysmbol 0xdf

volatile uint8_t timer0_total_ovf = 0;

char d[] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

int hr[] = {25, 30};

void timer0_init()
{
   
    TCCR0 |= (1 << CS01) | (1 << CS00);

    // this timer
    TCNT0 = 0;


    TIMSK |= (1 << TOIE0);
    sei();
}

void ADC_Init()
{
    ADCSRA = 0x87; 
    ADMUX = 0x40;  
}

int adc(char channel)
{
    ADMUX = 0x40 | (channel & 0x07);
    ADCSRA |= (1 << ADSC);
    while (!(ADCSRA & (1 << ADIF)))
        ;
    ADCSRA |= (1 << ADIF);
  de(1);
    return ADCW;
}

void lcd_write(char c)
{
    PORTC = c;
    PORTD |= (1 << 0);
    PORTD &= ~(1 << 1);
    PORTD |= (1 << 2);
    _delay_us(1);
    PORTD &= ~(1 << 2);
  de(2);
}

void lcd_command(char c)
{
    PORTC = c;
    PORTD &= ~(1 << 0);
    PORTD &= ~(1 << 1);
    PORTD |= (1 << 2);
    _delay_us(1);
    PORTD &= ~(1 << 2);
  de(2);
}

void lcd_init()
{
  de(20);

    lcd_command(0x38);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
  de(2);
    lcd_command(0x80);
}

void ud(char temp[])
{
    lcd_command(0x01);
    for (int i = 0; i < 16; i++)
    {
        lcd_write(temp[i]);
    }
}

int main(void)
{
    DDRC = 0xFF;

    DDRD = 0x0F;

    DDRA = 0x00;
    PORTA = 0x1E;

    lcd_init();
    ADC_Init();
    timer0_init();

    char hysteresis_str[16];
    char temp[16];
    float temp_celsius;

    while (1)
    {
        if (temp_celsius > hr[1])
        {
            PORTD |= (1 << 3);
        }

        if (temp_celsius < hr[0])
        {
            PORTD &= ~(1 << 3);
        }

        if (!(PINA & (1 << 1)))
        {
            hr[1] = hr[1] + 1;
            sprintf(hysteresis_str, "hy upper: %d%cC  ", (int)hr[1], degree_sysmbol);
            ud(hysteresis_str);
          de(500);
            while (!(PINA & (1 << 1)));
        }

        if (!(PINA & (1 << 2)))
        {
            hr[1]--;
            sprintf(hysteresis_str, "hy upper: %d%cC  ", (int)hr[1], degree_sysmbol);
            ud(hysteresis_str);
          de(500);
            while (!(PINA & (1 << 2)));
        }

        if (!(PINA & (1 << 3)))
        {
            hr[0]++;
            sprintf(hysteresis_str, "hy upper: %d%cC  ", (int)hr[0], degree_sysmbol);
            ud(hysteresis_str);
          de(500);
            while (!(PINA & (1 << 3)));
        }

        if (!(PINA & (1 << 4)))
        {
            hr[0]--;
            sprintf(hysteresis_str, "hy upper: %d%cC  ", (int)hr[0], degree_sysmbol);
            ud(hysteresis_str);
          de(500);
            while (!(PINA & (1 << 4)));
        }

        if (timer0_total_ovf >= 61)
        {
            temp_celsius = (adc(0) * 4.88); 
            temp_celsius = (temp_celsius / 10.00);
            sprintf(temp, "Temperature: %d%cC  ", (int)temp_celsius, degree_sysmbol);
            ud(temp);
          de(500);
            TCNT0 = 0;
            timer0_total_ovf = 0;
        }
    }
}

ISR(TIMER0_OVF_vect)
{
    timer0_total_ovf++;
}