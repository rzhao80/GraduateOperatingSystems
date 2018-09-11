## Sample Midterm Questions

1. How is a new process created? Select all that apply.

a. Via fork (If we want to create a process that is an exact replica of the calling process)
b. Via exec
c. Via fork followed by exec (If we want to create a process that is not an exact replica of the calling process)
d. Via exec followed by fork
e. Via exec or fork followed by exec
__f. Via fork or fork followed by exec__
g. None of the above
h. All of the above

2. Is there a benefit of multithreading on 1 CPU?

__a. Yes__
b. No

Give 1 reason to support your answer.

Should be obvious. Multithreading one cpu uses concurrency to hide latency from blocking operations such as I/O

3. Critical Section
In the (pseudo) code segments for the producer code and consumer code, mark and explain all the lines where there are errors.

Global Section
```
int in, out, buffer[BUFFERSIZE];
mutex_t m;
cond_var_t not_empty, not_full;
```

Producer Code
```
while (more_to_produce) {
    mutex_lock(&m);
    if (out == (in + 1) % BUFFERSIZE)) // buffer full ERROR: This should be a while loop, not an if statement
        condition_wait(&not_full); // ERROR: there's no mutex specified
    add_item(buffer[in]); // add item
    in = (in + 1) % BUFFERSIZE 
    cond_broadcast(&not_empty); // WARNING: Although technicaly this works, there's only one item added so a signal can be sent instead
    // ERROR: Never unlocks the mutex!
} // end producer code
```

Consumer Code
```
while (more_to_consume) {
    mutex_lock(&m);
    if (out == in) // buffer empty ERROR: This should be a while loop, not an if statement
        condition_wait(&not_empty); // ERROR: there's no mutex specified
    remove_item(out);
    out = (out + 1) % BUFFERSIZE; 
    condition_signal(&not_empty); // ERROR: should specify &not_full instead of &not_empty
    // ERROR: Never unlocks the mutex!
} // end consumer code
```

4. Calendar Critical Section
A shared calendar supports three types of operations for reservations:

read
cancel
enter

Requests for cancellations should have priority above reads, who in turn have priority over new updates.

In pseudocode, write the critical section enter/exit code for the read operation.

5. Signals
If the kernel cannot see user-level signal masks, then how is a signal delivered to a user-level thread (where the signal can be handled)?

Answer: The signal is sent to the threading library handler on the user level. This handler then takes the appropriate next steps depending on the user threads and their masks.

6. Solaris Papers
The implementation of Solaris threads described in the paper "Beyond Multiprocessing: Multithreading the Sun OS Kernel", describes four key data structures used by the OS to support threads.

For each of these data structures, list at least two elements they must contain:

__Process__
The process scheduling state, Process structuring information, Interprocess communication information, Process Privileges, Process State, Process Number, Program Counter, CPU Registers, CPU Scheduling Information, Memory Management Information, Accounting Information, I/O Status Information

__LWP__
contains the per-LWP data such as the process-control-block (pcb) for storing user-level processor registers, system call arguments, signal handling masks, resource usage information, and profiling pointers. It also contains pointers to the associated kernel thread and process structures.

__Kernel-threads:__
kernel registers, scheduling class, dispatch queue links, and pointers to the stack and the associated LWP, process, and CPU structures.

__CPU__
has pointers to the currently executing thread, the idle thread for that CPU, and current dispatching and interrupt handling information. There is a substructure of the cpu structure that can be architecture dependent, but the main body is intended to be applicable to most multiprocessing architectures.


7. Pipeline Model
An image web server has three stages with average execution times as follows:

Stage 1: read and parse request (10ms)
Stage 2: read and process image (30ms)
Stage 3: send image (20ms)
You have been asked to build a multi-threaded implementation of this server using the pipeline model. Using a pipeline model, answer the following questions:

How many threads will you allocate to each pipeline stage?
What is the expected execution time for 100 requests (in sec)?
What is the average throughput of the system in Question 2 (in req/sec)? Assume there are infinite processing resources (CPU's, memory, etc.).

Answer:

- Threads should be allocated as follows:
    1. Stage 1 should have 1 thread
        A. This 1 thread will parse a new request every 10ms
    2. Stage 2 should have 3 threads
        A. The requests parsed by Stage 1 get passed to the threads in Stage 2. Each thread picks up a request and needs 30ms to process the image. Hence, we need 3 threads in order to pick up a new request as soon as Stage 1 passes it.
    3. Stage 3 should have 2 threads.
        A. This is because Stage 2 will process an image and pass a request every 10ms (once the pipeline is filled). In this way, each Stage 3 thread will pick up a request and send an image in 20ms. Once the pipeline is filled, Stage 3 will be able to pick up a request and send the image every 10ms.
- The first request will take 60ms. The last stage will continue delivering the remaining 99 requests at 10ms intervals. So, the total is 60 + (99 * 10ms) = 1050ms = 1.05s
- 100 req / 1.05 sec = 95.2 req/s


8. Performance Observations
Here is a graph from the paper "Flash: An Efficient and Portable Web Server", that compares the performance of Flash with other web servers.

For data sets where the data set size is less than 100 MB why does...

Flash perform worse than SPED?
Flash perform better than MP?

Solution:

1. In both cases the dataset will likely fit in cache, but Flash incurs an overhead on each request because Flash must first check for cache residency. In the SPED model, this check is not performed.
2. When data is present in the cache, there is no need for slow disk I/O operations. Adding threads or processes just adds context switching overheads, but there is no benefit of “hiding I/O latency”.
