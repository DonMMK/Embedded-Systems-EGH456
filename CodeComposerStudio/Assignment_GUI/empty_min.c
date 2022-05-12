//LAB 3 TASK C SKELETON FILE
//This file is not complete and requires modification to get Task C to work
//This file will not compile in its current form
//You also need to have some correct modules included in the TI-RTOS .cfg file

// Add include files here

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
#include <ti/drivers/GPIO.h>
/* Board Header file */
#include "Board.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
/* std header files */
#include <stdint.h>
#include <stdbool.h>
/* Tiva C series macros header files */
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
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

// Increase the stacksize if needed
#define TASKSTACKSIZE   1024

// Task
Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];

// Hardware - Software interrupt handles
GateHwi_Handle gateHwi;
GateHwi_Params gHwiprms;

Swi_Handle SwiHandle;
Hwi_Handle UartHwiHandle;


// Variables and Buffers for drawing the shapes
uint32_t LineDrawingInitialX[32]; // Use larger buffers if neccessary
uint32_t LineDrawingInitialY[32]; // Use larger buffers if neccessary

uint32_t NumOfLines; // To count the number of lines that are being drawn.

uint32_t XCoordLineFinish[32];
uint32_t YCoordLineFinish[32];

uint32_t XPreviousValue;
uint32_t YPreviousValue;

uint32_t XCoordinateCursor; // For the cursor positions
uint32_t YCoordinateCursor; // For the cursor positions

// For the display
uint32_t LimitForLowerX; // Boundary Criteria for the display - Lower X
uint32_t LimitForLowerY; // Boundary Criteria for the display - Lower Y

uint32_t LimitForUpperX; // Boundary Criteria for the display - High X
uint32_t LimitForUpperY; // Boundary Criteria for the display - High Y

char InputChar; // Input for the Hwi and Swi
bool updateScreen;

//Interrupt Handlers
void UARTFxn(){
    UInt gateKey;

    //
    // Get and clear interrupt status.
    uint32_t intStatus = UARTIntStatus(UART0_BASE, UART_INT_RX);
    UARTIntClear(UART0_BASE, intStatus);

    //This should output to console, use to check if UART is working
    System_printf("UART is working - HWI\n");


    //Get character from UART
    char c = (char)UARTCharGetNonBlocking(UART0_BASE);

    //Use this to protect shared resources that might be used in Swi or Task threads
    //These need to be setup appropriately
    gateKey = GateHwi_enter(gateHwi);


    // Change the cursor position when character pressed
    // The character is taken in through the UART Console
    if (c == 'w' && YCoordinateCursor > LimitForLowerY) {           // Limit for upper y is 30
        YCoordinateCursor-=5;
    } else if (c == 'a' && XCoordinateCursor > LimitForLowerX) {
        XCoordinateCursor-=5;
    } else if (c == 's' && YCoordinateCursor < LimitForUpperY) {
        YCoordinateCursor+=5;
    } else if (c == 'd' && XCoordinateCursor < LimitForUpperX) {
        XCoordinateCursor+=5;
    } else if (c == '\r' || c == '\x7f'){ // trigger swi if return or backspace pressed
        InputChar = c;
        Swi_inc(SwiHandle);
    }

    // Change the flag to update the cursor
    updateScreen = true;

    GateHwi_leave(gateHwi, gateKey);


}

//Implement Swi Function to flag to task that a space character has been found
void SwiFxn(){
    UInt gateKey;

    //Ensure to output system prints
    System_printf("SWI is triggered\n");

    //use this to access shared buffers
    gateKey = GateHwi_enter(gateHwi);


    // update line buffer when the carriage return is pressed - change accordingly on mac
    if (InputChar == '\r'){

        // Dont draw anything the first time
        if (XPreviousValue == 0 && YPreviousValue == 0){

            XPreviousValue = XCoordinateCursor;
            YPreviousValue = YCoordinateCursor;

        } else {

            // if lines are drawn too many times reset the buffer
            if (NumOfLines == 32)
            {
                NumOfLines = 0;
            }

            // Initial position of the line drawn
            LineDrawingInitialX[NumOfLines] = XPreviousValue;
            LineDrawingInitialY[NumOfLines] = YPreviousValue;

            // Final position of the line drawn
            XCoordLineFinish[NumOfLines] = XCoordinateCursor;
            YCoordLineFinish[NumOfLines] = YCoordinateCursor;

            NumOfLines = NumOfLines + 1; // Increase the line count

            // Assign the current location to previous for next line drawing
            XPreviousValue = XCoordinateCursor;
            YPreviousValue = YCoordinateCursor;


        }

    } else if (InputChar == '\x7f') { // mac key for backspace - vs delete

        // Reset the buffer array
        memset(LineDrawingInitialX, 0, 32);
        memset(LineDrawingInitialY, 0, 32);

        // Reset line count to zero
        NumOfLines = 0;

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

    // Initialize the graphics context.
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);

    System_printf("LCD initialized.\n");
    System_flush();

    // Draws a frame - Change to Rect fill method in Lab 02 if needed
    FrameDraw(&sContext, "Don Lab 03");


    // for clearing screen
    sRect.i16XMin = 0;
    sRect.i16YMin = 15;
    sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.i16YMax = GrContextDpyHeightGet(&sContext) - 1;

    // set cursor to be in display center
    XCoordinateCursor = GrContextDpyWidthGet(&sContext) / 2;
    YCoordinateCursor = GrContextDpyHeightGet(&sContext) / 2;


    // set previous location to 0 (default, wont draw from this point)
    XPreviousValue = 0;
    YPreviousValue = 0;

    // display boundaries for cursor movement
    LimitForUpperX = GrContextDpyWidthGet(&sContext) - 10;
    LimitForUpperY = GrContextDpyHeightGet(&sContext) - 10;
    LimitForLowerX = 10;
    LimitForLowerY = 30;

    // lines drawn
    NumOfLines = 0;

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
            GrStringDrawCentered(&sContext, "X", -1, XCoordinateCursor, YCoordinateCursor, 0);


            // draw lines from buffers
            int i;
            for (i = 0; i < NumOfLines; i++){
                GrLineDraw(&sContext, LineDrawingInitialX[i], LineDrawingInitialY[i], XCoordLineFinish[i], YCoordLineFinish[i]);
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
    //Construct requi es stack parameters to be set
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
