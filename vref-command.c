/* 
 * File:   vref-command.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 18 December 2020, 09:59
 */

#include "vref-command.h"
#include "util.h"
#include <string.h>
#include <avr/io.h>
#include <inttypes.h>

static void vref_command_init(void);
static bool vref_command_execute(char *arglist, const char *arglist_end);
static void vref_command_print_help_text(void);

const command vref_cmd = {
    .name = "VREF",
    .short_help_blurb = "Displays and sets the reference voltage",

    .init = &vref_command_init,
    .execute = &vref_command_execute,
    .print_help_text = &vref_command_print_help_text,
};

#define A(major, minor) { .name = #major "V" #minor,\
    .value = VREF_ADC0REFSEL_ ## major ## V ## minor ## _gc, }
static struct
{
    const char *name;
    VREF_ADC0REFSEL_t value;
} const set_args[] = {
    A(0,55),
    A(1,1),
    A(1,5),
    A(2,5),
    A(4,34),
};
#undef A

static void vref_command_init(void)
{
    // Set some reasonable reference voltage
    VREF.CTRLA |= VREF_ADC0REFSEL_0V55_gc;
}

static bool vref_command_execute(char *arglist, const char *arglist_end)
{
    char *arg = arglist;
    // If we got arguments, check if it's a SET command
    if (iterate_args(&arg, &arglist, arglist_end))
    {
        if (strcasecmp(arg, "SET") != 0) {
            printf("VREF: Unknown argument: %s\r\n", arg);
            return false;
        }
        
        arg = arglist;
        // Now check whether we have a required parameter...
        if (!iterate_args(&arg, &arglist, arglist_end))
        {
            printf("VREF: Usage: VREF SET %s\r\n",
                    "[0V55|1V1|1V5|2V5|4V34]");
            return false;
        }
        // ...and if we do, check its validity.
        bool found_voltage = false;
        VREF_ADC0REFSEL_t voltage;
        for (size_t i = 0; i < ARRAY_LEN(set_args); ++i)
        {
            if (strcasecmp(set_args[i].name, arg) == 0)
            {
                found_voltage = true;
                voltage = set_args[i].value;
                break;
            }
        }
        
        if (!found_voltage)
        {
            printf("VREF: Usage: VREF SET %s\r\n",
                    "[0V55|1V1|1V5|2V5|4V34]");
            return false;
        }
        
        VREF.CTRLA = voltage;        
    }
    else
    {
        uint8_t voltage = VREF.CTRLA;
        // 3 bits set at the proper position, i.e.
        // 0b01110000
        const uint8_t voltage_mask = 7 << 4;
        // Otherwise, just print the selected reference voltage.
        for (size_t i = 0; i < ARRAY_LEN(set_args); ++i)
        {
            if ((voltage & voltage_mask) == set_args[i].value)
            {
                printf("Current reference voltage: %s\r\n", set_args[i].name);
                break;
            }
        }
    }
    return true;
}

static void vref_command_print_help_text(void)
{
    printf("\tVREF\tPrints the selected reference voltage\r\n");
    printf("\tVREF SET %s\tSets the reference voltage\r\n",
            "[0V55|1V1|1V5|2V5|4V34]");
}