/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty_min.c ========
 */

/* Lab 03 Task C header files */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"

#include "drivers/frame.h"
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/gates/GateHwi.h>

/* TI-RTOS Header files */
#include <ti/drivers/EMAC.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/SDSPI.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/USBMSCHFatFs.h>
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/WiFi.h>
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

/* Board Header file */
#include "Board.h"

#define TASKSTACKSIZE   512

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];


//This file is not complete and requires modification to get Task C to work
//This file will not compile in its current form
//You also need to have some correct modules included in the TI-RTOS .cfg file

Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];

GateHwi_Handle gateHwi;
GateHwi_Params gHwiprms;

Swi_Handle SwiHandle;
Hwi_Handle UartHwiHandle;

// You can use two different methods for UART hwi setup
// 1. creating the Hwi yourself and map the hwi to the UART receive/transmit interrupt
// 2. using the TI-RTOS UART driver and using the UART_read and UART_write functions

// Use these definitions if using TI-RTOS drivers
// UART_Handle uart;
// UART_Params uartParams;


//Interrupt Handlers
void UARTFxn(){
    UInt gateKey;

    //
    // Get and clear interrupt status.
    intStatus = UARTIntStatus(??);
    UARTIntClear(??, intStatus);

    //This should output to console, use to check if UART is working
    System_printf("Write Some Debug Info here\n");

    size_t i = 0;
    for(i = 0; i < size; i++){

        //Get character from UART
        c = (char)UARTCharGetNonBlocking(??);

        //Use this to protect shared resources that might be used in Swi or Task threads
        //These need to be setup appropriately
        gateKey = GateHwi_enter(gateHwi);

        //Recommendation is to use multiple buffers to keep track of which keys are pressed

        //Typical process here is trigger a Swi after a certain number of char have been received in buffer

        //alternatively you could check if space or carriage return conditions are met
        //Flag to swi thread that condition has been met

        GateHwi_leave(gateHwi, gateKey);
    }

}

//Implement Swi Function to flag to task that a space character has been found
void SwiFxn(){
    UInt gateKey;

    //Ensure to output system prints
    System_printf(??);

    //use this to access shared buffers
    gateKey = GateHwi_enter(gateHwi);

    //it is now safe to read from buffers
    //Put code in here to handle swi post event
    //Use gatekey only if access shared resources is needed
    GateHwi_leave(gateHwi, gateKey);

}

//An alternative method is to use the TI-RTOS driver implementation of uart
//This example skeleton needs to be modified for it to work with the TI-RTOS driver implementation
//you will need to investigate the driver functions UART_read() and UART_write()
// void openUART(){

//     /* Create a UART with data processing off. */
//     UART_Params_init(&uartParams);
//     uartParams.readMode = UART_MODE_CALLBACK;
//     uartParams.readCallback = UARTCallbackFxn;
//     uartParams.writeDataMode = ??;
//     uartParams.readDataMode = ??;
//     uartParams.readReturnMode = UART_RETURN_FULL;
//     uartParams.readEcho = UART_ECHO_OFF;
//     uartParams.baudRate = ??;
//     uart = UART_open(??, &uartParams);

//     if (uart == NULL) {
//         System_abort("Error opening the UART");
//     }
// }


//This is a suggested initialisation function for simple UART implementation
void initUART(){

    //Create Hwi for UART interrupt
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);

    //need to find the interrupt number for the uart and replace at XX
    UartHwiHandle = Hwi_create(??, (Hwi_FuncPtr)UARTFxn , &hwiParams, NULL);

    if (UartHwiHandle == NULL) {
        System_abort("Hwi create failed");
    }

    //
    // Configure and Enable the UART and interrupt for recieve and transmit.
    //
    // Need to include in arguments to configset:
    // base of UART device you want to use, baud rate and config for data length (8, stop bit is one, parity is none)
    Types_FreqHz cpuFreq;
    BIOS_getCpuFreq(&cpuFreq);
    UARTConfigSetExpClk(??, (uint32_t)cpuFreq.lo, ??, configs UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //Need to turn on the uart,
    UARTEnable(??);

    //This enables the interrupt for uart in the vector table
    IntEnable(??);

    //configure the what type of interrupt you want to cause an interrupt on the uart
    //Need to choose the right config values for the interrupt behaviours you want
    //Have a look in driverlib/uart.h for some predefines for these settings
    UARTIntEnable(??, ?? | ??);

}


void displayFxn(UArg arg0, UArg arg1)
{
    //Make sure you have setup variables here

    Types_FreqHz cpuFreq;
    BIOS_getCpuFreq(&cpuFreq);

    //Add init functions for LCD and touch screen
    ??

    //Title screen for display
    //Feel free to modify this
    FrameDraw(&sContext, "Lab 3 Drawing App");
    GrRectFill(&sContext, &fill);


    while(1) {

        //use this to access shared buffers
        gateKey = GateHwi_enter(gateHwi);
        //it is now safe to read from buffers

        //Add logic here to see if should draw line to display
        System_printf("Received a second carriage return character drawing line to screen \n");
        GrLineDraw(&sContext, ??);

        //make sure to flag we have finished accessing buffers
        GateHwi_leave(gateHwi, gateKey);

        //Use this to print debug info to console while task is running
        System_flush();
    }
}




/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
//Void heartBeatFxn(UArg arg0, UArg arg1)
//{
//    while (1) {
//        Task_sleep((unsigned int)arg0);
//        GPIO_toggle(Board_LED0);
//    }
//}

/*
 *  ======== main ========
 */
//int main(void)
//{
//    Task_Params taskParams;
//
//    /* Call board init functions */
//    Board_initGeneral();
//    // Board_initEMAC();
//    Board_initGPIO();
//    // Board_initI2C();
//    // Board_initSDSPI();
//    // Board_initSPI();
//    // Board_initUART();
//    // Board_initUSB(Board_USBDEVICE);
//    // Board_initUSBMSCHFatFs();
//    // Board_initWatchdog();
//    // Board_initWiFi();
//
//    /* Construct heartBeat Task  thread */
//    Task_Params_init(&taskParams);
//    taskParams.arg0 = 1000;
//    taskParams.stackSize = TASKSTACKSIZE;
//    taskParams.stack = &task0Stack;
//    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);
//
//    /* Turn on user LED  */
//    GPIO_write(Board_LED0, Board_LED_ON);
//
//    /* Start BIOS */
//    BIOS_start();
//
//    return (0);
//}


/*
 *  ======== main ========
 */
int main(void)
{
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();

    //This can be used for either Peripheral driver or TI-RTOS driver
    //Primarily this setups UART pins and we can reuse that
    //This will also create a TI-RTOS uart via the config structs but
    //usually doesn't conflict if we setup a Hwi directly
    Board_initUART();

    /* Construct BIOS objects */
    Task_Params taskParams;

    //Create the task threads
    //Can use create or construct
    //Construct requies stack parameters to be set
    Task_Params_init(??);
    taskParams.stackSize = ??;
    taskParams.stack = ??;
    taskParams.instance->name = "display";
    Task_construct(??);

    //Create the Swi thread
    //Can use construct or create
    Swi_Params_init(??);
    SwiHandle = Swi_create(??);

    //Good practice to check if handle was created
    if (SwiHandle == NULL) {
       System_abort("Swi create failed");
    }

    //Create Hwi Gate Mutex
    GateHwi_Params_init(&gHwiprms);
    gateHwi = GateHwi_create(&gHwiprms, NULL);
    if (gateHwi == NULL) {
        System_abort("Gate Hwi create failed");
    }

    //Call function to initialise UART
    initUART();

    /* This example has logging and many other debug capabilities enabled */
    System_printf("This example does not attempt to minimize code or data "
                  "footprint\n");

    System_printf("Starting the UART Echo example\nSystem provider is set to "
                  "SysMin. Halt the target to view any SysMin contents in "
                  "ROV.\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();


    return (0);
}
