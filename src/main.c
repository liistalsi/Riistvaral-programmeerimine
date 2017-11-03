#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 1000

void main (void)
{
    /* Set port H pin 7 for output for Arduino Mega yellow LED */
    DDRB |= _BV(DDB7);

    while (1) {
        /* Set port H4 pin 7 high to turn Arduino Mega yellow LED on */
        PORTB |= _BV(PORTB7);
        _delay_ms(BLINK_DELAY_MS);
        /* Set port H4 pin 7 high to turn Arduino Mega yellow LED off */
        PORTB &= ~_BV(PORTB7);
        _delay_ms(BLINK_DELAY_MS);
    }
}

