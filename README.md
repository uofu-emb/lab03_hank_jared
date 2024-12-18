# Lab 3
In this lab we explore threads, locks, and how they all interact with each other.

## Activity 0
The two execution contexts are the main_thread and side_thread. The entry point for the main thread is in the main function. The entry point for the side thread is in the main thread. They both use the counter variable. Semaphore is named semaphore. As for behavior we don't know which thread will execute first, but they both print out and update the counter. Because there is no lock on them, race conditions may occur. After running the program, there is indeed unpredicted behavior with the counter variable.

## Activity 1
The shared resource is not in a critical section. Changes were made to rectify this. It is not starving the system, but only the parts that use the counter should be locked down in a critical section.

## Activity 2
Tests are written for the thread code taking and releasing the semaphore.

## Activity 4
Case 1 deadlock can be found in a commit. Tested with printing out statuses of threads.

## Activity 5
Test written showing the case 2 deadlock.