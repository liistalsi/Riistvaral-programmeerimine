#include <stdio.h>
#include <avr/io.h>
#include "print_helper.h"

#define ASCII_COUNT 128

/* Method to print ascii table*/
int print_ascii_tbl (FILE *stream)
{
    for (char c = ' '; c <= '~'; c++) {
        if (!fprintf_P(stream, "%c ", c)) {
            return 0;
        }
    }

    return fprintf_P(stream, "\n");
}

/* Method to print human table and what isn't in asci table, will be printed in hex code*/
int print_for_human (FILE *stream)
{
    for (unsigned char i = 0; i < ASCII_COUNT; ++i) {
        if (i >= ' ' && i <= '~') {
            if (!fprintf_P(stream, "%c", i)) {
                return 0;
            }
        } else {
            if (!fprintf_P(stream, "\"0x%02X\"", i)) {
                return 0;
            }
        }
    }

    return fprintf_P(stream, "\n");
}
