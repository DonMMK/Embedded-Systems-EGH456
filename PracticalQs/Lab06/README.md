## Pracical 06

### Task A
Queston: What is the time taken by task1 within its critical section each time it runs? (Try 3
passes).  
Answer: 844 ticks, 23,23 afterwards

Queston: Are these approximately the same? If not, why not? (Observe the sequence of messages).  
Answer: No. Task 2 acquires the semaphore from the pend from Task 1 in the first iteration.  
Task 3 takes priority over the Task 2 and executes. Afterwards task 2 finishes execution. Then Task 1 resumes. 

Queston: What is the unit of time that task times are being measured? (ticks = ?)    
Answer: Ticks are measured in milli seconds, 1 ms for each tick.  

Queston: Which tasks are sharing the semaphore? (read the code to see where pend and post
are).  
Answer: Task 1 and Task 2 share the same semaphore.  

Queston: Which task pre-empts the low priority task while it is holding the semaphore? (read
the code).  
Answer: Task 3 which has a mid level priority is able to pre-empt the Task 2 which has a lower priority.

Queston: Which task is potentially at risk for missing a critical deadline? (read lecture 10 and
the code).  
Answer: Task 1 would be at risk. This would be due to the priority inversion.

### Task B
Queston: Does the time taken by task1 depend on the computation time for task3? Why? (note
this and refer to priority inversion problem description in lecture 10).  
Answer: Yes, because the Task 2 holds onto resource that Task 1 needs to continue execution. Task 3 preempts execution over Task 2 where Task 3 has to finish first for the rest of the Tasks to finish execution

Queston: Where is the extra time being spent by task1? Doing what? (refer to priority inversion
problem description in lecture 10).  
Answer: Waiting for resource held by task 2 to release via semaphore post.  

Queston: What is a solution to the priority inversion problem? (refer to priority inheritance
protocol description in lecture 10).  
Answer: The solutions available are Priority Inheritance Protocol, Priority Ceiling Protocol and Random Boosting.
For the purposes of this question - the solution explored is Priority Inheritance Protocol.  
- Priorities of tasks are dynamically changed.  
- A task in a critical section inherits the priority of the highest task pending on that critical region.  
- Priority inheritance does not prevent deadlock.  

Queston: What are the different types of ‘gates’ in SYS/BIOS? (refer to the user manual for
SYS/BIOS).  
Answer: As per the user manual the different gates are,  
1. gateHWI  
2. gateSWI  
3. gateTask  
4. gateMutex  
5. GateMutexPri  

Queston: Which gate supports a solution to this problem? (read about Gates in the SYS/BIOS
manual).  
Answer: The GateMutexPri

Queston:Was the time more with the semaphore than with the GateMutexPri? Why?  
Answer:

Queston: Which task inherits the priority?  
Answer:

Queston: Which task is the priority inherited from?  
Answer:

Queston: Why does task3 not pre-empt task2 when GateMutexPri is used as it did when a
semaphore was used?  
Answer:
 
