/*
 * File:   temp-command.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 18 December 2020, 13:43
 */

#include "temp-command.h"
#include <string.h>
#include "util.h"
#include <avr/io.h>
#include <inttypes.h>

static void temp_command_init(void);
static bool temp_command_execute(char *arglist, const char *arglist_end);
static void temp_command_print_help_text(void);

command temp_cmd = {
    .name = "TEMP",
    .short_help_blurb = "Displays the internal temperature",

    .init = &temp_command_init,
    .execute = &temp_command_execute,
    .print_help_text = &temp_command_print_help_text,
};

static void temp_command_init(void)
{
}

static bool temp_command_execute(char *arglist, const char *arglist_end)
{
    char *arg = arglist;
    // If we got arguments, bail since this command takes no args.
    if (iterate_args(&arg, &arglist, arglist_end))
    {
        printf("TEMP: Unknown argument: %s\r\n", arg);
        return false;
    }
    else
    {
        // Save all vrefs and such temporarily.
        uint8_t temp_voltage = VREF.CTRLA;
        uint8_t temp_adc0c = ADC0.CTRLC;
        uint8_t temp_muxpos = ADC0.MUXPOS;
        uint8_t temp_adc0d = ADC0.CTRLD;
        uint8_t temp_sample = ADC0.SAMPCTRL;

        // And set relevant values for temperature measurement
        VREF.CTRLA = VREF_ADC0REFSEL_1V1_gc | ADC_RESSEL_10BIT_gc;
        ADC0.CTRLC = ADC_REFSEL_INTREF_gc | (1 << ADC_SAMPCAP_bp)
                | ADC_PRESC_DIV4_gc;
        ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
        ADC0.CTRLD = ADC_INITDLY_DLY64_gc;
        ADC0.SAMPCTRL = ADC_SAMPNUM_ACC64_gc;

        int8_t sigrow_offset = SIGROW.TEMPSENSE1;
        uint8_t sigrow_gain = SIGROW.TEMPSENSE0;

        // Now the ADC is set so it can read the temperature.
        // Read it!
        ADC0.COMMAND = ADC_STCONV_bm;
        // Wait until it's done
        while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
        // And now read the result. Also clears interrupt flag
        uint16_t result = ADC0.RES;
        // First turn it into Kelvin.
        int32_t temp = result - sigrow_offset;
        temp *= sigrow_gain;
        temp += 0x0080; // Round to degree
        temp >>= 8; // And now it's Kelvin!

        int32_t celsius = temp - 273;

        // Restore previous values.
        ADC0.SAMPCTRL = temp_sample;
        ADC0.CTRLD = temp_adc0d;
        ADC0.MUXPOS = temp_muxpos;
        ADC0.CTRLC = temp_adc0c;
        VREF.CTRLA = temp_voltage;

        printf("Internal temperature is %"PRId32" degrees Celsius\r\n",
                celsius);

        return true;
    }
}

static void temp_command_print_help_text(void)
{
    printf("\tTEMP\tPrints the internal temperature in degrees Celsius\r\n");
}