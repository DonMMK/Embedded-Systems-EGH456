//LAB 3 TASK C SKELETON FILE

//ADD INCLUDE FILES HERE
//e.g.
#include <stdint.h>
#include <stdbool.h>
//#include <stdio.h>
#include <string.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
//#include <ti/drivers/UART.h>

/* Board Header file */
#include "Board.h"

/* Driverlib Header files */
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/fpu.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

/* grlib Header files */
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

//This file is not complete and requires modification to get Task C to work
//This file will not compile in its current form
//You also need to have some correct modules included in the TI-RTOS .cfg file

#define TASKSTACKSIZE   512
#define UARTBUFFERSIZE  100
#define COORDBUFFERSIZE 5

#define HWI_ID          1
#define NUMMSGS         3       /* Number of messages */
#define TIMEOUT         12      /* Timeout value */

typedef struct MsgObj {
    Int         id;             /* Writer task id */
    Char        val;            /* Message value */
} MsgObj, *Msg;

Mailbox_Struct mbxStruct;
Mailbox_Handle mbxHandle;

Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE]; // display function stack

GateHwi_Handle gateHwi; // shared resource mutex
GateHwi_Params gHwiParams;

Hwi_Handle HwiHandle; // read char inputs

Semaphore_Struct sem0Struct, sem1Struct;
Semaphore_Handle sem0Handle, sem1Handle;

Event_Struct evtStruct;
Event_Handle evtHandle;

uint32_t intStatus;

char uartBuffer[UARTBUFFERSIZE];

int32_t x1_coords[COORDBUFFERSIZE];
int32_t y1_coords[COORDBUFFERSIZE];
int32_t x2_coords[COORDBUFFERSIZE];
int32_t y2_coords[COORDBUFFERSIZE];

int32_t xLims[2];
int32_t yLims[2];
int32_t cursorLoc[2] = {0};

int32_t lineCounter = 0;
int32_t inputChar = 0;

bool drawingLine = false;
bool drawingStarted = false;
bool screenUpdate = true;

// You can use two different methods for UART hwi setup
// 1. creating the Hwi yourself and map the hwi to the UART receive/transmit interrupt
// 2. using the TI-RTOS UART driver and using the UART_read and UART_write functions

// Use these definitions if using TI-RTOS drivers
// UART_Handle uart;
// UART_Params uartParams;


//Interrupt Handlers
void UARTFxn(){
    UInt gateKey;
    char c;
    int32_t cInt;
    MsgObj msg;

    //
    // Get and clear interrupt status.
    intStatus = UARTIntStatus(UART0_BASE, false);
    UARTIntClear(UART0_BASE, intStatus);

    //This should output to console, use to check if UART is working
//    System_printf("Triggered Hwi Interrupt\n");

    size_t size = 5; // receive 5 chars at a time before returning to Swi
    size_t i;
    for(i = 0; i < size; i++)
    {
        //Get character from UART
        cInt = UARTCharGetNonBlocking(UART0_BASE);
        if (cInt == -1)
        {
            break;
        }
        else
        {
            c = (char)cInt;
//            System_printf("Hwi received UART char \n");
        }

        //Use this to protect shared resources that might be used in Swi or Task threads
        gateKey = GateHwi_enter(gateHwi);

        //Recommendation is to use multiple buffers to keep track of which keys are pressed
        //Typical process here is trigger a Swi after a certain number of char have been received in buffer
//        inputChar = cInt;
        screenUpdate = true;

        if(c == 'w' && cursorLoc[1] > yLims[0])
        {
            cursorLoc[1] -= 5;
        }
        else if(c == 's' && cursorLoc[1] < yLims[1])
        {
            cursorLoc[1] += 5;
        }
        else if(c == 'a' && cursorLoc[0] > xLims[0])
        {
            cursorLoc[0] -= 5;
        }
        else if(c == 'd' && cursorLoc[0] < xLims[1])
        {
            cursorLoc[0] += 5;
        }
        else if(cInt == 13) // enter character
        {
            msg.id = i;
            msg.val = c;

            Mailbox_post(mbxHandle, &msg, BIOS_NO_WAIT);
            Event_post(evtHandle, Event_Id_00);
        }
        else if(cInt == 127) // backspace character
        {
            msg.id = i;
            msg.val = c;

            Mailbox_post(mbxHandle, &msg, BIOS_NO_WAIT);
            Event_post(evtHandle, Event_Id_01);
        }
        else
        {
            screenUpdate = false;
        }

        //alternatively you could check if space or carriage return conditions are met
        //Flag to swi thread that condition has been met
        GateHwi_leave(gateHwi, gateKey);
    }
}

void enterFxn()
{
    MsgObj msg;

    System_printf("enter function entered\n");
    if(Mailbox_pend(mbxHandle, &msg, BIOS_NO_WAIT)) {
        System_printf("enterFxn: id = %d and val = '%c'.\n",msg.id, msg.val);
    }

    if(lineCounter < COORDBUFFERSIZE) // enter character
    {
        if(!drawingLine) // start drawing new line from cursor location
        {
            x1_coords[lineCounter] = cursorLoc[0];
            y1_coords[lineCounter] = cursorLoc[1];

            drawingLine = true;
        }
        else // finish drawing line at cursor location
        {
            x2_coords[lineCounter] = cursorLoc[0];
            y2_coords[lineCounter] = cursorLoc[1];

            lineCounter++;

            if(lineCounter < COORDBUFFERSIZE)
            {
                x1_coords[lineCounter] = cursorLoc[0];
                y1_coords[lineCounter] = cursorLoc[1];
            }
        }
    }
}

void backspaceFxn()
{
    MsgObj msg;

    System_printf("backspace function entered\n");
    if(Mailbox_pend(mbxHandle, &msg, BIOS_NO_WAIT)) {
        System_printf("backspaceFxn: id = %d and val = '%c'.\n",msg.id, msg.val);
    }

    memset(x1_coords, 0, sizeof(x1_coords));
    memset(y1_coords, 0, sizeof(y1_coords));

    memset(x2_coords, 0, sizeof(x2_coords));
    memset(y2_coords, 0, sizeof(y2_coords));

    lineCounter = 0;
    drawingLine = false;
}

//This is a suggested initialisation function for simple UART implementation
void initUART(){

    //Create Hwi for UART interrupt
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);

    //need to find the interrupt number for the uart and replace at XX
    HwiHandle = Hwi_create(INT_UART0, (Hwi_FuncPtr)UARTFxn , &hwiParams, NULL);

    if (HwiHandle == NULL) {
        System_abort("Hwi create failed");
    }

    //
    // Configure and Enable the UART and interrupt for recieve and transmit.
    //
    // Need to include in arguments to configset:
    // base of UART device you want to use, baud rate and config for data length (8, stop bit is one, parity is none)
    Types_FreqHz cpuFreq;
    BIOS_getCpuFreq(&cpuFreq);
    UARTConfigSetExpClk(UART0_BASE, (uint32_t)cpuFreq.lo, 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //Need to turn on the uart,
    UARTEnable(UART0_BASE);

    //This enables the interrupt for uart in the vector table
    IntEnable(INT_UART0);

    //configure the what type of interrupt you want to cause an interrupt on the uart
    //Need to choose the right config values for the interrupt behaviours you want
    //Have a look in driverlib/uart.h for some predefines for these settings
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

}

void displayFxn(UArg arg0, UArg arg1)
{
    System_printf("Initialising Display \n");
    //Make sure you have setup variables here
    UInt gateKey;
    tContext sContext;
    tRectangle sRect;

    // event and queue vars
    UInt posted;

    Types_FreqHz cpuFreq;
    BIOS_getCpuFreq(&cpuFreq);

    FPUEnable();
    FPULazyStackingEnable();

    //Add init functions for LCD and touch screen
    // Initialize the display driver and graphics context.
    Kentec320x240x16_SSD2119Init((uint32_t)cpuFreq.lo);
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);

    // get limits for cursor coordinates
    gateKey = GateHwi_enter(gateHwi);
    xLims[0] = 0;
    xLims[1] = GrContextDpyWidthGet(&sContext) - 1;
    yLims[0] = 0;
    yLims[1] = GrContextDpyHeightGet(&sContext) - 1;
    GateHwi_leave(gateHwi, gateKey);

    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.i16YMax = GrContextDpyHeightGet(&sContext) - 1;

    GrContextForegroundSet(&sContext, ClrDarkBlue);
    GrRectFill(&sContext, &sRect);

    //Title screen for display
    //Feel free to modify this
    GrContextForegroundSet(&sContext, ClrWhite);
    GrContextFontSet(&sContext, &g_sFontCm20);
    GrStringDrawCentered(&sContext, "Lab 3 Drawing App", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 40, 0);
//    GrRectFill(&sContext, &fill);

    System_printf("Display Initialised \n");

    size_t i;
    for(i = 0; i < 6000; i++){} // wait on loading screen for a bit

    // main task loop
    while(1)
    {
        //use this to access shared buffers
        gateKey = GateHwi_enter(gateHwi);
        //it is now safe to read from buffers

        //Add logic here to see if should draw line to display
        if(screenUpdate)
        {
            screenUpdate = false;
            System_printf("Updating Screen \n");

            // pend for an event without waiting
            posted = Event_pend(evtHandle, Event_Id_NONE,
                                Event_Id_00 + Event_Id_01, BIOS_NO_WAIT);

            if(posted & Event_Id_00) {
                enterFxn();
            }
            if(posted & Event_Id_01) {
                backspaceFxn();
            }

            // clear screen
            GrContextForegroundSet(&sContext, ClrBlack);
            GrRectFill(&sContext, &sRect);
            GrFlush(&sContext);

            // draw cursor
             GrContextForegroundSet(&sContext, ClrWhite);
             GrStringDrawCentered(&sContext, "o", -1, cursorLoc[0]+1, cursorLoc[1]-4, 0);

            // draw lines
            for(i = 0; i < lineCounter; i++)
            {
                GrLineDraw(&sContext, x1_coords[i], y1_coords[i], x2_coords[i], y2_coords[i]);
            }
        }

        //make sure to flag we have finished accessing buffers
        GateHwi_leave(gateHwi, gateKey);

        //Use this to print debug info to console while task is running
        System_flush();
    }
}

 /*  ======== main ======== */
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
//    Semaphore_Params semParams;
    Mailbox_Params mbxParams;

    //Create the task threads
    //Can use create or construct
    //Construct requies stack parameters to be set
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    taskParams.instance->name = "display";
    Task_construct(&task1Struct, (Task_FuncPtr)displayFxn, &taskParams, NULL);

    /* Obtain event instance handle */
    Event_construct(&evtStruct, NULL);
    evtHandle = Event_handle(&evtStruct);

    // create mailbox
    Mailbox_Params_init(&mbxParams);
    mbxParams.readerEvent = evtHandle;
    mbxParams.readerEventId = Event_Id_02;
    Mailbox_construct(&mbxStruct, sizeof(MsgObj), 2, &mbxParams, NULL);
    mbxHandle = Mailbox_handle(&mbxStruct);


    //Create Hwi Gate Mutex
    GateHwi_Params_init(&gHwiParams);
    gateHwi = GateHwi_create(&gHwiParams, NULL);
    if (gateHwi == NULL) {
        System_abort("Gate Hwi create failed");
    }

//    Semaphore_Params_init(&semParams);
//    semParams.mode = Semaphore_Mode_BINARY;
//
//    Semaphore_construct(&sem0Struct, 0, &semParams);
//    Semaphore_construct(&sem1Struct, 0, &semParams);
//
//    sem0Handle = Semaphore_handle(&sem0Struct);
//    sem1Handle = Semaphore_handle(&sem1Struct);

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
