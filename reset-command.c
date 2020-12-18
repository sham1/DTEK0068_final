/* 
 * File:   reset-command.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 17 December 2020, 22:58
 */

#include "reset-command.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>

void reset_command_init(void);
bool reset_command_execute(char *arglist);
void reset_command_print_help_text(void);

command reset_cmd = {
    .name = "RESET",
    .short_help_blurb = "Reset the microcontroller",

    .init = &reset_command_init,
    .execute = &reset_command_execute,
    .print_help_text = &reset_command_print_help_text,
};

// Disable watchdog at the start
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void wdt_init(void)
{
    // Just so we don't get reset by accident by the watchdog.
    wdt_disable();
}

void reset_command_init(void)
{
    // All the necessary stuff is done in wdt_init.
}

bool reset_command_execute(char *arglist)
{
    (void) arglist;

    // TODO: Figure out why this isn't resetting!!
    wdt_enable(WDTO_15MS);

    return true;
}
void reset_command_print_help_text(void)
{
    printf("\tRESET\tResets this microcontroller\r\n");
}