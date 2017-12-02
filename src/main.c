#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "../lib/hd44780_111/hd44780.h"
#define BLINK_DELAY_MS 100
#include "hmi_msg.h"
#include "print_helper.h"

void init_leds(void)
{
    /* Set port B pin 7 for output for Arduino Mega yellow LED */
    DDRA |= _BV(DDA0);
    DDRA |= _BV(DDA2);
    DDRA |= _BV(DDA4);
    DDRB |= _BV(DDB7);
    PORTB &= ~_BV(PORTB7);
}

/* Init error console as stderr in UART1 and print user code info */
void init_errcon(void)
{
    simple_uart1_init();
    /*stderr = &simple_uart1_out;*/
    fprintf_P(stderr, PSTR(VER_FW),
              PSTR(FW_VERSION), PSTR(__DATE__), PSTR(__TIME__));
    fprintf_P(stderr, PSTR(VER_LIBC),
              PSTR(__AVR_LIBC_VERSION_STRING__), PSTR(__VERSION__));
}

/* lcd init and print on lcd screen student name*/
void student_lcd(void)
{
    init_leds();
    init_errcon();
    lcd_init();
    lcd_home();
    lcd_puts_P(PSTR(STUDENT_NAME));
}
/* led blinking in 3 colors*/
void blink_leds(void)
{
    PORTA |= _BV(PORTA0);
    _delay_ms(BLINK_DELAY_MS);
    /* Set port B pin 7 high to turn Arduino Mega yellow LED off */
    PORTA &= ~_BV(PORTA0);
    _delay_ms(BLINK_DELAY_MS);
    /* Set port B pin 6 high to turn Arduino Mega yellow LED off */
    PORTA |= _BV(PORTA2);
    _delay_ms(BLINK_DELAY_MS);
    /* Set port B pin 6 high to turn Arduino Mega yellow LED off */
    PORTA &= ~_BV(PORTA2);
    _delay_ms(BLINK_DELAY_MS);
    /* Set port B pin 5 high to turn Arduino Mega yellow LED off */
    PORTA |= _BV(PORTA4);
    _delay_ms(BLINK_DELAY_MS);
    /* Set port B pin 5 high to turn Arduino Mega yellow LED off */
    PORTA &= ~_BV(PORTA4);
    _delay_ms(BLINK_DELAY_MS);
}

void main(void)
{
    student_lcd();
    /* init uart0 */
    simple_uart0_init();
    /*stdin = stdout = &simple_uart0_io;*/ /* stdin=stdout stream */
    fprintf_P(stdout, PSTR(STUDENT_NAME)); /* print student name */
    /* ascii table */
    print_ascii_tbl(stdout);
    print_for_human(stdout);

    while (1) {
        /* Asks a number and return number in word*/
        int info;
        fprintf_P(stdout, PSTR(ENTER_NUMBER));
        fscanf(stdin, "%d", &info);
        fprintf(stdout, "%d\n", info);

        if (info >= 0 && info < 10) {
            fprintf_P(stdout, PSTR(REPLY_NUMBER), (PGM_P)pgm_read_word(&(numbers[info])));
        } else {
            fprintf_P(stdout, PSTR(WRONG_NUMBER));
        }

        blink_leds();
    }
}
