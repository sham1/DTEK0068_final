/*
 * File:   button-command.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 18 December 2020, 16:34
 */

#include <avr/io.h>
#include <string.h>
#include "button-command.h"
#include "util.h"

static void button_command_init(void);
static bool button_command_execute(char *arglist, const char *arglist_end);
static void button_command_print_help_text(void);

const command button_cmd = {
    .name = "BUTTON",
    .short_help_blurb = "Displays configures the push button",

    .init = &button_command_init,
    .execute = &button_command_execute,
    .print_help_text = &button_command_print_help_text,
};

static void button_command_init(void)
{
    // Alright, first make the button act as an input.
    PORTF.DIRCLR = PIN6_bm;
}

static bool button_command_execute(char *arglist, const char *arglist_end)
{
    char *arg = arglist;
    // If we got arguments, check if it's a INV or PUP command
    if (iterate_args(&arg, &arglist, arglist_end))
    {
        if (!((strcasecmp(arg, "INV") == 0) || (strcasecmp(arg, "PUP") == 0)))
        {
            printf("BUTTON: Unknown argument: %s\r\n", arg);
            return false;
        }

        // Now it has to either be INV or PUP
        bool is_invert = strcasecmp(arg, "INV") == 0;

        arg = arglist;
        if (!iterate_args(&arg, &arglist, arglist_end))
        {
            // In this case, we didn't get a required argument.
            printf("BUTTON: Usage: BUTTON %s [ON|OFF]\r\n",
                   is_invert ? "INV" : "PUP");
            return false;
        }
        // Both INV and PUP share an argument of either "ON" or "OFF".
        if (!((strcasecmp(arg, "ON") == 0) || (strcasecmp(arg, "OFF") == 0)))
        {
            printf("BUTTON: Usage: BUTTON %s [ON|OFF]\r\n",
                   is_invert ? "INV" : "PUP");
            return false;
        }

        bool is_on = strcasecmp(arg, "ON");

        if (is_invert)
        {
            // Set the inverter
            if (is_on)
            {
                PORTF.PIN6CTRL &= ~PORT_INVEN_bm;
            }
            else
            {
                PORTF.PIN6CTRL |= PORT_INVEN_bm;
            }
        }
        else
        {
            // Set the pull-up resistor
            if (is_on)
            {
                PORTF.PIN6CTRL &= ~PORT_PULLUPEN_bm;
            }
            else
            {
                PORTF.PIN6CTRL |= PORT_PULLUPEN_bm;
            }
        }
    }
    else
    {
        bool button = PORTF.IN & PIN6_bm;
        bool invert_on = (PORTF.PIN6CTRL & PORT_INVEN_bm) != 0;
        bool pullup_on = (PORTF.PIN6CTRL & PORT_PULLUPEN_bm) != 0;

        printf("Button logical state: %d\r\n", button ? 1 : 0);
        printf("State invert: %s\r\n", invert_on ? "ON" : "OFF");
        printf("Pull-up resistor: %s\r\n", pullup_on ? "ON" : "OFF");
    }
    return true;
}

static void button_command_print_help_text(void)
{
    printf("\tBUTTON\tPrints the status of the button\r\n");
    printf("\tBUTTON INV [ON|OFF]\tConfigures whether inversion is on\r\n");
    printf("\tBUTTON PUP [ON|OFF]\tConfigures pull-up resistor\r\n");
}