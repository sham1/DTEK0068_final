/*
 * File:   led-command.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 18 December 2020, 18:12
 */

#include "led-command.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "util.h"

#include <inttypes.h>

static void led_command_init(void);
static bool led_command_execute(char *arglist, const char *arglist_end);
static void led_command_print_help_text(void);

command led_cmd = {
    .name = "LED",
    .short_help_blurb = "Displays and configures the LED",

    .init = &led_command_init,
    .execute = &led_command_execute,
    .print_help_text = &led_command_print_help_text,
};

static void init_timer(void);

static volatile bool is_blinking = false;
static volatile uint8_t duty_on = 0;

static void led_command_init(void)
{
    // Initialise the timer for the PWM action.
    init_timer();
    // Set the LED as an output.
    PORTF.DIRSET = PIN5_bm;
    // and turn it off by default.
    PORTF.OUTSET = PIN5_bm;
}

static void set_led(bool on);

static bool led_command_execute(char *arglist, const char *arglist_end)
{
    char *arg = arglist;
    // If we got arguments, check if it's ON, OFF, or SET
    if (iterate_args(&arg, &arglist, arglist_end))
    {
        if ((strcasecmp(arg, "ON") == 0) || (strcasecmp(arg, "OFF") == 0))
        {
            set_led(strcasecmp(arg, "ON") == 0);
            return true;
        }
        else if (strcasecmp(arg, "SET") == 0)
        {
            arg = arglist;
            // Check if we have any argument...
            if (!iterate_args(&arg, &arglist, arglist_end))
            {
                printf("LED: Usage: LED SET <n> (0 <= n <= 255)\r\n");
                return false;
            }
            // ...and check that it's a number between 0 and 255.
            // We have to clear out `errno` so we can tell whether the
            // conversion is valid.
            errno = 0;
            char *end_ptr = NULL;
            int32_t converted = strtol(arg, &end_ptr, 10);
            if (converted == 0)
            {
                // We could either have our zero, or an error.
                // Let's rule out the errors.
                if (errno != 0)
                {
                    printf("LED: Usage: LED SET <n> (0 <= n <= 255)\r\n");
                    return false;
                }
                if (end_ptr == arg)
                {
                    printf("LED: Usage: LED SET <n> (0 <= n <= 255)\r\n");
                    return false;
                }

                // At this point we know that our output was zero, but it also
                // was what we expected.
            }

            // Now we check the range.
            if (converted < 0)
            {
                printf("LED: Usage: LED SET <n> (0 <= n <= 255)\r\n");
                return false;
            }
            if (converted > 255)
            {
                printf("LED: Usage: LED SET <n> (0 <= n <= 255)\r\n");
                return false;
            }

            // And now we know that our argument can be used as a duty cycle
            // value.

            // Temporarily disable blink for the purpose of setting the new
            // duty cycle.
            is_blinking = false;

            duty_on = (uint8_t) converted;

            is_blinking = true;

            return true;
        }
        else
        {
            printf("LED: Unknown argument: %s\r\n", arg);
        }
    }
    else
    {
    }
    return true;
}

static void led_command_print_help_text(void)
{
    printf("\tLED\tQuery LED brightness and ON/OFF state\r\n");
    printf("\tLED [ON|OFF]\tTurn the LED on or off\r\n");
    printf("\tLED SET <n>\tSet LED brightness (0 <= n <= 255)\r\n");
}

static void set_led(bool on)
{
    is_blinking = false;
    if (on)
    {
        PORTF.OUTCLR = PIN5_bm;
    }
    else
    {
        PORTF.OUTSET = PIN5_bm;
    }
}

static void init_timer(void)
{
    // Enable overflow interrupt for timer
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
    // Set as normal mode
    TCA0.SINGLE.EVCTRL &= ~TCA_SINGLE_CNTEI_bm;

    // Disable event counting
    TCA0.SINGLE.PER = 0x0CB6;

    // Enable timer and set clock source to be system/256
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
}

ISR(TCA0_OVF_vect)
{
    static uint8_t counter = 0;
    // If we are to blink, let's blink
    if (is_blinking)
    {
        // If we're on the "on" period, turn on the LED.
        if (counter < duty_on)
        {
            PORTF.OUTCLR = PIN5_bm;
        }
        else
        {
            // Otherwise, turn it off.
            PORTF.OUTSET = PIN5_bm;
        }
        // Either way, increment the counter.
        // It will just work since with unsigned ints, the standard
        // says that no overflow is possible and everything wraps around
        // neatly!
        ++counter;
    }

    // Clear interrupt flag
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}