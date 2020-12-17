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

// A 1 KiB buffer ought to be enough space for all the command needs.
static volatile char command_buffer[1024] = {'\0'};

static void usart0_init(void);
static int usart0_print_char(char c, FILE *stream);
static char usart0_read_char(void);

int main(void)
{
    usart0_init();
    printf("Hello, %s\r\n", "World!");
    
    sei();
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    while (1)
    {
        sleep_mode();
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

ISR(USART0_RXC_vect)
{
    char c = usart0_read_char();
    printf("Got character: %d!\r\n", c);
}