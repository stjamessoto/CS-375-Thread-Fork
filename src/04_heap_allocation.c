/*
 * Problem 3.3 - Heap Allocation
 *
 * Question: What does C print?
 *
 * Answer: "I am the child"
 *
 * message is allocated on the HEAP with malloc(), and the heap is part
 * of the shared address space of the process, so both the main thread
 * and the child thread are reading/writing the very same buffer (no
 * copy is made when it's passed to pthread_create). main() first
 * writes "I am the parent" into it, then the child thread overwrites
 * it with "I am the child" via strcpy(). Because main() calls
 * pthread_join() before printing, it is guaranteed to see the child's
 * final overwrite, not its own original string.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *helper(void *arg) {
    char *message = (char *) arg;
    strcpy(message, "I am the child");
    return NULL;
}

int main() {
    char *message = malloc(100);
    strcpy(message, "I am the parent");
    pthread_t thread;
    pthread_create(&thread, NULL, &helper, message);
    pthread_join(thread, NULL);
    printf("%s\n", message);
    free(message);
    return 0;
}
