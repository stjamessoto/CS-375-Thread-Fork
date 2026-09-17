/*
 * Problem 3.2 - Stack Allocation
 *
 * (Note: the worksheet's helper signature has a typo, "void *helper(void
 * arg)"; it must be "void *helper(void *arg)" to compile. Fixed below.)
 *
 * Question: What does C print?
 *
 * Answer: "i is 2"
 *
 * main() passes &i (the address of its own local/stack variable) to
 * the child thread. Because main() calls pthread_join() before reading
 * i, main's stack frame is guaranteed to still be alive and main is
 * guaranteed to observe the write the child thread made to *num (i.e.
 * to i) before printf runs. Passing the address of a caller's local
 * variable is safe here specifically because the caller blocks
 * (joins) until the callee is done using it.
 */

#include <pthread.h>
#include <stdio.h>

void *helper(void *arg) {
    int *num = (int *) arg;
    *num = 2;
    return NULL;
}

int main() {
    int i = 0;
    pthread_t thread;
    pthread_create(&thread, NULL, &helper, &i);
    pthread_join(thread, NULL);
    printf("i is %d\n", i);
    return 0;
}
