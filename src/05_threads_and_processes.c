/*
 * Problem 3.4 - Threads and Processes
 *
 * Question: What does C print? (Hint: there may be zero, one, or
 * multiple answers.)
 *
 * Answer:
 * fork() duplicates the ENTIRE address space, so after fork() the
 * parent and child processes each have their OWN independent copy of
 * the global variable `data` (unlike threads, which would share it).
 *
 *   - Child process (pid == 0): calls worker() once.
 *       data: 0 -> 1   prints "Data is 1"
 *
 *   - Parent process (else branch): calls worker() twice, joining
 *     between each call, so its own two calls are strictly ordered:
 *       data: 0 -> 1   prints "Data is 1"
 *       data: 1 -> 2   prints "Data is 2"
 *     then wait(&status) reaps the child.
 *
 * So three lines print in total:
 *     Data is 1      (child)
 *     Data is 1      (parent, 1st call)
 *     Data is 2      (parent, 2nd call)
 *
 * The parent's own two lines are guaranteed to appear in that relative
 * order (1 before 2), but the child process runs concurrently and
 * independently of the parent, so the child's single "Data is 1" line
 * can be interleaved anywhere among the parent's output -- there is
 * NOT one single guaranteed ordering for all three lines together.
 *
 * Follow-up question: How would you retrieve the return value of
 * worker() (e.g. "42")?
 *
 * Answer: pthread_join()'s second parameter is a `void **` -- pass the
 * address of a void* variable, and after the call it will hold
 * whatever the thread returned via `return`. See
 * 06_retrieve_return_value.c for a working, runnable demonstration
 * (cast back through intptr_t since the value 42 was smuggled through
 * a pointer-sized integer, not a real pointer).
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void *worker(void *arg) {
    int *data = (int *) arg;
    *data = *data + 1;
    printf("Data is %d\n", *data);
    return (void *) 42;
}

int data;

int main() {
    int status;
    data = 0;
    pthread_t thread;
    pid_t pid = fork();
    if (pid == 0) {
        pthread_create(&thread, NULL, &worker, &data);
        pthread_join(thread, NULL);
    } else {
        pthread_create(&thread, NULL, &worker, &data);
        pthread_join(thread, NULL);
        pthread_create(&thread, NULL, &worker, &data);
        pthread_join(thread, NULL);
        wait(&status);
    }
    return 0;
}
