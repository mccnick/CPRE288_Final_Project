/**
 * movement.c
 *
 * Nick & Jash - Team A-2
 */

#include "open_interface.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>
#include "lcd.h"
#include "uart-interrupt.h"

void moveForwards(oi_t *sensor, int centemeters);
void turnCyBOT(oi_t *sensor, int degrees);
void moveBackwards(oi_t *sensor, int millimiters);
void uart_sendCharNonBlocking(char data);
