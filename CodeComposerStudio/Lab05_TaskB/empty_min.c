//LAB 3 TASK C SKELETON FILE
//This file is not complete and requires modification to get Task C to work
//This file will not compile in its current form
//You also need to have some correct modules included in the TI-RTOS .cfg file

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
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

/* Example/Board Header files */
#include "Board.h"
#include "grlib/grlib.h"
#include "drivers/frame.h"
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/pinout.h"

#define TASKSTACKSIZE 768
#define NUMMSGS 50
#define TIMEOUT 20

void displayFxn(UArg arg0, UArg arg1);
void UARTFxn(UART_Handle handle, void *rxBuf, size_t size, int id);
void openUART();
void displayFxn(UArg arg0, UArg arg1);

typedef struct MsgObj {
    Queue_Elem elem;    /* first field for Queue */
    int id;             /* writer task id */
    Char data;          /* message value */
} MsgObj, *Msg;

Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];

ti_sysbios_gates_GateHwi_Handle gateHwi;
ti_sysbios_gates_GateHwi_Params gHwiprms;

Hwi_Handle UartHwiHandle;

char rxBuf[8];
char shared_buffer[64];
char shared_buffer_LCD[64];
int idx = 0;
int idx_2 = 0;
int count = 0;

//Semaphore
Semaphore_Struct semStruct0;
Semaphore_Struct semStruct1;
Semaphore_Handle semHandle;

//Event
Event_Struct evtStruct;
Event_Handle evtHandle;

GateHwi_Handle gateHwi;
GateHwi_Params gHwiprms;

Hwi_Handle UartHwiHandle;

UART_Handle uart;
UART_Params uartParams;

Queue_Handle my_queue;
Queue_Handle my_queue_lcd;
MsgObj msg_mem[NUMMSGS];

void UARTFxn(UART_Handle handle, void *rxBuf, size_t size, int id){
    UInt gateKey;
    MsgObj *msg;
    char c;

    //uint32_t intStatus;
    System_printf("Received read buffer \n");

    size_t x = 0;
    for(x = 0; x < size; x++){

        c = ((uint8_t*)rxBuf)[x];
        gateKey = GateHwi_enter(gateHwi);
        msg = Queue_get(my_queue_lcd);
        msg->data = c;
        count++;
        Queue_put(my_queue, &msg->elem);

        if (c == ' '){
            System_printf("Received a space character echoing buffer \n");
            Event_post(evtHandle, Event_Id_01);

        }else if (c == '\r'){
            Event_post(evtHandle, Event_Id_00);
        }
        Semaphore_post(semHandle);
        GateHwi_leave(gateHwi, gateKey);
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


void displayFxn(UArg arg0, UArg arg1)
{
    UInt gateKey;
    tContext sContext;
    const tRectangle fill = {.i16XMin = 0, .i16XMax = 320, .i16YMin = 0, .i16YMax = 240 };

    Msg msg;
    UInt posted;
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

    while(1) {

        UART_read(uart, rxBuf, 1);
        posted = Event_pend(evtHandle,Event_Id_NONE,Event_Id_00 + Event_Id_01,TIMEOUT);
        gateKey = GateHwi_enter(gateHwi);

        if (posted)
        {

            Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
            int x;
            for(x=0; x < count; x++){

                msg = Queue_get(my_queue);
                shared_buffer[idx++] = msg->data;
                shared_buffer_LCD[idx_2++] = msg->data;
                Queue_put(my_queue_lcd, &msg-> elem);
            }

            if(posted & Event_Id_00){
            System_printf("Received a carriage return character printing to screen \n");
            GrRectFill(&sContext, &fill);
            System_printf("Drawing a string to the screen\n");
            GrStringDraw(&sContext, &shared_buffer_LCD, idx_2-1, 40, 40, 1);
            idx_2 = 0;
            count = 0;
            }

            else if(posted & Event_Id_01){
            System_printf("Received a space character echoing to terminal \n");
            UART_write(uart, &shared_buffer, idx);
            idx = 0;
            count = 0;
            }
        }
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
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    taskParams.instance->name = "display";
    Task_construct(&task1Struct, (Task_FuncPtr)displayFxn, &taskParams, NULL);

    Event_construct(&evtStruct, NULL);
    evtHandle = Event_handle(&evtStruct);

    //Semaphore setup
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    semParams.event = evtHandle;
    semParams.eventId = Event_Id_02;
    Semaphore_construct(&semStruct0, 0, &semParams);
    semHandle = Semaphore_handle(&semStruct0);

    int i;
    MsgObj *msg = &msg_mem;
    my_queue = Queue_create(NULL, NULL);
    my_queue_lcd = Queue_create(NULL, NULL);

    for (i = 0; i < NUMMSGS; msg++, i++) {
        Queue_put(my_queue_lcd, &msg->elem);
    }

    GateHwi_Params_init(&gHwiprms);
    gateHwi = GateHwi_create(&gHwiprms, NULL);

    if (gateHwi == NULL) {
        System_abort("Gate Hwi create failed");
    }

    //Call function to initialise UART
    openUART();

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
