#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "../lib/hd44780_111/hd44780.h"
#define BLINK_DELAY_MS 100
#include "hmi_msg.h"
#include "print_helper.h"

static inline void init_leds(void)
{
    /* Set port B pin 7 for output for Arduino Mega yellow LED */
    DDRA |= _BV(DDA0);
    DDRA |= _BV(DDA2);
    DDRA |= _BV(DDA4);
    DDRB |= _BV(DDB7);
    PORTB &= ~_BV(PORTB7);
}

/* Init error console as stderr in UART1 and print user code info */
static inline void init_errcon(void)
{
    simple_uart1_init();
    stderr = &simple_uart1_out;
    fprintf_P(stderr, PSTR(VER_FW),
              PSTR(FW_VERSION), PSTR(__DATE__), PSTR(__TIME__));
    fprintf_P(stderr, PSTR(VER_LIBC),
              PSTR(__AVR_LIBC_VERSION_STRING__), PSTR(__VERSION__));
}

/* lcd init and print on lcd screen student name*/
static inline void student_lcd(void)
{
    init_leds();
    init_errcon();
    lcd_init();
    lcd_home();
    lcd_puts_P(PSTR(STUDENT_NAME));
}
/* led blinking in 3 colors*/
static inline void blink_leds(void)
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

static inline void print_text(void)
{
    blink_leds();
    int input = -1;
    printf_P(PSTR(ENTER_NUMBER));

    if (!scanf("%d", &input)) {
        // Hack to get around scanf behaviour for handling invalid input
        // See http://www.avrfreaks.net/forum/scanf-problem
        printf("%c\n", getc(stdin));
        printf_P(PSTR(NOT_NUMBER));
        lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
        lcd_goto(LCD_ROW_2_START);
        lcd_puts_P(PSTR("Not a number"));
        lcd_home();
    }

    if (input > -1) {
        printf("%i\n", input);
    }

    // Clear LCD and move to "home"
    if (input >= 0 && input < 10) {
        fprintf_P(stdout, PSTR(REPLY_NUMBER), (PGM_P)pgm_read_word(&(numbers[input])));
        lcd_goto(LCD_ROW_2_START);
        lcd_puts_P((PGM_P)pgm_read_word(&(numbers[input])));
    } else {
        fprintf_P(stdout, PSTR(WRONG_NUMBER));
        lcd_goto(LCD_ROW_2_START);
        lcd_puts_P(PSTR("Not a number"));
    }
}

void main(void)
{
    student_lcd();
    /* init uart0 */
    simple_uart0_init();
    stdin = stdout = &simple_uart0_io; /* stdin=stdout stream */
    fprintf_P(stdout, PSTR(STUDENT_NAME "\n")); /* print student name */
    /* ascii table */
    print_ascii_tbl(stdout);
    /* char array for ascii human table */
    unsigned char ascii[128] = {0};

    for (unsigned char i = 0; i < sizeof(ascii); i++) {
        ascii[i] = i;
    }

    print_for_human(stdout, ascii, 128);

    while (1) {
        print_text();
    }
}
