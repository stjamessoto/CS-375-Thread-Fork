# CS 375 — Assignment 3: Threads and Context Switching

Runnable C code and answers for the "Threads and Context Switching"
discussion worksheet (pthreads, `fork()`, stack vs. heap allocation, and
the Pintos `switch_threads` / `intr_entry` / `intr_exit` context-switch
assembly).

## What's in here

```
src/
  01_hello_world.c              Warmup 1.1 — getpid() shared across threads
  02_join_before.c              Problem 3.1 — the racy version (no join)
  02_join_after.c               Problem 3.1 — fixed with pthread_join()
  03_stack_allocation.c         Problem 3.2 — passing a stack address safely
  04_heap_allocation.c          Problem 3.3 — shared heap memory across threads
  05_threads_and_processes.c    Problem 3.4 — fork() + threads, separate address spaces
  06_retrieve_return_value.c    Problem 3.4 follow-up — capturing a thread's return value
```

Each file starts with a comment block explaining the question, the answer,
and *why* — that's the write-up for that problem. The two worksheet
questions that have no code (3.5 Context Switching and 3.6 Reflections on
Threads) are answered directly below, in [Conceptual answers](#conceptual-answers).

## Quick start — build and run everything

Requires `gcc` and a POSIX system (Linux/macOS/WSL, or this Codespace).
No external dependencies beyond `pthread`, which ships with every standard
C library.

**Build and run one file:**
```bash
gcc -Wall -pthread src/01_hello_world.c -o hello
./hello
```

**Build and run all of them at once:**
```bash
cd src
for f in *.c; do
  name="${f%.c}"
  gcc -Wall -pthread "$f" -o "/tmp/$name" && echo "=== $name ===" && /tmp/"$name"
done
```

### Why does the output change between runs?

`01_hello_world.c` and `02_join_before.c` are *intentionally racy* — that's
the point of the exercise. Run them several times in a row and watch the
output change (or not) between runs. `02_join_after.c` fixes the race with
`pthread_join()` and will print the same thing every time.

## Taking screenshots for the Word report

Run each command below **in this repo's terminal** (in this Codespace, or
your own machine after cloning the repo) and screenshot the terminal
showing both the command and its output — that's the "running and testing
your code" evidence the assignment asks for. A one-line description for
each is included so you can paste it next to the screenshot in Word.

```bash
# 1. Warmup — Hello World: same PID printed twice, order not guaranteed
gcc -Wall -pthread src/01_hello_world.c -o /tmp/01 && /tmp/01 && /tmp/01 && /tmp/01

# 2a. Join — racy version: run several times, note it's not always the same
gcc -Wall -pthread src/02_join_before.c -o /tmp/02a && /tmp/02a && /tmp/02a && /tmp/02a

# 2b. Join — fixed with pthread_join(): always HELPER then MAIN
gcc -Wall -pthread src/02_join_after.c -o /tmp/02b && /tmp/02b && /tmp/02b

# 3. Stack Allocation: prints "i is 2"
gcc -Wall -pthread src/03_stack_allocation.c -o /tmp/03 && /tmp/03

# 4. Heap Allocation: prints "I am the child"
gcc -Wall -pthread src/04_heap_allocation.c -o /tmp/04 && /tmp/04

# 5. Threads and Processes: fork() gives each process its own copy of `data`
gcc -Wall -pthread src/05_threads_and_processes.c -o /tmp/05 && /tmp/05 && /tmp/05

# 6. Retrieving a thread's return value via pthread_join()
gcc -Wall -pthread src/06_retrieve_return_value.c -o /tmp/06 && /tmp/06
```

That's 6 screenshots covering every code problem (2a/2b share one problem,
so you can put them side by side). Run each 2–3 times if you want to show
the non-determinism in 1 and 2a explicitly.

## Conceptual answers

### 3.5 Context Switching (Pintos)

**How many stacks are involved in a context switch? Identify the purpose
of each stack.**

Two stacks: the currently running thread's stack (`CUR`) and the next
thread's stack (`NEXT`). Every Pintos thread has its own dedicated kernel
stack. `switch_threads` pushes `CUR`'s callee-saved registers (`%ebx`,
`%ebp`, `%esi`, `%edi`) onto `CUR`'s own stack and records the resulting
stack pointer in `CUR`'s `struct thread`, so `CUR` can resume later exactly
where it left off. It then loads `%esp` from `NEXT`'s saved stack pointer
and pops the registers that were saved there the last time `NEXT` itself
was switched out, restoring `NEXT`'s context.

**Inside `intr_exit`, why does the order of the 5 pop instructions
matter?**

The five pops (`popal`, `popl %gs`, `popl %fs`, `popl %es`, `popl %ds`)
must exactly reverse the order of the pushes done on entry in `intr_entry`
(`pushl %ds`, `%es`, `%fs`, `%gs`, then `pushal`). The stack is LIFO — it
has no notion of which value belongs to which register, only position.
`pushal` happened last on entry, so it must be undone first (`popal`) on
exit, then `%gs`, `%fs`, `%es`, `%ds` in that order. Popping out of order
would load the wrong saved value into the wrong register (e.g. a saved
`%ds` ending up in `%gs`), silently corrupting the resumed thread's state.

### 3.6 Reflections on Threads

**Two programming examples where multithreading beats a single-threaded
solution:**

1. **Network/web servers** handling many simultaneous client connections —
   a thread per connection (or a thread pool) lets the server keep serving
   other clients while one thread blocks on a slow socket read or disk
   I/O, improving throughput and responsiveness.
2. **Parallelizable computation on multi-core CPUs** — e.g. splitting a
   large matrix multiplication, image/video filter, or simulation into
   chunks run on separate threads across cores, cutting wall-clock time
   roughly in proportion to the number of cores used.

**Two differences between user-level and kernel-level threads; when is
one better?**

- **Who manages them:** user-level threads are scheduled entirely by a
  user-space library, invisible to the kernel; kernel-level threads are
  created and scheduled directly by the OS, which tracks each individually.
- **Blocking behavior:** if a user-level thread makes a blocking syscall,
  the *whole process* blocks (the kernel only sees one underlying kernel
  thread). With kernel-level threads, the kernel can schedule a different
  thread from the same process while one is blocked. (Kernel threads can
  also run in true parallel across cores; user-level threads on a single
  kernel thread cannot.)

User-level threads win when you want extremely cheap, lightweight
creation/switching for many short-lived, mostly non-blocking, cooperative
tasks. Kernel-level threads win when you need real parallelism across
cores, or your threads do blocking I/O and the others must keep running.

**Actions a kernel takes to context-switch between kernel-level threads:**

1. Something triggers the kernel — a timer interrupt, syscall, voluntary
   yield, or the running thread blocking.
2. The kernel saves the running thread's CPU state (registers, instruction
   pointer, stack pointer, flags) onto its kernel stack / TCB.
3. The scheduler picks the next thread to run from the ready queue.
4. Thread bookkeeping is updated: old thread → ready/blocked, new thread →
   running.
5. The kernel switches the stack pointer to the new thread's kernel stack.
6. The kernel restores the new thread's previously saved registers.
7. If switching between *different processes*, memory-management state
   (e.g. `CR3`/page tables) is also swapped; skipped for threads of the
   *same* process, since they already share one address space.
8. Execution resumes in the new thread exactly where it left off.

**What resources are used when a thread is created? How do they differ
from those used when a process is created?**

Creating a **thread** only needs a new **stack** and a small **thread
control block** (registers, thread ID, scheduling info); everything else
(address space, file descriptors, heap, globals) is reused from the
process. Creating a **process** (`fork()`) is far more expensive: a brand
new address space (page tables, copy-on-write of the parent's memory), a
new process control block, its own copy of the file descriptor table, its
own heap, and one initial thread + stack. Thread creation shares almost
everything; process creation duplicates a nearly complete, independent
copy of the process's resources.

## Submission checklist

Per the assignment instructions:

- [x] **Source code files, uploaded individually (not zipped):** everything
      in [`src/`](src/) — 7 `.c` files, each self-contained and compiling on
      its own.
- [ ] **Word document with screenshots of running/testing your code, with
      descriptions:** use the commands above under
      [Taking screenshots](#taking-screenshots-for-the-word-report) — one
      screenshot + one sentence per file.
- [ ] **Upload the report document** (the Word doc above).
- [ ] **Paste your GitHub repo URL** in the submission text box/comments.
- [ ] **Submit before the deadline.**
