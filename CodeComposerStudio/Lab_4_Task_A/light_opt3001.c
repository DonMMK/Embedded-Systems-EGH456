//*****************************************************************************
//
// light_opt3001.c - Example to use OPT3001
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
//
//*****************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "drivers/pinout.h"
#include "drivers/buttons.h"

#include "drivers/opt3001.h"

//*****************************************************************************
//
//Light Measurement with the OPT3001
//
// This example demonstrates the basic use of the OPT3001 light sensor
//
// The Sensors BoosterPack must be installed on BoosterPack 1 interface.
// Code changes will be needed if installed on BoosterPack 2 interface.
//
//
//*****************************************************************************


//*****************************************************************************
//
// The system tick rate expressed both as ticks per second and a millisecond
// period.
//
//*****************************************************************************
#define SYSTICKS_PER_SECOND     1
#define SYSTICK_PERIOD_MS       (1000 / SYSTICKS_PER_SECOND)

//*****************************************************************************
//
// Global variable for storage of actual system clock frequency.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif



//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}


//*****************************************************************************
//
// Detect Interrupt
//
//*****************************************************************************

uint32_t check_delay;
void I2C0EventDetectInterrupt(void){
    check_delay +=1;

    if(check_delay >(g_ui32SysClock/300)){
        check_delay =  0;
        bool success;
        uint16_t  rawData = 0;
        float     convertedLux = 0;
        char      tempStr[50];

        success = sensorOpt3001Read(&rawData);
        if (success) {
            sensorOpt3001Convert(rawData, &convertedLux);
            //Relevant if conditions
            if(convertedLux > 2818){
                 sprintf(tempStr, "High Light Event: %5.2f Lux\n", convertedLux);
                 UARTprintf("%s\n", tempStr);
            }
            else if(convertedLux < 40.95){
                sprintf(tempStr, "Low Light Event: %5.2f Lux\n", convertedLux);
                UARTprintf("%s\n", tempStr);
            }else{
                sprintf(tempStr, "Lux: %5.2f\n", convertedLux);
                UARTprintf("%s\n", tempStr);
            }
        }

    }

}




//*****************************************************************************
//
// Main 'C' Language entry point.
//
//*****************************************************************************
int main(void)
{
    bool      worked, success;
    uint16_t  rawData = 0;
    float     convertedLux = 0;
    char      tempStr[40];

    //
    // Configure the system frequency.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);


    // Initialise the delay
    check_delay = 0;

    //
    // Configure the device pins for this board.
    // This application does not use Ethernet or USB.
    //
    PinoutSet(false, false);

    //
    // Initialize the UART.
    //
    ConfigureUART();

    //
    // Clear the terminal and print the welcome message.
    //
    UARTprintf("\033[2J\033[H");
    UARTprintf("OPT001 Example\n");

    //
    // The I2C0 peripheral must be enabled before use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    // This step is not necessary if your part does not support pin muxing.
    //
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    //
    // Select the I2C function for these pins.  This function will also
    // configure the GPIO pins pins for I2C operation, setting them to
    // open-drain operation with weak pull-ups.  Consult the data sheet
    // to see which functions are allocated per pin.
    //
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    //
    // Enable interrupts to the processor.
    //
    IntMasterEnable();


    // Test that sensor is set up correctly
    UARTprintf("\nTesting OPT3001 Sensor:\n");
    worked = sensorOpt3001Test();

    while (!worked) {
        SysCtlDelay(g_ui32SysClock);
        UARTprintf("\nTest Failed, Trying again\n");
        worked = sensorOpt3001Test();
    }

    UARTprintf("All Tests Passed!\n\n");

    // Initialize opt3001 sensor
    sensorOpt3001Init();
    sensorOpt3001Enable(true);

    // Loop Forever
    while(1)
    {

        SysCtlDelay(g_ui32SysClock/100);


//        SysCtlDelay(g_ui32SysClock/100);
//
//        //Read and convert OPT values
//        success = sensorOpt3001Read(&rawData);
//
//        if (success) {
//            sensorOpt3001Convert(rawData, &convertedLux);
//
//            // Construct Text
//            sprintf(tempStr, "Lux: %5.2f\n", convertedLux);
//            UARTprintf("%s\n", tempStr);
//
//
//        }

    }
}
