/*
 * Problem 3.1 - Join (before fix)
 *
 * Question: What does C print? (Hint: there may be zero, one, or
 * multiple answers.)
 *
 * Answer: This is a race condition, so there are MULTIPLE possible
 * outputs depending on how the OS scheduler interleaves the main
 * thread and the child thread:
 *
 *   1) HELPER
 *      MAIN
 *   2) MAIN
 *      HELPER
 *   3) MAIN            (main finishes and the process exits via
 *                        return 0 before the child thread is ever
 *                        scheduled, so "HELPER" never prints)
 *
 * pthread_yield() only asks the calling thread to give up the CPU; it
 * does NOT guarantee the helper thread runs, and it does not block
 * until the helper thread finishes. So the relative order of "HELPER"
 * and "MAIN" -- and even whether "HELPER" appears at all -- is
 * undefined without further synchronization.
 */

#define _GNU_SOURCE /* pthread_yield() is a non-standard GNU extension;
                       this exposes its prototype from pthread.h. */
#include <pthread.h>
#include <stdio.h>

void *helper(void *arg) {
    printf("HELPER\n");
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, &helper, NULL);
    pthread_yield();
    printf("MAIN\n");
    return 0;
}
