#ifndef HMI_MSG_H_
#define HMI_MSG_H_

#define VER_FW "Version: " FW_VERSION " built on: " __DATE__ " " __TIME__ "\r\n"
#define VER_LIBC "avr-libc version: " __AVR_LIBC_VERSION_STRING__ " avr-gcc version: " __VERSION__ "\r\n"

extern const char START_NOTE[];
extern const char ENTER_NUMBER[];
extern const char REPLY_NUMBER[];
extern const char WRONG_NUMBER[];
extern const char NOT_NUMBER[];
extern const char STUDENT_NAME[];
extern PGM_P const numbers[];

#endif /* header guard end*/
