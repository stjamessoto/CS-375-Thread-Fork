/*
 * Follow-up to Problem 3.4: "How would you retrieve the return value
 * of worker? (e.g. 42)"
 *
 * Answer: pthread_join()'s second parameter is a `void **`. Pass the
 * address of a void* variable and, once the target thread has
 * terminated, that variable is filled in with whatever the thread
 * returned. Since we smuggled the integer 42 through a void* (not a
 * real pointer), we cast it back through intptr_t on the way out.
 *
 * Expected output:
 *   worker produced 42
 *   worker's return value was 42
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

void *worker(void *arg) {
    printf("worker produced 42\n");
    return (void *) 42;
}

int main() {
    pthread_t thread;
    void *retval;

    pthread_create(&thread, NULL, &worker, NULL);
    pthread_join(thread, &retval);

    int result = (int) (intptr_t) retval;
    printf("worker's return value was %d\n", result);
    return 0;
}
