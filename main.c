/*
 * File:   main.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 17 December 2020, 13:21
 */

#define F_CPU 3333333
#define BAUD_RATE(bd) ((float)(F_CPU * 64 / (16 * (float)(bd))) + 0.5)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <avr/sleep.h>
#include <stdbool.h>

#include "command.h"
#include "util.h"

// A 1 KiB buffer ought to be enough space for all the command needs.
static char command_buffer[1024] = {'\0'};
static char *command_buffer_end = &command_buffer[0];

// A ring-buffer for the incoming characters.
static volatile char input_buffer[1024] = {'\0'};
static volatile size_t input_buffer_start = 0;
static volatile size_t input_buffer_end = 0;

static volatile bool has_command_ready = false;

static void usart0_init(void);
static int usart0_print_char(char c, FILE *stream);
static char usart0_read_char(void);

static void print_prompt(void);

static void process_keys(void);

static void init_commands(void);

int main(void)
{
    usart0_init();
    init_commands();
    sei();
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    while (1)
    {
        print_prompt();
        sleep_mode();
        process_keys();

        if (has_command_ready)
        {
            printf("\r\n");
            has_command_ready = false;

            for (char *arg = command_buffer, *next = NULL; iterate_args(&arg,
                    &next, command_buffer_end);)
            {
                // TODO: Move to proper command.
                if (strcasecmp(arg, "HELP") == 0)
                {
                    printf("Available commands:\r\n");
                    for (command **cmd = commands; *cmd != NULL; ++cmd)
                    {
                        printf("\t%s\t%s\r\n", (*cmd)->name,
                                (*cmd)->short_help_blurb);
                    }
                    break;
                } else {
                    for (command **cmd = commands; *cmd != NULL; ++cmd)
                    {
                        if (command_match_name(*cmd, arg))
                        {
                            (*cmd)->execute(next, command_buffer_end);
                            break;
                        }
                    }
                }
                break;
            }

            command_buffer_end = &command_buffer[0];
            *command_buffer_end = '\0';
        }
    }
}

static void init_commands(void)
{
    for (command **cmd = commands; *cmd != NULL; ++cmd)
    {
        (*cmd)->init();
    }
}

// Make sure that `printf` and friends can be used.
static FILE usart0_stream = FDEV_SETUP_STREAM(usart0_print_char,
        NULL, _FDEV_SETUP_WRITE);

static void usart0_init(void)
{
    // Set pin 1 as receive and pin 0 as send
    PORTA.DIRCLR = PIN1_bm;
    PORTA.DIRSET = PIN0_bm;
    
    USART0_BAUD = (uint16_t)BAUD_RATE(9600);
    
    // Enable receiving and sending
    USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
    // Also enable interrupts
    USART0.CTRLA |= USART_RXCIE_bm;
    
    // And set the standard out appropriately so `printf` can be used.
    stdout = &usart0_stream;
}

static int usart0_print_char(char c, FILE *stream)
{
    (void) stream;
    // Wait until we can send the char...
    while (!(USART0.STATUS & USART_DREIF_bm));
    // ...and send it.
    USART0.TXDATAL = c;
    
    return 0;
}

static char usart0_read_char(void)
{
    // Wait until we can read the char...
    while (!(USART0.STATUS & USART_DREIF_bm));
    // ..and return it.
    return USART0.RXDATAL;
}

static void print_prompt(void)
{
    printf("\r> ");
    printf("%s", command_buffer);
}

static void process_keys(void)
{
    while (input_buffer_start < input_buffer_end)
    {
        char c = input_buffer[input_buffer_start];

        switch (c)
        {
        // Backspace
        case 0x7F:
            // Check whether we are already at the beginning of the command
            // buffer so we don't affect things before it.
            if (command_buffer_end == command_buffer)
            {
                break;
            }

            // Otherwise, back up with the end of the command,
            // land remove character.
            putchar(c);
            --command_buffer_end;
            *command_buffer_end = '\0';
            break;
        // Newline
        case 0x0D:
            has_command_ready = true;
            break;
        default:
            // Add character to buffer, and terminate current command string.
            *command_buffer_end = c;
            ++command_buffer_end;
            *command_buffer_end = '\0';
            break;
        }

        input_buffer_start = (input_buffer_start + 1) % 1024;
    }
}

ISR(USART0_RXC_vect)
{
    char c = usart0_read_char();
    // Store the newest character.
    input_buffer[input_buffer_end] = c;
    input_buffer_end = (input_buffer_end + 1) % 1024;
}
