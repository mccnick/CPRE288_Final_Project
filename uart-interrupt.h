/*
 *
 *   uart-interrupt.h
 *
 *   Used to set up the RS232 connector and WIFI module
 *   Uses RX interrupt
 *   Functions for communicating between CyBot and PC via UART1
 *   Serial parameters: Baud = 115200, 8 data bits, 1 stop bit,
 *   no parity, no flow control on COM1, FIFOs disabled on UART1
 *
 *   @author Dane Larson
 *   @date 07/18/2016
 *   Phillip Jones updated 9/2019, removed WiFi.h, Timer.h
 *   Diane Rover updated 2/2020, added interrupt code
 */

#ifndef UART_H_
#define UART_H_

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/interrupt.h"

// Notice that interrupt.h provides library function prototypes for IntMasterEnable() and IntRegister()

// These two varbles have been declared
// in the file containing main
extern volatile char uart_data; // Your UART interupt code can place read data here
extern volatile char flag;       // Your UART interupt can update this flag
// to indicate that it has placed new data
// in uart_data

void uart_init(int baud);
void uart_interrupt_init();
void uart_interrupt_handler();
char uartReceiveNonBlock(void);
char uartReceive(void);
void uartSendChar(char data);
void uartSendString(const char *data);
#endif /* UART_H_ */
