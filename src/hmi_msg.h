#include <avr/pgmspace.h>
#ifndef _HMI_MSG_H
#define _HMI_MSG_H

#define VER_FW "Version:  %S built on: %S %S\n"
#define VER_LIBC "avr-libc version: %s avr-gcc version: %S\n"
#define ENTER_NUMBER "Enter number >"
#define REPLY_NUMBER "You entered number %S\n"
#define WRONG_NUMBER "Please enter number between 0 and 9!\n"
#define NOT_NUMBER "Please enter decimal number\n"
#define STUDENT_NAME "Liis Talsi"

const char n1[] PROGMEM = "Zero";
const char n2[] PROGMEM = "One";
const char n3[] PROGMEM = "Two";
const char n4[] PROGMEM = "Three";
const char n5[] PROGMEM = "Four";
const char n6[] PROGMEM = "Five";
const char n7[] PROGMEM = "Six";
const char n8[] PROGMEM = "Seven";
const char n9[] PROGMEM = "Eight";
const char n10[] PROGMEM = "Nine";

PGM_P const numbers[] PROGMEM = {n1, n2, n3, n4, n5, n6, n7, n8, n9, n10};
#endif /* header guard end*/
