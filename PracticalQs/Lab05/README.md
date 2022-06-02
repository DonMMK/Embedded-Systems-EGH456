## Pracical 05

### Task A
Question: What is an “event”? What are the events in this example that the reader 
task waits
for?  
Answer: They are data structures that you can pass around your system, that indicate something has happened or some state has changed in for system Like a button press or character received at a serial port. The reader task is waiting for Event00 AND Event01 or Event02.  
An event is a form of system interrupt. In this example the reader pends on the event_id_02 OR (event_id_00 AND event_id_01).  
- event 02 is posted by the mailbox everytime the writer task posts a message to the mailbox


Question: What is “implicit” or “explicit” posting of events in the context of this 
program?  
Answer: 


Question: What does a semaphore “post” operation do?  
Answer: It posts to a semaphore, incrementing its value by one.  


Question: What is a “mailbox” and what do tasks use it for?  
Answer: Mailboxes provide a means of passing messages between tasks for data exchange or task synchronization.  


Question: Which operation is used when a message is placed by the writer?  
Answer: Mailbox post.  


Question: What is a timeout period used for, in the context of semaphores and 
events?  
Answer:


Question: When is the timeout period set to zero (BIOS NO WAIT) and why?  
Answer:

### Task B
Question: Do this using multiple threads in TI-RTOS. Explain your use of message queues and/or
semaphores.  
Answer:  
### Task C
