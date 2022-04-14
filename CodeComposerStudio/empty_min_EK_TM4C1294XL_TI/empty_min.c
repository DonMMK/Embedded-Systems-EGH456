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
//LAB 3 TASK C SKELETON FILE
//This file is not complete and requires modification to get Task C to work
//This file will not compile in its current form
//You also need to have some correct modules included in the TI-RTOS .cfg file
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/gates/GateHwi.h>

/* TI-RTOS Header files */
// #include <ti/drivers/EMAC.h>
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/USBMSCHFatFs.h>
// #include <ti/drivers/Watchdog.h>
// #include <ti/drivers/WiFi.h>

/* Board Header file */
#include "Board.h"


/* std header files */
#include <stdint.h>
#include <stdbool.h>

/* Tiva C series macros header files */
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

/* Tiva C series driverlib header files */
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"


/* grlib header files */
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "drivers/frame.h"

/* LCD drivers header files */
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"



#define TASKSTACKSIZE   512

Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];

GateHwi_Handle gateHwi;
GateHwi_Params gHwiprms;

Swi_Handle SwiHandle;
Hwi_Handle UartHwiHandle;



// character received in hwi, for swi to read
char charRec;


// Drawing variables

uint32_t cursorX;
uint32_t cursorY;
uint32_t prevX;
uint32_t prevY;
uint32_t linesStartX[32];
uint32_t linesStartY[32];
uint32_t linesEndX[32];
uint32_t linesEndY[32];
uint32_t linesCount;



// display boundaries
uint32_t xUpperLimit;
uint32_t yUpperLimit;
uint32_t xLowerLimit;
uint32_t yLowerLimit;

bool updateScreen;



//Interrupt Handlers
void UARTFxn(){
    UInt gateKey;

    //
    // Get and clear interrupt status.
    uint32_t intStatus = UARTIntStatus(UART0_BASE, UART_INT_RX);
    UARTIntClear(UART0_BASE, intStatus);

    //This should output to console, use to check if UART is working
    System_printf("HWI\n");


    //Get character from UART
    char c = (char)UARTCharGetNonBlocking(UART0_BASE);

    //Use this to protect shared resources that might be used in Swi or Task threads
    //These need to be setup appropriately
    gateKey = GateHwi_enter(gateHwi);


    // update cursor position
    if (c == 'w' && cursorY > yLowerLimit) {
        cursorY-=5;
    } else if (c == 'a' && cursorX > xLowerLimit) {
        cursorX-=5;
    } else if (c == 's' && cursorY < yUpperLimit) {
        cursorY+=5;
    } else if (c == 'd' && cursorX < xUpperLimit) {
        cursorX+=5;
    } else if (c == '\r' || c == '\x7f'){ // trigger swi if return or backspace pressed
        charRec = c;
        Swi_inc(SwiHandle);
    }

    updateScreen = true;

    GateHwi_leave(gateHwi, gateKey);


}

//Implement Swi Function to flag to task that a space character has been found
void SwiFxn(){
    UInt gateKey;

    //Ensure to output system prints
    System_printf("SWI \n");

    //use this to access shared buffers
    gateKey = GateHwi_enter(gateHwi);


    // if return carriage received, update lines buffer for display task
    if (charRec == '\r'){

        // if first time here, set previous location as current
        // (won't draw, lines count still 0)
        if (prevX == 0 && prevY == 0){

            prevX = cursorX;
            prevY = cursorY;

        } else {

            // wrap around buffer
            if (linesCount == 32) { linesCount = 0; }

            // line drawing start position
            linesStartX[linesCount] = prevX;
            linesStartY[linesCount] = prevY;

            // line drawing end position
            linesEndX[linesCount] = cursorX;
            linesEndY[linesCount] = cursorY;

            // increment count of lines to draw for display task
            linesCount++;

            // update previous location to current for the next line drawing
            prevX = cursorX;
            prevY = cursorY;


        }

    } else if (charRec == '\x7f') { // backspace received

        // clear lines buffer
        memset(linesStartX, 0, 32);
        memset(linesStartY, 0, 32);

        // set count of lines to draw to zero for display task
        linesCount = 0;

    }



    GateHwi_leave(gateHwi, gateKey);

}



//This is a suggested initialisation function for simple UART implementation
void initUART(){

    //Create Hwi for UART interrupt
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);

    //need to find the interrupt number for the uart and replace at XX
    UartHwiHandle = Hwi_create(INT_UART0_TM4C129, (Hwi_FuncPtr)UARTFxn , &hwiParams, NULL);

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
    UARTConfigSetExpClk(UART0_BASE, (uint32_t)cpuFreq.lo, 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

    //Need to turn on the uart,
    UARTEnable(UART0_BASE);

    //This enables the interrupt for uart in the vector table
    IntEnable(INT_UART0);

    //configure the what type of interrupt you want to cause an interrupt on the uart
    //Need to choose the right config values for the interrupt behaviours you want
    //Have a look in driverlib/uart.h for some predefines for these settings
    UARTIntEnable(UART0_BASE, UART_INT_RT | UART_INT_TX);

}


void displayFxn(UArg arg0, UArg arg1)
{
    UInt gateKey;

    tContext sContext;
    tRectangle sRect;

    //Make sure you have setup variables here

    Types_FreqHz cpuFreq;
    BIOS_getCpuFreq(&cpuFreq);

    // init functions for LCD and touch screen
    //
    uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                                 SYSCTL_OSC_MAIN |
                                                 SYSCTL_USE_PLL |
                                                 SYSCTL_CFG_VCO_480), 120000000);
    Kentec320x240x16_SSD2119Init(g_ui32SysClock);

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);

    System_printf("LCD initialized.\n");
    System_flush();


    //Title screen for display
    //
    // Draw the application frame.
    //
    FrameDraw(&sContext, "Lab 3 Drawing App");


    // for clearing screen
    sRect.i16XMin = 0;
    sRect.i16YMin = 23;
    sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.i16YMax = GrContextDpyHeightGet(&sContext) - 1;

    // set cursor to be in display center
    cursorX = GrContextDpyWidthGet(&sContext) / 2;
    cursorY = GrContextDpyHeightGet(&sContext) / 2;


    // set previous location to 0 (default, wont draw from this point)
    prevX = 0;
    prevY = 0;

    // display boundaries for cursor movement
    xUpperLimit = GrContextDpyWidthGet(&sContext) - 10;
    yUpperLimit = GrContextDpyHeightGet(&sContext) - 10;
    xLowerLimit = 10;
    yLowerLimit = 30;

    // lines drawn
    linesCount = 0;

    updateScreen = true;

    while(1) {

        //use this to access shared buffers
        gateKey = GateHwi_enter(gateHwi);
        //it is now safe to read from buffers


        if (updateScreen == 1){

            // clear screen
            GrContextForegroundSet(&sContext, ClrBlack);
            GrRectFill(&sContext, &sRect);
            GrFlush(&sContext);


            // draw cursor
            GrContextForegroundSet(&sContext, ClrWhite);
            GrStringDrawCentered(&sContext, "X", -1, cursorX, cursorY, 0);


            // draw lines from buffers
            int i;
            for (i = 0; i < linesCount; i++){
                GrLineDraw(&sContext, linesStartX[i], linesStartY[i], linesEndX[i], linesEndY[i]);
            }

            updateScreen = false;

        }

        //Add logic here to see if should draw line to display
        //System_printf("Received a second carriage return character drawing line to screen \n");
        //make sure to flag we have finished accessing buffers
        GateHwi_leave(gateHwi, gateKey);


        //Use this to print debug info to console while task is running
        System_flush();
    }
}

/*
 *  ======== main ========
 */
int main(void){
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
    Swi_Params swiParams;

    //Create the task threads
    //Can use create or construct
    //Construct requies stack parameters to be set
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    taskParams.instance->name = "display";
    Task_construct(&task1Struct, (Task_FuncPtr) displayFxn, &taskParams, NULL);


    //Create the Swi thread
    //Can use construct or create
    Swi_Params_init(&swiParams);
    SwiHandle = Swi_create((Task_FuncPtr) SwiFxn, &swiParams, NULL);

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
