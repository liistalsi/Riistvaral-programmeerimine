#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <assert.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "hmi_msg.h"
#include "print_helper.h"
#include "cli_microrl.h"
#include <time.h>
#include "rfid.h"
#include "../lib/helius_microrl/microrl.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/matejx_avr_lib/mfrc522.h"
#include "../lib/andy_brown_memdebug/memdebug.h"

#define UART_BAUD 9600
#define UART_STATUS_MASK 0x00FF

#define BLINK_DELAY_MS 900
#define LED_RED PORTA0 // Arduino Mega digital pin 22
#define LED_GREEN PORTA2 // Arduino Mega digital pin 24


static inline void init_rfid_reader(void)
{
    MFRC522_init();
    PCD_Init();
}


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
    static time_t prev_time;
    char print_buf[11] = {0x00}; // Buffer lagre enough to hold all long (uint32_t) digits
    time_t now = time(NULL);

    if (prev_time != now) {
        ltoa(now, print_buf, 10); // Convert a long integer to a string.
        uart1_puts_p(PSTR("Uptime: "));
        uart1_puts(print_buf);
        uart1_puts_p(PSTR(" s.\r\n"));
        PORTA ^= _BV(LED_GREEN);
        prev_time = now;
    }
}


time_t door_open = -2;
time_t prev_visit;
char lastuid[20];
char uid_string[20];
bool statusis0 = false;


void update_status(int status, char *name)
{
    if (status == 0) {
        lcd_clrscr();
        lcd_puts(STUDENT_NAME);
        lcd_goto(LCD_ROW_2_START);
        lcd_puts("Closed");
        PORTA &= ~_BV(LED_GREEN);
        statusis0 = true;
    } else if (status == 1) {
        lcd_clrscr();
        lcd_puts(STUDENT_NAME);
        lcd_goto(LCD_ROW_2_START);
        lcd_puts("Open   ");
        lcd_puts(name);
        statusis0 = false;
        PORTA |= _BV(LED_GREEN);
        ;
    } else {
        lcd_clrscr();
        lcd_puts(STUDENT_NAME);
        lcd_goto(LCD_ROW_2_START);
        lcd_puts("Closed AxsDenied");
        PORTA &= ~_BV(LED_GREEN);
        statusis0 = false;
    }
}


void manage_door()
{
    Uid uid;
    Uid *uid_ptr = &uid;

    if (PICC_IsNewCardPresent()) {
        strcpy(uid_string, "");
        PICC_ReadCardSerial(uid_ptr);

        for (byte i = 0; i < uid.size; i++) {
            char suid[20];
            itoa(uid.uidByte[i], suid, 10);
            strcat(uid_string, suid);
        }

        prev_visit = time(NULL);
    }

    if (time(NULL) - prev_visit > 1) {
        strcpy(lastuid, "");
        strcpy(uid_string, "");
    }

    if (strcmp(lastuid, uid_string) != 0) {
        char * name = find(uid_string);

        if (name == NULL) {
            update_status(2, NULL);
            door_open = time(NULL);
        } else {
            update_status(1, name);
            door_open = time(NULL);
        }

        strcpy(lastuid, uid_string);
    }

    if (time(NULL) - door_open >= 2) {
        if (!statusis0) {
            update_status(0, NULL);
        }
    }
}


void main (void)
{
    lcd_init();
    lcd_clrscr();
    lcd_puts_P(STUDENT_NAME);
    sei();
    init_leds();
    init_con_uarts();
    init_counter_1();
    microrl_init(prl, uart0_puts);
    microrl_set_execute_callback(prl, cli_execute);
    init_rfid_reader();

    while (1) {
        heartbeat();
        //CLI commands are handled in cli_execute()
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
        manage_door();
    }
}


/* Counter 1 ISR */
ISR(TIMER1_COMPA_vect)
{
    system_tick();
}

