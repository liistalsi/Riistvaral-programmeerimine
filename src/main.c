#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h> // stdlib is needed to use ltoa()
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "hmi_msg.h"
#include "print_helper.h"
#include "cli_microrl.h"
#include "../lib/helius_microrl/microrl.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"

#define UART_BAUD 9600
#define UART_STATUS_MASK 0x00FF

#define BLINK_DELAY_MS 900
#define LED_RED PORTA0 // Arduino Mega digital pin 22
#define LED_GREEN PORTA2 // Arduino Mega digital pin 24

/* Define counter scale here NB! Uncomment only one */
#define COUNT_SECONDS         // seconds
//#define COUNT_1_10_SECONDS  // 1/10 seconds
//#define COUNT_1_100_SECONDS // 1/100 seconds


volatile uint32_t counter_1; //Global seconds counter
// prev_time = 0; //Heartbeat time placeholder
// now = 0; //Heartbeat time placeholder

// Create microrl object and pointer on it
microrl_t rl;
microrl_t *prl = &rl;


static inline void init_leds(void)
{
    /* RGB LED board set up and off */
    DDRA |= _BV(LED_RED) | _BV(LED_GREEN);
    PORTA &= ~(_BV(LED_RED) | _BV(LED_GREEN));
}


static inline void init_con_uarts(void)
{
    uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart0_puts_p(STUDENT_NAME);
    uart0_puts_p(PSTR("\r\n"));
    uart0_puts_p(PSTR("Use backspace to delete entry and enter to confirm.\r\n"));
    uart0_puts_p(PSTR("Arrow keys and del doesn`t work currently.\r\n"));
    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart1_puts_p(START_NOTE);
    uart1_puts_p(PSTR("\r\n"));
    uart1_puts_p(STUDENT_NAME);
    uart1_puts_p(PSTR("\r\n"));
    uart1_puts_p(PSTR(VER_FW));
    uart1_puts_p(PSTR(VER_LIBC));
    uart1_puts_p(PSTR("\r\n"));
    // call init with ptr to microrl instance and print callback
    microrl_init (prl, uart0_puts);
    // set callback for execute
    microrl_set_execute_callback (prl, cli_execute);
}


static inline void init_counter_1(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= _BV(WGM12); // Turn on CTC (Clear Timer on Compare)
    TCCR1B |= _BV(CS12); // fCPU/256
    OCR1A = 62549; // Note that it is actually two registers OCR5AH and OCR5AL
    TIMSK1 |= _BV(OCIE1A); // Output Compare A Match Interrupt Enable
}


static inline void heartbeat(void)
{
    static uint32_t prev_time;
    uint32_t now = 0;
    char print_buf[11] = {0x00}; // Buffer lagre enough to hold all long (uint32_t) digits
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        now = counter_1;
    }

    if (prev_time != now) {
        ltoa(now, print_buf, 10); // Convert a long integer to a string.
        uart1_puts_p(PSTR("Uptime: "));
        uart1_puts(print_buf);
        uart1_puts_p(PSTR(" s.\r\n"));
        PORTA ^= _BV(LED_GREEN);
        prev_time = now;
    }
}


void main (void)
{
    lcd_init();
    lcd_home();
    lcd_puts_P(STUDENT_NAME);
    sei(); // Enable all interrupts. Set up all interrupts before sei()!!!
    init_leds();
    init_con_uarts();
    init_counter_1();

    while (1) {
        heartbeat();
        //CLI commands are handled in cli_execute()
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
    }
}


/* Counter 1 ISR */
ISR(TIMER1_COMPA_vect)
{
    counter_1++;
}

