#include <avr/pgmspace.h>
#ifndef HMI_MSG /* prevent the header file to be included more than once, see "wrapper #ifndef" */
#define HMI_MSG

const char START_NOTE[] PROGMEM = "Program started";
const char ENTER_NUMBER[] PROGMEM = "Enter number >";
const char REPLY_NUMBER[] PROGMEM = "You entered number ";
const char WRONG_NUMBER[] PROGMEM = "Please enter number between 0 and 9!";
const char NOT_NUMBER[] PROGMEM = "Please enter decimal number";
const char STUDENT_NAME[] PROGMEM = "Liis Talsi";

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
