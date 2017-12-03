#include <avr/io.h>
#include <stdio.h>
#include "print_helper.h"

/* Method to print ascii table*/
int print_ascii_tbl (FILE *stream)
{
    for (char c = ' '; c <= '~'; c++) {
        if (!fprintf(stream, "%c ", c)) {
            return 0;
        }
    }

    return fprintf(stream, "\n");
}

/* Method to print human table and what isn't in asci table, will be printed in hex code*/
int print_for_human (FILE *stream, const unsigned char *array,
                     const size_t len)
{
    for (unsigned int i = 0; i < len; i++) {
        if (array[i] >= ' ' && array[i] <= '~') {
            if (!fprintf(stream, "%c", array[i])) {
                return 0;
            }
        } else {
            if (!fprintf(stream, "\"0x%02X\"", array[i])) {
                return 0;
            }
        }
    }

    return fprintf(stream, "\n");
}
