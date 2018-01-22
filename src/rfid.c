#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/helius_microrl/microrl.h"
#include "../lib/matejx_avr_lib/mfrc522.h"
#include "../lib/andy_brown_memdebug/memdebug.h"
#include "hmi_msg.h"
#include "cli_microrl.h"
#include "print_helper.h"



typedef struct card {
    char *uid;
    char *name;
    short size;
    struct card * next;
} card_temp;


card_temp * linkedlist = NULL;

//Reading card
void rfid_read(void)
{
    Uid uid;
    Uid *uid_ptr = &uid;

    if (PICC_IsNewCardPresent()) {
        uart0_puts("Card selected!\n\r");
        PICC_ReadCardSerial(uid_ptr);
        char ssize[2];
        itoa(uid.size, ssize, 10);
        uart0_puts("\n\r");
        uart0_puts("Card UID:");
        char uid_string[100] = "\0";

        for (byte i = 0; i < uid.size; i++) {
            char suid[20];
            itoa(uid.uidByte[i], suid, 10);
            strcat(uid_string, suid);
        }

        uart0_puts(uid_string);
        uart0_puts(" (");
        uart0_puts(ssize);
        uart0_puts(" Bytes)\r\n");
        uart0_puts("Card Type: ");
        uart0_puts(PICC_GetTypeName(PICC_GetType(uid.sak)));
        uart0_puts("\r\n");
    } else {
        uart0_puts("Unable to select card.\r\n");
    }
}

//Adding new card to the "safe-list"
void rfid_add(const char *const *argv)
{
    if (linkedlist == NULL) {
        linkedlist = malloc(sizeof(card_temp));

        if (linkedlist == NULL) {
            uart0_puts("\r\nNot enough memory to add a new card :(\r\n");
            return;
        }

        linkedlist->uid = malloc(strlen(argv[1]) + 1);

        if (linkedlist->uid == NULL) {
            uart0_puts("\r\nNot enough memory to add a new card :(\r\n");
            free(linkedlist);
            return;
        }

        strcpy(linkedlist->uid, argv[1]);
        linkedlist->name = malloc(strlen(argv[2]) + 1);

        if (linkedlist->name == NULL) {
            uart0_puts("\r\nNot enough memory to add a new card :(\r\n");
            free(linkedlist->uid);
            free(linkedlist);
            return;
        }

        strcpy(linkedlist->name, argv[2]);
        linkedlist->size = strlen(argv[1]);
        linkedlist->next = NULL;
        uart0_puts("\r\nAdded card UID ");
        uart0_puts(linkedlist->uid);
        uart0_puts(" for user ");
        uart0_puts(linkedlist->name);
        uart0_puts("\r\n");
        return;
    }

    card_temp * card_present = linkedlist;

    while (card_present != NULL) {
        if (strcmp(card_present->uid, argv[1]) == 0) {
            uart0_puts("\r\nCan not add card. Card already in use by user ");
            uart0_puts(card_present->name);
            uart0_puts("\r\n");
            return;
        }

        card_present = card_present->next;
    }

    card_present = linkedlist;

    while (card_present ->next != NULL) {
        card_present = card_present->next;
    }

    card_present->next = malloc(sizeof(card_temp));
    card_present = card_present->next;

    if (card_present == NULL) {
        uart0_puts("Not enough memory to add a new card :(\r\n");
        return;
    }

    card_present->uid = malloc(strlen(argv[1]) + 1);

    if (card_present->uid == NULL) {
        uart0_puts("\r\nNot enough memory to add a new card :(\r\n");
        free(card_present);
        return;
    }

    strcpy(card_present->uid, argv[1]);
    card_present->name = malloc(strlen(argv[2]) + 1);

    if (card_present->name == NULL) {
        uart0_puts("\r\nNot enough memory to add a new card :(\r\n");
        free(card_present->uid);
        free(card_present);
        return;
    }

    strcpy(card_present->name, argv[2]);
    card_present->size = strlen(argv[1]);
    card_present->next = NULL;
    uart0_puts("\r\nAdded UID ");
    uart0_puts(card_present->uid);
    uart0_puts(" for user ");
    uart0_puts(card_present->name);
    uart0_puts("\r\n");
}

//Removing card from the "safe_list"
void rfid_remove(const char *const *argv)
{
    card_temp * card_present = linkedlist;
    card_temp * temp;

    if (strcmp(linkedlist->uid, argv[1]) == 0) {
        temp = linkedlist->next;
        free(linkedlist);
        linkedlist = temp;
    }

    while (card_present != NULL) {
        if (strcmp(card_present->next->uid, argv[1]) == 0) {
            temp = card_present->next->next;
            free(card_present->next->uid);
            free(card_present->next->name);
            free(card_present->next);
            card_present->next = temp;
        }

        card_present = card_present->next;
    }
}

//Check if any cards have been added. If yes then print out the list of "safe" cards
void rfid_print_list()
{
    if (linkedlist == NULL) {
        uart0_puts("\r\nNo cards have been added\r\n");
        return;
    }

    card_temp * card_present = linkedlist;
    int count = 0;
    char counts[10];
    uart0_puts("\r\n");

    while (card_present != NULL) {
        count++;
        itoa(count, counts, 10);
        uart0_puts(counts);
        uart0_puts(". User: ");
        uart0_puts(card_present->name);
        uart0_puts(" UID: ");
        uart0_puts(card_present->uid);
        uart0_puts(" (");
        char sizes[10];
        itoa(card_present->size, sizes, 10);
        uart0_puts(sizes);
        uart0_puts(" Bytes)\r\n");
        card_present = card_present->next;
    }

    uart0_puts("\r\n");
}

//function for going through the list of safe-cards to use in the door management
char *find(char* search)
{
    card_temp * card_present = linkedlist;

    while (card_present != NULL) {
        if (strcmp(card_present->uid, search) == 0) {
            return card_present->name;
        }

        card_present = card_present->next;
    }

    return NULL;
}
