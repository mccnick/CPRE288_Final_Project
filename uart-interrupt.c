/*
 *
 *   uart-interrupt.c
 *
 *   Nick & Jash - Team A-2
 */

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "uart-interrupt.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include "Timer.h"
#define REPLACE_ME 0x00

volatile char uart_data;
volatile char flag;

void uart_init(int baud)
{
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    SYSCTL_RCGCUART_R |= 0b00000010;
    GPIO_PORTB_AFSEL_R = 0b00000011;
    GPIO_PORTB_PCTL_R = 0x00000011;
    GPIO_PORTB_DEN_R = 0b00000011;
    GPIO_PORTB_DIR_R = 0b00000011;

    int ibrd;
    double fbrd;

    fbrd = (float) 16000000 / (16.0 * (float) baud);
    ibrd = (int) (fbrd);
    fbrd = (int) ((fbrd - ibrd) * 64 + 0.5);

    UART1_CTL_R &= 0xFFFE;
    ;
    UART1_IBRD_R = ibrd;
    UART1_FBRD_R = (int) fbrd;
    UART1_LCRH_R = 0b01100000;
    UART1_CC_R = 0x0;
    UART1_CTL_R |= 0x301;

}

void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0b00010000;

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0x00000040;

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interrupt_handler);

}

void uart_interrupt_handler()
{
    if (UART1_MIS_R & 0x10)
    {
        uart_data = uartReceive();
        flag = 1;
        UART1_ICR_R = 0b00010000;
    }
}

char uartReceiveNonBlock(void)
{
    char data = 0;
    if ((UART1_FR_R & UART_FR_RXFE))
    {
        data = (char) (UART1_DR_R & 0xFF);
    }
    return data;
}

char uartReceive(void)
{
    char receiveData = 0;

    while (UART1_FR_R & 0x10)
    {

    }
    receiveData = (char) (UART1_DR_R & 0xFF);
    return receiveData;
}

void uartSendChar(char uartData)
{
    while (UART1_FR_R & 0x20)
    {

    }
    UART1_DR_R = uartData;
}

void uartSendString(const char *data)
{
    int i = 0;
    while (data[i] != '\0')
    {
        uartSendChar(data[i]);
        i++;
    }
}
