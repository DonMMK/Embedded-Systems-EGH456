## Pracical 02

### Task A
Question:  
How will the program get out of the while(1) loop? What does it execute outside this
loop?  

Answer: Exits the while loop due to the interupt. It does the code in the interupt hanlder outside the loop.  


### Task B
Question:  
If an interrupt occurs before the service for the previous one is completed, what happens?  

Answer: When a second hardware interrupt occurs while and initial one is still being handled, the second interrupt will be put on hold until the first one has finished. Source - stackoverflow  

Question:  
How many interrupts and priority levels does the Nested Vectored Interrupt Controller
(NVIC) support?  

Answer: The NVIC supports: An implementation-defined number of interrupts, in the range 1-240 interrupts. A programmable priority level of 0-255 for each interrupt. A higher level corresponds to a lower priority, so level 0 is the highest interrupt priority. Source - developer.arm  


### Task C
Question:  
Describe how a watchdog timer would be useful in a real-time system.  

Answer: A watch dog timer can be used in real-time systems that have a task that is repeated and that task will have a deadline that it is meant to be executed.  
For example in a reading the speed in a fast moving car and updating the speed on the speedometer within a given time limit must be done constantly so that the driver knows what speed they are going at.  
This can be context of real-time specific in that the timer can be used in both "soft" and "hard" real-time systems as explained in the lecture slides. Source - lecture  

### Task D
Question:  
What is a real-time clock?  

Answer: Is a computer clock in the form of an integrated circuit that is specifically used to keep time. It is able to function even when the computer is powered down through a battery or independantly from the system's main power.  

Question:  
What are thread and handler modes of operation?  

Answer: Thread mode is the normal mode that programs run in. Thread mode can be privileged or unprivileged software execution. Handler mode is the mode that exceptions are handled in. Source - keil.com  

Question:  
What does the Wake-up Interrupt Controller (WIC) do?  

Answer: The Wakeup Interrupt Controller (WIC) is a peripheral that can detect an interrupt and wake the processor from deep sleep mode. The WIC is enabled only when the system is in deep sleep mode. The WIC is not programmable, and does not have any registers or user interface. It operates entirely from hardware signals. Source - developer.arm  


### Task E
