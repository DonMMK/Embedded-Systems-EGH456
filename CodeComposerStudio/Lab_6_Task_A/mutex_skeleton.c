
/*
 * Written for Lab 6 by Prof Vinod Chandran (QUT)
 * This example project is not part of the TI example projects.
 * It was written starting from the mutex example project and uses an edited version of the
 * same configuration.
 */

/***   CHANGE HERE TO USE EITHER A SEMAPHORE OR A GATEMUTEXPRI  ******/
// uncomment appropriately to use a GateMutexPri or a semaphore
//#define USEGATEMUTEXPRI  1
#define USESEMAPHORE   1
/*********************************************************************/
/* Build and run the program
 * From the pull down menu Window -> Show View -> Other -> RTOS Object View (ROV)
 * Go to SysMin and observe the contents of the output buffer as you have done
 * for previous Lab exercises.
 */


/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/gates/GateMutexPri.h>  /* added */

/* Example/Board Header files */
#include "Board.h"

#define TASKSTACKSIZE   512


/* changes here to add third task */
Void task1Fxn(UArg arg0, UArg arg1);
Void task2Fxn(UArg arg0, UArg arg1);
Void task3Fxn(UArg arg0, UArg arg1);

Int resource = 0;
UInt32 sleepTickCount;

Task_Struct task1Struct, task2Struct, task3Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE], task3Stack[TASKSTACKSIZE];


#ifdef USEGATEMUTEXPRI
// if using GateMutex uncomment such lines and comment out the semaphore ones
UInt gateKey;
GateMutexPri_Handle gampHandle;
#endif
#ifdef USESEMAPHORE
Semaphore_Struct semStruct;
Semaphore_Handle semHandle;
#endif


/*
 *  ======== main ========
 */
Int main()
{
    /* Construct BIOS objects */
    Task_Params taskParams;
#ifdef USESEMAPHORE
    Semaphore_Params semParams;
#endif
#ifdef USEGATEMUTEXPRI
    GateMutexPri_Params gampParams;
#endif


    /* Call board init functions */
    Board_initGeneral();

    /* Construct Task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    taskParams.priority = 3;
    Task_construct(&task1Struct, (Task_FuncPtr)task1Fxn, &taskParams, NULL);

    taskParams.stack = &task2Stack;
    taskParams.priority = 1;
    Task_construct(&task2Struct, (Task_FuncPtr)task2Fxn, &taskParams, NULL);

    taskParams.stack = &task3Stack;
    taskParams.priority = 2;
    Task_construct(&task3Struct, (Task_FuncPtr)task3Fxn, &taskParams, NULL);

#ifdef USESEMAPHORE
    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
     Semaphore_Params_init(&semParams);
     Semaphore_construct(&semStruct, 1, &semParams);
     /* Obtain instance handle */
     semHandle = Semaphore_handle(&semStruct);
#endif
#ifdef USEGATEMUTEXPRI
    /* Construct a GateMutexPri object to use as a resource lock */

    if (gampHandle == NULL) {
    System_abort("Gate Mutex Pri create failed");
    }
#endif


    BIOS_start();    /* Does not return */
    return(0);
}

/*
 *  ======== task1Fxn ======== HIGH PRIORITY TASK SHARES RESOURCE ===========
 */
Void task1Fxn(UArg arg0, UArg arg1)
{
    UInt32 time1, time2, t2count, loopcount;
    loopcount = 0;
    for (;;) {
    	loopcount++;
        System_printf("Running task1 (high priority) function \n");
        if (loopcount == 1) Task_sleep(5);
        // this delay beofre it accesses the resource will ensure that the low priority task
        // will get the semaphore (or GateMutexPri) in the FIRST cycle
        // NOTE: In other cycles the sequence of execution may be different
        // and there may be no priority inversion

        /* get time when semaphore pend is attempted */
           time1 = Clock_getTicks();
        /* Get access to resource */
#ifdef USESEMAPHORE
        Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
#endif
#ifdef USEGATEMUTEXPRI
       //enter gate mutex pri and get key
#endif
        System_printf("task1 (high priority) in critical sectio\n");
        /* do work on locked resource */
        for (t2count = 0; t2count < 100000; t2count++) {
                resource += 1;
                }
        /* unlock resource */
        System_printf("task1 (high priority) leaving critical sectio\n");
#ifdef USESEMAPHORE
        Semaphore_post(semHandle);
#endif
#ifdef USEGATEMUTEXPRI
        //leave gate mutex pri and return key
#endif

        /* get time when semaphore is released */
        time2 = Clock_getTicks();

        System_printf("Task 1 (high priority) takes %d ticks\n",(time2-time1));
        // This time will be the semaphore acquisition time plus computation time.
        // Computation time is kept constant.
        // breakpoint here will allow examining the completion time for task 1
        Task_sleep(50);

        if (loopcount > 2) {
              System_printf("Calling BIOS_exit from task1\n");
              BIOS_exit(0);
          }
    }
}

/*
 *  ======== task2Fxn ========  LOW PRIORITY TASK  SHARE RESOURCE ====================
 */
Void task2Fxn(UArg arg0, UArg arg1)
{
	 UInt32 t2count;

   for (;;) {
       System_printf("Running task2 (low priority) \n");

       /* Get access to resource */
#ifdef USESEMAPHORE
        Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
#endif
#ifdef USEGATEMUTEXPRI
        //enter gate mutex pri and get key
#endif
       System_printf("task2 (low priority) entered critical sectio\n");

       /* do work on locked resource */
       for (t2count = 0; t2count < 40000; t2count++) {
       resource += 1;
       }
       /* unlock resource */
       System_printf("task2 (low priority) leaving critical section\n");
#ifdef USESEMAPHORE
        Semaphore_post(semHandle);
#endif
#ifdef USEGATEMUTEXPRI
        //leave gate mutex pri and return key
#endif

       Task_sleep(50);

   }
}


/*
 *  ======== task3Fxn ========    MEDIUM PRIORITY TASK DOES NOT USE RESOURCE ====================
 */
Void task3Fxn(UArg arg0, UArg arg1)
{
	 UInt32 icount1, icount2;

   for (;;) {

	   /* a small delay to ensure that in the first cycle the low priority task
	    * gets to run first */
       Task_sleep(2);

       System_printf("Running task3 (medium priority) function and doing work\n");
       /* do work  */
       /* changing the amount of processing here will change the completion time
        * of task1 in the first cycle where priority inversition is simulated
        * if you are using a semaphore.
        * Change the number of iterations of the outer loop by a factor such as 2 or 10 and observe
        * the change in the time that task1 (not this task) is in its critical section.
        * NOTE: For a factor of 10, you may have to wait for the output in the SysMin output buffer
        * Observe that changing the computations in this task
        * will change the time taken by the high priority task
        * because the high priority task is kept waiting for that period
        * if you use a semaphore
        * Thus the priority of the 'high priority task' has been inverted
        * compared to the low priority one.
      */
       for(icount1 = 0; icount1 < 100000; icount1++) {
         	   for(icount2 = 0; icount2 < 100; icount2++);
            }


      System_printf("task3 (medium priority) now goes to sleep\n");
      Task_sleep(50);
      System_printf("task3 (medium priority) awakes from sleep\n");
   }
}

