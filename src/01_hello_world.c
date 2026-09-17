/*
 * Warmup - Hello World
 *
 * Question: What does C print?
 *
 * Answer: The program prints "My pid is X" TWICE, and X is the SAME
 * number both times. getpid() returns the process ID, and all threads
 * created within a single process (via pthread_create) share that same
 * process ID -- only the address space owner (the process) has a PID,
 * threads do not get their own. The child thread and the direct call
 * made by main() are therefore reporting the identical PID.
 *
 * The ORDER of the two lines is not guaranteed: main() never calls
 * pthread_join(), so there is a race between the child thread's printf
 * and main()'s own printf/return. In practice both usually print because
 * the direct call to identify(NULL) in main gives the child thread time
 * to be scheduled, but this is not guaranteed by the standard.
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void *identify(void *arg) {
    pid_t pid = getpid();
    printf("My pid is %d\n", pid);
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, &identify, NULL);
    identify(NULL);
    return 0;
}
