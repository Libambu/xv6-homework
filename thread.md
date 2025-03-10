# Homework: Threads and synchronize

Note: Your can use your Linux's gcc; you don't need the xv6 in this homework.

说明：
- 本实验在任何一台 Linux 上使用 gcc 即可完成，包括虚拟机中的 Linux ，无平台限制，不需要像其他一些实验一样需要 xv6 环境。
- 虚拟机可以使用VMware Workstation或Oracle VirtualBox，也可以使用Microsoft wsl(windows subsystem for linux)，注意本实验需要为虚拟机分配多于1个的CPU核，以期观察多核环境下的竞争和同步现象。

## Part One: Locking

In this assignment you will explore parallel programming with threads and locks using a hash table. You should do this homework on a real computer (not xv6, not qemu) that has multiple cores. Most recent laptops have multicore processors. 

Download [ph.c](ph.c) and compile it:

```
$ gcc -g -O2 ph.c -pthread
$ ./a.out 2
```

The 2 specifies the number of threads that execute put and get operations on the the hash table.
After running for a little while, the program will produce output similar to this:

```
1: put time = 0.003338
0: put time = 0.003389
0: get time = 7.684335
0: 17480 keys missing
1: get time = 7.684335
1: 17480 keys missing
completion time = 7.687856
```

Each thread runs in two phases. In the first phase each thread puts NKEYS/nthread keys into the hash table. In the second phase, each thread gets NKEYS from the hash table. The print statements tell you how long each phase took for each thread. The completion time at the bottom tells you the total runtime for the application. In the output above, the completion time for the application is about 7.7 seconds. Each thread computed for about 7.7 seconds (~0.0 for put + ~7.7 for get).

To see if using two threads improved performance, let's compare against a single thread:

```
$ ./a.out 1
0: put time = 0.004073
0: get time = 6.929189
0: 0 keys missing
completion time = 6.933433
```

The completion time for the 1 thread case (~7.0s) is slightly less than for the 2 thread case (~7.7s), but the two-thread case did twice as much total work during the get phase. Thus the two-thread case achieved nearly 2x parallel speedup for the get phase on two cores, which is very good.
When you run this application, you may see no parallelism if you are running on a machine with only one core or if the machine is busy running other applications.

Two points: 1) The completion time is about the same as for 2 threads, but this run did twice as many gets as with 2 threads; we are achieving good parallelism. 2) The output for 2 threads says that many keys are missing. In your runs, there may be more or fewer keys missing. If you run with 1 thread, there will never be any keys missing.

Why are there missing keys with 2 or more threads, but not with 1 thread? Identify a sequence of events that can lead to keys missing for 2 threads.

To avoid this sequence of events, insert lock and unlock statements in put and get so that the number of keys missing is always 0. The relevant pthread calls are (for more see the manual pages, man pthread):

```c
pthread_mutex_t lock;     // declare a lock
pthread_mutex_init(&lock, NULL);   // initialize the lock
pthread_mutex_lock(&lock);  // acquire lock
pthread_mutex_unlock(&lock);  // release lock
```

Test your code first with 1 thread, then test it with 2 threads. Is it correct (i.e. have you eliminated missing keys?)? Is the two-threaded version faster than the single-threaded version?

Modify your code so that get operations run in parallel while maintaining correctness. (Hint: are the locks in get necessary for correctness in this application?)

Modify your code so that some put operations run in parallel while maintaining correctness. (Hint: would a lock per bucket work?)

Submit: your modified ph.c

## Part Two: Barriers

In this assignment we will explore how to implement a barrier using condition variables provided by the pthread library. A barrier is a point in an application at which all threads must wait until all other threads reach that point too. Condition variables are a sequence coordination technique similar to xv6's sleep and wakeup. 

Download [barrier.c](barrier.c) and compile it:
```
$ gcc -g -O2 -pthread barrier.c
$ ./a.out 2
```

Assertion failed: (i == t), function thread, file barrier.c, line 55.
The 2 specifies the number of threads that synchronize on the barrier ( nthread in barrier.c). Each thread sits in a tight loop. In each loop iteration a thread calls barrier() and then sleeps for some random number of microseconds. The assert triggers, because one thread leaves the barrier before the other thread has reached the barrier. The desired behavior is that all threads should block until nthreads have called barrier.
Your goal is to achieve the desired behavior. In addition to the lock primitives that you have seen before, you will need the following new pthread primitives (see man pthreads for more detail):
```c
pthread_cond_wait(&cond, &mutex);  // go to sleep on cond, releasing lock mutex
pthread_cond_broadcast(&cond);     // wake up every thread sleeping on cond
```

pthread_cond_wait releases the mutex when called, and re-acquires the mutex before returning.
We have given you barrier_init(). Your job is to implement barrier() so that the panic won't occur. We've defined struct barrier for you; its fields are for your use.

There are two issues that complicate your task:

You have to deal with a succession of barrier calls, each of which we'll call a round. bstate.round records the current round. You should increase bstate.round when each round starts.
You have to handle the case in which one thread races around the loop before the others have exited the barrier. In particular, you are re-using bstate.nthread from one round to the next. Make sure that a thread that leaves the barrier and races around the loop doesn't increase bstate.nthread while a previous round is still using it.
Test your code with one, two, and more than two threads.

Submit: your modified barrier.c
