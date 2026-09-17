/*
 * Problem 3.1 - Join (fixed)
 *
 * Question: How can we modify the code above to always print out
 * "HELPER" followed by "MAIN"?
 *
 * Answer: Replace pthread_yield() with pthread_join(thread, NULL).
 * pthread_join() blocks the calling (main) thread until the target
 * thread has fully terminated, guaranteeing "HELPER" is printed and
 * completed before main() prints "MAIN". This removes the race
 * condition entirely.
 */

#include <pthread.h>
#include <stdio.h>

void *helper(void *arg) {
    printf("HELPER\n");
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, &helper, NULL);
    pthread_join(thread, NULL);
    printf("MAIN\n");
    return 0;
}
