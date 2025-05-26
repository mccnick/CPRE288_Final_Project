/**
 * main.c
 *
 * Nick & Jash - Team A-2
 *
 * Note: TA's Yi Ang and Jack Tonn OK'd cyBot scan library after multiple attempts to fix
 * my servo movement, that was our only blocker, and we are just a team of 2.
 *
 * No GUI either - just using PuTTY data only to read centimeter distances after scans.
 */

#include "timer.h"
#include "lcd.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "movement.h"
#include "uart-interrupt.h"
#include "adc.h"
#include "cyBot_Scan.h"

int direction;
void printStringToPutty(char str[]);
float min(float a, float b);
float max(float a, float b);
int IRreadingToCentimeter(int raw);
int averageIRreading(cyBOT_Scan_t *scan);
int getDistanceAtSpecificAngle(int angle, cyBOT_Scan_t *scan);

int manualDriving(char input, oi_t *sensor_data)
{
    switch (input)
    {
    case 'w':
        uartSendString("\r-----MOVE FORWARD!!!-----");
        moveForwards(sensor_data, 150);
        break;
    case 'a':
        uartSendString("\r-----LEFT TURN!!!-----");
        turnCyBOT(sensor_data, 20);
        break;
    case 's':
        uartSendString("\r-----MOVE BACKWARD!!!-----");
        moveBackwards(sensor_data, 100);
        break;
    case 'd':
        uartSendString("\r-----RIGHT TURN!!!-----");
        turnCyBOT(sensor_data, -20);
        break;
    case NULL:
        oi_setWheels(0, 0);
        break;

    default:
        oi_setWheels(0, 0);
        uartSendString("\r~~~ waiting for input ~~~"); // stop, also the constant PuTTY output after each input
    }
    return direction;
}

typedef struct Object_Struct
{
    float distance; // cm
    int angle; // degrees
    int middle; // middle
    int width; // radial
    int linearWidth; // cm
    int IRminimum; // cm reading
    int IRfirst; // cm reading
    int IRlast; // cm reading
} Object_t;

int main(void)
{
    // -------- INITITALIZATIONS --------
    lcd_init();                     // initialize lcd
    timer_init();                   // initialize timer
    uart_init(115200);              // initialize uart
    uart_interrupt_init();          // initialize uart interrupt
    adc_init();                     // initialize adc

    int angle;
    int start = 0x00000007;
    int objectInView, objectTracker = 0;
    int const maxDist = 75;
    int const maxMarginOfError = 10;
    char sensorData[100];
    char objectData[100];
    float lastDistanceScan = 0;

    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    uart_interrupt_init();
    timer_init();
    cyBOT_init_Scan(start);
    oi_setWheels(0, 0);

    //cyBOT_SERVO_cal();
    right_calibration_value = 274750;
    left_calibration_value = 1230250;

    cyBOT_Scan_t scan;
    cyBOT_Scan(0, &scan);

    // -------- MUSIC - RICK ROLL BABY--------
    unsigned char rickrollNotes[] = { 53, 55, 48, 55, 57, 60, 58, 57, 53, 55, 48 };
    unsigned char rickrollDuration[] = { 48, 64, 16, 48, 48, 8,  8,  8,  48, 64, 64};

    while (1)
    {
        Object_t objects[10];
        char input = uartReceiveNonBlock();
        manualDriving(input, sensor_data);

        if (input == 'm') // scan
        {
            // cyBOT_Scan(0, &scan);
            int minRaw;

            printStringToPutty("\n\r    Angle  Distance(cm) CaliberatedIR");
            for (angle = 0; angle <= 180; angle += 2)
            {
                cyBOT_Scan(angle, &scan);
                int rawCentimeter = IRreadingToCentimeter(averageIRreading(&scan)); // convert

                // if closer and no current objects
                if (rawCentimeter + maxMarginOfError < min(lastDistanceScan, maxDist)
                        && objectInView == 0)
                {
                    uartSendChar('V'); // starting of an object
                    objectInView = 1; // found object

                    objects[objectTracker].angle = angle; // starting angle
                    objects[objectTracker].IRfirst = rawCentimeter; // first IR reading
                    minRaw = rawCentimeter;
                }

                minRaw = min(rawCentimeter, minRaw); // absolute min reading

                // if object being tracked and next reading signficantly different
                if (rawCentimeter > min(lastDistanceScan, maxDist) + maxMarginOfError
                        && angle - objects[objectTracker].angle > 2
                        && objectInView == 1)
                {
                    uartSendChar('^'); // end of an object
                    objectInView = 0; // no object in view anymore

                    objects[objectTracker].IRminimum = minRaw; // min reading
                    objects[objectTracker].IRlast = rawCentimeter; // last distance
                    objects[objectTracker].width = angle - objects[objectTracker].angle; // angular width

                    int middle = (angle + objects[objectTracker].angle) / 2; // middle of the object
                    objects[objectTracker].middle = middle; // storing middle angle in object tracker
                    objects[objectTracker].distance = getDistanceAtSpecificAngle(middle, &scan); // uses getDistanceAtSpecificAngle function

                    // averaging
                    int averageIRreading = (objects[objectTracker].IRminimum
                            + objects[objectTracker].IRfirst) / 2;
                    objects[objectTracker].linearWidth = averageIRreading
                            * sin((objects[objectTracker].width / 2) * M_PI / 180)
                            * 2;

                    objectTracker++; // increment
                }

                // false object filtering
                else if (rawCentimeter
                        > min(lastDistanceScan, maxDist) + maxMarginOfError
                        && angle - objects[objectTracker].angle <= 2
                        && objectInView == 1)
                {
                    objectInView = 0;
                }

                // scan data
                sprintf(sensorData, "\n\r%7d %7.0f %12d", angle,
                        min(scan.sound_dist, maxDist), rawCentimeter);
                printStringToPutty(sensorData);

                lastDistanceScan = rawCentimeter;
            }

            // Checks if an object is still being tracked at the end of the scan and is wide enough (>8 degrees)
            if (objectInView == 1 && angle - objects[objectTracker].angle > 8)
            {
                objectInView = 0;

                objects[objectTracker].IRminimum = minRaw;
                objects[objectTracker].width = angle - objects[objectTracker].angle;

                int middle = (angle + objects[objectTracker].angle) / 2;
                objects[objectTracker].middle = middle;
                objects[objectTracker].distance = getDistanceAtSpecificAngle(middle, &scan); // uses getDistanceAtSpecificAngle

                int averageIRreading = (objects[objectTracker].IRminimum
                        + objects[objectTracker].IRfirst) / 2;
                objects[objectTracker].linearWidth = averageIRreading
                        * sin((objects[objectTracker].width / 2) * M_PI / 180) * 2;
                objectTracker++; // increment
            }

            printStringToPutty(
                    "\n\r Object#  Angle  Distance  Width  LinearWidth"); // header

            int i;
            for (i = 0; i < objectTracker; i++) // iterate current objects
            {
                sprintf(objectData, "\n\r%4d %8d %8.0f %8d %10d", i + 1,
                        objects[i].middle, objects[i].distance,
                        objects[i].width, objects[i].linearWidth);
                printStringToPutty(objectData);
            }

            int j = 0;
            int smallestWidth = 100;
            Object_t thinnestObject; // thin object struct

            // loop thru all objects, and update smallest width object
            for (j = 0; j < objectTracker; j++)
            {
                if (objects[j].linearWidth < smallestWidth)
                {
                    thinnestObject = objects[j];
                    smallestWidth = objects[j].linearWidth;
                }
            }

            cyBOT_Scan(thinnestObject.middle, &scan); // point to middle of thinnest object

            // print thinnest object data to PuTTY
            sprintf(objectData, "\n\rThinnest Object: %4d %8d %8.0f %8d %10d",
                    i + 1, thinnestObject.middle, thinnestObject.distance,
                    thinnestObject.width, thinnestObject.linearWidth);
            printStringToPutty(objectData);

        }

        if (input == 'i') // this is the victory music once we see PLANT SPIKE from movement.c
        {
            lcd_printf("******VICTORY!******");
            oi_loadSong(0, 11, rickrollNotes, rickrollDuration);
            oi_play_song(0);
        }
    }
}

// putty printer
void printStringToPutty(char string[])
{
    int k = 0;
    while (string[k])
    {
        uartSendChar(string[k]);
        ++k;
    }
    uartSendChar('\n');
    uartSendChar('\r');
}

// helper function that returns distance at a specific angle
int getDistanceAtSpecificAngle(int angle, cyBOT_Scan_t *scan)
{
    int total = 0;
    int i = 0;
    cyBOT_Scan(angle - 2, scan);
    timer_waitMillis(1);
    for (i = 0; i < 5; i++)
    {
        cyBOT_Scan(angle - 2, scan); // stabilize
        timer_waitMillis(1);
        char str[20];
        sprintf(str, "\n\r%f", scan->sound_dist);
        printStringToPutty(str); // send dist to putty
        total += scan->sound_dist; // increment current distance

        timer_waitMillis(100);
    }

    return total / 5; // return the average dist
}

// return minimum of two floats
float min(float a, float b)
{
    return (a < b) ? a : b;
}

// return maximum of two floats
float max(float a, float b)
{
    return (a > b) ? a : b;
}

// convert raw IR to cm
int IRreadingToCentimeter(int raw)
{
    return (double) 8000000 * pow((double) raw, -1.709);
}

// average IR readings
int averageIRreading(cyBOT_Scan_t *scan)
{
    int i = 0;
    int totalIR = 0;

    for (i = 0; i < 5; i++)
    {
        totalIR += scan->IR_raw_val;
    }

    return totalIR / 5;
}
