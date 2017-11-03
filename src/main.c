#include <avr/io.h>
#include <util/delay.h>

// Set blink delay
#define BLINK_DELAY_MS 1234

void main (void)
{
    DDRB |= _BV(DDB7);
    DDRA |= _BV(DDA0);
    DDRA |= _BV(DDA2);
    DDRA |= _BV(DDA4);
    PORTB &= ~_BV(PORTB7);

    while (1) {
        // Set port A pin 22 high to turn Arduino Mega red LED on
        PORTA |= _BV(PORTA0);
        _delay_ms(BLINK_DELAY_MS);
        // Set port A pin 22 high to turn Arduino Mega red LED off
        PORTA &= ~_BV(PORTA0);
        _delay_ms(BLINK_DELAY_MS);
        // Set port A pin 24 high to turn Arduino Mega green LED on
        PORTA |= _BV(PORTA2);
        _delay_ms(BLINK_DELAY_MS);
        // Set port A pin 24 high to turn Arduino Mega green LED off
        PORTA &= ~_BV(PORTA2);
        _delay_ms(BLINK_DELAY_MS);
        // Set port A pin 26 high to turn Arduino Mega blue LED on
        PORTA |= _BV(PORTA4);
        _delay_ms(BLINK_DELAY_MS);
        // Set port A pin 26 high to turn Arduino Mega blue LED off
        PORTA &= ~_BV(PORTA4);
        _delay_ms(BLINK_DELAY_MS);
    }
}

