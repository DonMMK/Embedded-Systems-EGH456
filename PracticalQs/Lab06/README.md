## Pracical 06

### Task A
Queston: What is the time taken by task1 within its critical section each time it runs? (Try 3
passes).
Answer:

Queston: Are these approximately the same? If not, why not? (Observe the sequence of messages).  
Answer:

Queston: What is the unit of time that task times are being measured? (ticks = ?)    
Answer:

Queston: Which tasks are sharing the semaphore? (read the code to see where pend and post
are).  
Answer:

Queston: Which task pre-empts the low priority task while it is holding the semaphore? (read
the code).  
Answer:

Queston: Which task is potentially at risk for missing a critical deadline? (read lecture 10 and
the code).  
Answer:

### Task B
Queston: Does the time taken by task1 depend on the computation time for task3? Why? (note
this and refer to priority inversion problem description in lecture 10).  
Answer:

Queston: Where is the extra time being spent by task1? Doing what? (refer to priority inversion
problem description in lecture 10).  
Answer:

Queston: What is a solution to the priority inversion problem? (refer to priority inheritance
protocol description in lecture 10).  
Answer:

Queston: What are the different types of ‘gates’ in SYS/BIOS? (refer to the user manual for
SYS/BIOS).  
Answer:

Queston: Which gate supports a solution to this problem? (read about Gates in the SYS/BIOS
manual).  
Answer:

Queston:Was the time more with the semaphore than with the GateMutexPri? Why?  
Answer:

Queston: Which task inherits the priority?  
Answer:

Queston: Which task is the priority inherited from?  
Answer:

Queston: Why does task3 not pre-empt task2 when GateMutexPri is used as it did when a
semaphore was used?  
Answer:
