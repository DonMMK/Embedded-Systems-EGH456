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
/* XDC module Headers */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

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
#include <ti/sysbios/gates/GateSwi.h>
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Queue.h>

#include <xdc/runtime/Error.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Tivaware DriverLib */
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "inc/tm4c129xnczad.h"



#include "grlib/grlib.h"

#include "drivers/frame.h"
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/pinout.h"


#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>

#include <xdc/cfg/global.h>

/* Example/Board Header files */
#include "Board.h"
#include <ti/sysbios/knl/Queue.h>




#define NUMMSGS 3 /* number of messages */
#define NUMWRITERS 3 /* number of writer tasks created with */
#define TIMEOUT         12      /* Timeout value */

Queue_Handle q;

#define TASKSTACKSIZE   5000

Void clk0Fxn(UArg arg0);
Void clk1Fxn(UArg arg0);
Void displayFxn(UArg arg0, UArg arg1);
Void writertask(UArg arg0, UArg arg1);


char rxBuf[8];
char shared_buffer[64];
char shared_buffer_LCD[64];
int idx = 0;
int idx_lcd = 0;
bool should_print = false;
bool should_echo = false;

GateHwi_Handle gateHwi;
GateHwi_Params gHwiprms;


//Swi_Handle SwiHandle;
Hwi_Handle UartHwiHandle;

UART_Handle uart;
UART_Params uartParams;

Task_Struct task0Struct, task1Struct;
Char task0Stack[TASKSTACKSIZE], task1Stack[TASKSTACKSIZE];
Semaphore_Struct sem0Struct, sem1Struct;
Semaphore_Handle semHandle;
int count = 0;



typedef struct Rec {
    Queue_Elem _elem;
    char data;
    int output;
} Rec;




Void heartBeatFxn(UArg arg0, UArg arg1)
{
    //This is from the heart beat meaning this is activated when we click r ir space
    while (1) {
        System_flush();

        Task_sleep((unsigned int)5);


        if(!(UART_read(uart, rxBuf, 1))){
            count++;
        }
        System_flush();

    }
}

/*
 *  ======== main ========
 */

Rec input[200];
void UARTFxn(UART_Handle handle, void *rxBuf, size_t size){

    char c;

    Int i;


    System_printf("Received read buffer \n");

    //    Rec * value = ( *) malloc(sizeof(person));

    //    System_printf("value %d \n",count);

    size_t m = 0;
    for(m = 0; m < size; m++){
        c = ((uint8_t*)rxBuf)[i];
        //        gateKey = GateHwi_enter(gateHwi);


        if (c == ' '){
            input[count].data = 's';
            Queue_enqueue(q, &input[count]._elem);
            Semaphore_post(semHandle);
            count = 0;
            System_flush();


        }else if (c == '\r'){
            input[count].data = 'e';
            Queue_enqueue(q, &input[count]._elem);
            Semaphore_post(semHandle);
            count = 0;
            System_flush();
        }else{
            input[count].data = c;
            Queue_enqueue(q, &input[count]._elem);
        }


    }

}


void openUART(){

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);


    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = UARTFxn;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }
}



Int main()
{

    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();

    q = Queue_create(NULL, NULL);


    /* Construct BIOS Objects */
    Task_Params taskParams;
    Semaphore_Params semParams;

    //
    /* Call board init functions */

    /* Construct writer/reader Task threads */
    Task_Params_init(&taskParams);
    //    taskParams.arg0 = (UArg)mbxHandle;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.priority = 1;


    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)displayFxn, &taskParams, NULL);

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000;
    Task_create((Task_FuncPtr)heartBeatFxn, &taskParams, NULL);


    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&sem0Struct, 0, &semParams);

    semHandle = Semaphore_handle(&sem0Struct);


    openUART();
    BIOS_start();    /* Does not return */
    return(0);
}



/*
 *  ======== reader ========
 */
//Void displayFxn(UArg arg0, UArg arg1)
//{
//
//
//}

/*
 *  ======== writer ========
 */
Void displayFxn(UArg arg0, UArg arg1)
{

    tContext sContext;
    const tRectangle fill = {.i16XMin = 0, .i16XMax = 320, .i16YMin = 0, .i16YMax = 240 };

    Types_FreqHz cpuFreq;
    BIOS_getCpuFreq(&cpuFreq);

    //Add init functions for LCD and touch screen
    Kentec320x240x16_SSD2119Init((uint32_t)cpuFreq.lo);

    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrContextFontSet(&sContext, &g_sFontCm20);

    //Title screen for display
    FrameDraw(&sContext, "UART Echo RTOS");
    GrRectFill(&sContext, &fill);

    //Print to terminal some user prompt
    UART_write(uart, (uint8_t *)"\fEnter text: ", 12);

    //    while(1) {
    System_printf("where is read used\n");



    while(1)
    {


        if(Semaphore_pend(semHandle, BIOS_WAIT_FOREVER))
        {

            Rec* rp;
            char buff[200];
            int i = 0;
            int m = 0;
            char num;

            while (!Queue_empty(q))
            {
                rp = Queue_dequeue(q);
                buff[i] = rp->data;

                //System_printf("%c", rp->data);
                i++;
            }

            num = buff[i-1];

            if(num == 'e')
            {
                GrRectFill(&sContext, &fill);
                int n;
                for(n = 0; n<i;n++)
                {
                    System_printf("%c", buff[n]);


                    GrStringDraw(&sContext, &buff[n], i-1, 40, 40, 1);
                    //                should_print = false;
                    //                idx_lcd = 0;
                    i = 0;

                }
            }
            else if(num == 's')
            {
                    System_printf("Received a space character echoing to terminal \n");

                    UART_write(uart, &buff, i-1);

            }

        }
    }

}



