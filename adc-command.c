/*
 * File:   adc-command.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 18 December 2020, 10:26
 */

#include "adc-command.h"
#include "util.h"
#include <avr/io.h>
#include <string.h>

static void adc_command_init(void);
static bool adc_command_execute(char *arglist, const char *arglist_end);
static void adc_command_print_help_text(void);

const command adc_cmd = {
    .name = "ADC",
    .short_help_blurb = "Reads an analog voltage as digital value",

    .init = &adc_command_init,
    .execute = &adc_command_execute,
    .print_help_text = &adc_command_print_help_text,
};

#define A(n) { .name = "A"#n, .value = ADC_MUXPOS_AIN ## n ## _gc, }
static struct
{
    const char *name;
    ADC_MUXPOS_t value;
} const set_args[] = {
    A(0),
    A(1),
    A(2),
    A(3),
    A(4),
    A(5),
    A(6),
    A(7),
    A(8),
    A(9),
    A(10),
    A(11),
    A(12),
    A(13),
    A(14),
    A(15),
};
#undef A

static void adc_command_init(void)
{
    // Make CLK_PER divided by four and use internal voltage reference
    ADC0.CTRLC = ADC_PRESC_DIV4_gc | ADC_REFSEL_INTREF_gc;
    // Enable ADC and set the 10-bit mode
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;
    
    // And set the channel.
    ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
}

static bool adc_command_execute(char *arglist, const char *arglist_end)
{
    char *arg = arglist;
    // If we got arguments, check if it's a SET command
    if (iterate_args(&arg, &arglist, arglist_end))
    {
        if (strcasecmp(arg, "SET") != 0) {
            printf("ADC: Unknown argument: %s\r\n", arg);
            return false;
        }
        
        arg = arglist;
        // Now check whether we have a required parameter...
        if (!iterate_args(&arg, &arglist, arglist_end))
        {
            printf("ADC: Usage: ADC SET A<n> (0 <= n <= 15)\r\n");
            return false;
        }
        // ...and if we do, check its validity.
        bool found_port = false;
        ADC_MUXPOS_t channel;
        for (size_t i = 0; i < ARRAY_LEN(set_args); ++i)
        {
            if (strcasecmp(set_args[i].name, arg) == 0)
            {
                found_port = true;
                channel = set_args[i].value;
                break;
            }
        }
        
        if (!found_port)
        {
            printf("ADC: Usage: ADC SET A<n> (0 <= n <= 15)\r\n");
            return false;
        }
        
        // Input channel found, now just set it.
        ADC0.MUXPOS = channel;
    }
    else
    {
        // Otherwise, just read the current channel and print the value
        // First, start conversion
        ADC0.COMMAND = ADC_STCONV_bm;
        // Wait until it's done
        while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
        // Clear the interrupt flag
        ADC0.INTFLAGS = ADC_RESRDY_bm;

        // And print the value
        printf("ADC value: %"PRIu16"\r\n", ADC0.RES);
    }
    return true;
}

static void adc_command_print_help_text(void)
{
    printf("\tADC\tPrints the value currently being read\r\n");
    printf("\tADC SET A<n>\tSets the input channel (0 <= n <= 15)\r\n");
}