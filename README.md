# CPRE288_Final_Project

## Autonomous Search & Destroy Robot
Embedded C project for an autonomous robot on the iRobot Create 2 with TM4C123GH6PM, simulating a VALORANT-inspired Search & Destroy game. The robot navigates, scans with PING/IR sensors, detects objects, avoids obstacles, and locates a "planting area" to win. Built by Nick & Jash (Team A-2) using Code Composer Studio and PuTTY.

## Features
- Autonomous navigation with obstacle avoidance.
- 180° scans to detect objects and calculate widths.
- Manual control via PuTTY (w, a, s, d).
- Victory tune (Rick Astley) on detecting planting area.
- UART output for real-time data.

## Demo
- Scanning: Robot scans 180° with PING/IR sensors, reporting distances via PuTTY.
- Movement: Robot navigates, avoiding obstacles and responding to manual inputs.

## Requirements
- Hardware: TM4C123GH6PM, iRobot Create 2, PING/IR sensors, servo, LCD.
- Software: Code Composer Studio, PuTTY (115200 baud), cyBot_Scan.h, movement.h, uart-interrupt.h, adc.h, timer.h, lcd.h.

## License
MIT License. See LICENSE.
