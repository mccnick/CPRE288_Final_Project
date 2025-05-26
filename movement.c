/**
 * movement.c
 *
 * Nick & Jash - Team A-2
 */

#include "movement.h"
#include "open_interface.h"
#include "music.h"

/*
 * MOVE FORWARD
 */
void moveForwards(oi_t *sensor, int millimiters)
{
    oi_setWheels(192, 195); // ---------- original movement speed
    // oi_setWheels(102, 105); // ---------- slower movement speed, drifts not good
    oi_update(sensor);
    if (sensor->bumpLeft)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop because bumped
        oi_update(sensor);
        uartSendString("\n\r-----BUMP FRONT LEFT!!!-----\n\r");
        timer_waitMillis(3000);
    }

    else if (sensor->bumpRight)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop because bumped
        oi_update(sensor);
        uartSendString("\n\r-----BUMP FRONT RIGHT!!!-----\n\r");
        timer_waitMillis(3000);
    }
    else if (sensor->cliffRight)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop because GAME OVER (bot reached planting site)
        oi_update(sensor);
        uartSendString(
                "\n\r-----PLANT SPIKE, GAME OVER!!! PLAY VICTORY MUSIC!!!-----\n\r");
        timer_waitMillis(4000);
    }
    else if (sensor->cliffLeft)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop because GAME OVER (bot reached planting site)
        oi_update(sensor);
        uartSendString(
                "\n\r-----PLANT SPIKE, GAME OVER!!! PLAY VICTORY MUSIC!!!-----\n\r");
        timer_waitMillis(4000);
    }
    else if (sensor->cliffFrontRight)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop because GAME OVER (bot reached planting site)
        oi_update(sensor);
        uartSendString(
                "\n\r-----PLANT SPIKE, GAME OVER!!! PLAY VICTORY MUSIC!!!-----\n\r");
        timer_waitMillis(4000);
    }
    else if (sensor->cliffFrontLeft)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop because GAME OVER (bot reached planting site)
        oi_update(sensor);
        uartSendString(
                "\n\r-----PLANT SPIKE, GAME OVER!!! PLAY VICTORY MUSIC!!!-----\n\r");
        timer_waitMillis(4000);
    }

    // ------------ OUT OF BOUNDS (WHITE TAPE) LOGIC ------------
    else if (sensor->cliffFrontRightSignal >= 2720
            && sensor->cliffFrontRightSignal <= 2770)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop the bot
        oi_update(sensor);
        uartSendString("\n\r-----OUT OF BOUNDS!!!-----\n\r");

        timer_waitMillis(4000);
    }
    else if (sensor->cliffRightSignal >= 2580
            && sensor->cliffRightSignal <= 2630)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop the bot
        oi_update(sensor);
        uartSendString("\n\r-----OUT OF BOUNDS!!!-----\n\r");

        timer_waitMillis(4000);
    }
    else if (sensor->cliffLeftSignal >= 2580 && sensor->cliffLeftSignal <= 2630)
    {
        oi_update(sensor);
        oi_setWheels(0, 0); // stop the bot
        oi_update(sensor);
        uartSendString("\n\r-----OUT OF BOUNDS!!!-----\n\r");
        timer_waitMillis(4000);
    }
}

/**
 * MOVE BACKWARDS
 */
void moveBackwards(oi_t *sensor, int millimiters)
{
    oi_setWheels(-200, -201); // move back
    // oi_setWheels(-35, -35);
}

/**
 * TURN THE CyBOT (NON-STOP) LEFT OR RIGHT
 */
void turnCyBOT(oi_t *sensor, int degrees)
{
    double iterativeSum = 0;
    if (degrees < 0)
    {
        oi_setWheels(-180, 180); // ----- Clockwise / RIGHT
        while (iterativeSum > degrees)
        {
            oi_update(sensor);

            iterativeSum += sensor->angle;
        }
    }
    else if (degrees > 0)
    {
        oi_setWheels(180, -180); // ----- Counter-clockwise / LEFT

        while (iterativeSum < degrees)
        {
            oi_update(sensor);
            iterativeSum += sensor->angle;
        }
    }
    oi_setWheels(0, 0); // else stop
}
