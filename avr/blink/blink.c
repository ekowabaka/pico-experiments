#include <avr/io.h>

int main()
{
    DDRB |= 0b00100000;

    while (1)
    {        
        PORTB |= 0b00100000;

        for (long i = 0; i < 100000; i++)
        {
            PORTB |= 0b00100000;
        }
        PORTB &= 0b11011111;
        for (long i = 0; i < 100000; i++)
        {
            PORTB &= 0b11011111;
        }
    }
}