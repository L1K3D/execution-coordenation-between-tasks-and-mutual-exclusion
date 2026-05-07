# Execution Coordination Between Tasks and Mutual Exclusion

## 📋 Overview

This project implements and demonstrates different strategies to solve the **race conditions** problem in concurrent programming. The focus is on ensuring **mutual exclusion** when multiple threads/processes simultaneously access shared resources.

The project includes a problem example and five progressive solutions, ranging from simple methods to modern approaches with atomic operations.

---

## 🎯 Purpose

When multiple threads execute simultaneously and share data, a **race condition** may occur – a situation where the result depends on the non-deterministic execution order of threads. This project demonstrates:

- How race conditions occur
- Different techniques to prevent race conditions
- Advantages and disadvantages of each approach
- Fundamental concepts of synchronization in operating systems

---

## ⚠️ The Problem: Race Condition

### Demonstration

**File:** [problem-process-competition.c](c-archives-and-outputs/problem-process-competition.c)

```c
int balance = 0;  // Variable shared across multiple threads

void *deposit(void *arg)
{
    for (int i = 0; i < 100000; i++)
    {
        balance = balance + 1;  // NON-ATOMIC operation
    }
    return NULL;
}
```

### What Happens?

The operation `balance = balance + 1` is not atomic. Internally, it involves **3 steps**:
1. **Read** the current value of `balance` (e.g., 100)
2. **Increment** the value in memory (e.g., 101)
3. **Write** the new value back (e.g., balance = 101)

If two threads execute these steps simultaneously:
- Thread A reads balance = 100
- Thread B reads balance = 100 (before A writes)
- Thread A writes balance = 101
- Thread B writes balance = 101 ← **We lose one increment!**

### Expected vs. Actual Result

- **Expected:** 200,000 (2 threads × 100,000 increments)
- **Actual:** A random number < 200,000 (e.g., 187,532)

---

## 🔧 Implemented Solutions

### Solution 1: Interrupt Inhibition (Simulated)

**File:** [solution-inhibit-interruptions.c](c-archives-and-outputs/solution-inhibit-interruptions.c)

#### Concept
Disables hardware interrupts during the critical section, preventing the kernel from performing context switching.

#### Implementation
```c
void *deposit(void *arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        disable_interrupts();    // Disable interrupts
        balance = balance + 1;   // Critical section
        enable_interrupts();     // Re-enable interrupts
    }
    return NULL;
}
```

#### Advantages
✅ Simple and effective on single-core systems  
✅ No synchronization overhead

#### Disadvantages
❌ **Does not work on multi-core:** Another core can access simultaneously  
❌ Risk of deadlock: If thread enters infinite loop with interrupts disabled, OS freezes  
❌ Only possible at kernel level

#### Use Case
Only in kernel code on single-core operating systems (rare today).

---

### Solution 2: Busy Variable (Busy Waiting)

**File:** [solution-busy-variable.c](c-archives-and-outputs/solution-busy-variable.c)

#### Concept
Uses a global `busy` variable as a software "lock". Threads check if the critical section is busy before entering.

#### Implementation
```c
int busy = 0;  // 0 = free, 1 = occupied

void *deposit(void *arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        while (busy);      // Wait while busy == 1
        busy = 1;          // Mark as occupied
        
        balance = balance + 1;  // Critical section
        
        busy = 0;          // Release the lock
    }
    return NULL;
}
```

#### Why It Fails
The "test and set" operation is not atomic:
1. Thread A checks: `busy == 0` ✓
2. Thread B checks: `busy == 0` ✓ (before A writes)
3. Thread A writes: `busy = 1`
4. Thread B writes: `busy = 1` ← **Both enter the critical section!**

#### Additional Problem: Busy Waiting
The `while (busy)` loop consumes CPU continuously, wasting processor cycles.

#### Result
❌ **Mutual exclusion FAILS** - Race condition still occurs

---

### Solution 3: Turn Alternation (Turn)

**File:** [solution-turn-alternation.c](c-archives-and-outputs/solution-turn-alternation.c)

#### Concept
Uses a `turn` variable to control which thread can enter the critical section.

#### Implementation
```c
int turn = 0;  // 0 = thread 0, 1 = thread 1

void *deposit(void *arg)
{
    long id = (long)arg;
    
    for (int i = 0; i < ITERATIONS; i++)
    {
        while (turn != id);         // Wait for your turn
        balance = balance + 1;      // Critical section
        turn = 1 - id;              // Pass turn to the other
    }
    return NULL;
}
```

#### Advantages
✅ It works! Mutual exclusion is guaranteed  
✅ No special hardware operations  
✅ Simple to understand

#### Disadvantages
❌ **Violates independence:** If one thread no longer needs to enter, the other stays blocked waiting  
❌ Both forced to alternate, even if one doesn't need to  
❌ Poor efficiency in asymmetric scenarios

#### Result
✅ **Mutual exclusion succeeds** - `balance == 2,000,000`  
❌ **But with design problems**

---

### Solution 4: Peterson's Algorithm

**File:** [solution-peterson.c](c-archives-and-outputs/solution-peterson.c)

#### Concept
Combines intention flags with turn alternation. Each thread indicates it wants to enter ("flag") but politely yields the turn ("turn").

#### Implementation
```c
int flag[2] = {0, 0};  // Intention of each thread
int turn = 0;          // Who has the right to the turn

void *deposit(void *arg)
{
    long id = (long)arg;
    long other = 1 - id;
    
    for (int i = 0; i < ITERATIONS; i++)
    {
        flag[id] = 1;              // Wants to enter
        turn = other;              // Yields turn to the other
        
        // Wait: other does NOT want to enter OR it's my turn
        while (flag[other] && turn == other);
        
        balance = balance + 1;     // Critical section
        flag[id] = 0;              // Exit
    }
    return NULL;
}
```

#### Advantages
✅ Guarantees mutual exclusion  
✅ Prevents deadlock  
✅ Respects independence: if one doesn't need to, the other enters  
✅ Pure software solution (no special hardware)  
✅ Works for 2 threads

#### Disadvantages
❌ Uses busy waiting (consumes CPU)  
❌ Complex to generalize for > 2 threads  
❌ Algorithm difficult to formally prove  

#### Result
✅ **Mutual exclusion succeeds**  
✅ **Efficient design for 2 threads**

---

### Solution 5: Atomic Operations (Test-and-Set)

**File:** [solution-atomic.c](c-archives-and-outputs/solution-atomic.c)

#### Concept
Uses atomic hardware instructions (like XCHG on x86) provided by C's `stdatomic.h` library. The reading and setting of the lock occur in a single CPU cycle, guaranteeing atomicity.

#### Implementation
```c
atomic_flag lock = ATOMIC_FLAG_INIT;  // Atomic lock

void *deposit(void *arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        // Atomically test and set: returns previous value, sets to true
        while (atomic_flag_test_and_set(&lock));
        
        balance = balance + 1;     // Critical section
        
        atomic_flag_clear(&lock);  // Release lock
    }
    return NULL;
}
```

#### How It Works
- **`atomic_flag_test_and_set()`** in hardware:
  1. Reads the flag value
  2. Sets flag = 1
  3. Returns the previous value
  - All in **a single indivisible cycle**

#### Advantages
✅ Hardware guarantee: no concurrency during test-and-set  
✅ Works on multi-core  
✅ Portable standard (C11)  
✅ Foundation for mutexes and other primitives

#### Disadvantages
❌ Uses busy waiting (consumes CPU)  
❌ Requires hardware support  
❌ Less efficient than OS mutexes (which use sleep)

#### Result
✅ **Mutual exclusion succeeds**  
✅ **Works on any modern architecture**

---

## 📊 Solutions Comparison

| Aspect | Sol. 1 | Sol. 2 | Sol. 3 | Sol. 4 | Sol. 5 |
|--------|--------|--------|--------|--------|--------|
| **Name** | Inhibit Interrupts | Busy Variable | Turn | Peterson | Atomic |
| **Works** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **Multi-core** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **Independence** | N/A | N/A | ❌ | ✅ | ✅ |
| **Busy Wait** | ✅ | ✅ | ✅ | ✅ | ✅ |
| **Hardware** | Yes | No | No | No | Yes |
| **Kernel Only** | Yes | No | No | No | No |
| **Practical** | ❌ | ❌ | ⚠️ | ✅ | ✅ |

---

## 🚀 How to Compile and Execute

### Requirements
- GCC or Clang
- POSIX threads (pthread.h)
- Operating system: Windows, Linux, or macOS

### Individual Compilation

```bash
# Problem
gcc -Wall -Wextra -g3 problem-process-competition.c -o problem-process-competition.exe -lpthread

# Solution 1
gcc -Wall -Wextra -g3 solution-inhibit-interruptions.c -o solution-inhibit-interruptions.exe -lpthread

# Solution 2
gcc -Wall -Wextra -g3 solution-busy-variable.c -o solution-busy-variable.exe -lpthread

# Solution 3
gcc -Wall -Wextra -g3 solution-turn-alternation.c -o solution-turn-alternation.exe -lpthread

# Solution 4
gcc -Wall -Wextra -g3 solution-peterson.c -o solution-peterson.exe -lpthread

# Solution 5
gcc -Wall -Wextra -g3 solution-atomic.c -o solution-atomic.exe -lpthread
```

### Execution

```bash
# Run any program
./problem-process-competition.exe
./solution-atomic.exe
# etc...
```

### Expected Output

**Problem:**
```
Final balance expected: 200000
Final balance obtained: 187532  (varies each execution)
```

**Solutions 1 and 2:**
```
Expected Balance: (varies)
Final Balance:    (incorrect value - race condition)
```

**Solutions 3, 4, and 5:**
```
Expected Balance: (2,000,000 or 1,000,000)
Final Balance:    (correct value)
```

---

## 📚 Key Concepts

### Critical Section
Code section where multiple threads access shared data. Must be protected against simultaneous access.

### Race Condition
Unpredictable behavior caused by non-deterministic timing of threads. Result depends on execution order.

### Mutual Exclusion
Guarantee that only one thread can execute the critical section at a time.

### Atomicity
Operation that cannot be interrupted in the middle. Executes "all or nothing".

### Busy Waiting
Thread stays in a loop checking a condition, consuming CPU.

### Lock (Locking)
Mechanism to control access to the critical section (variable or operation).

### Deadlock
Two or more threads remain blocked waiting for each other eternally.

---

## 🔍 Functional Analysis

### Why Does the Problem Occur?

The operation `balance = balance + 1` in C is translated to **multiple machine instructions**:

```assembly
MOV EAX, [balance]    ; Read balance into EAX register
INC EAX               ; Increment EAX
MOV [balance], EAX    ; Write EAX back to balance
```

If a thread is interrupted **between** these instructions, another thread can execute the same, causing overwrite.

### Why Do Solutions Work?

- **Sol. 1:** Prevents context switching (single-core)
- **Sol. 3 & 4:** Coordinates execution order via algorithms
- **Sol. 5:** Uses atomic `test-and-set` instruction (hardware)

---

## 💡 Practical Applications

These concepts are used in:
- **Operating Systems:** Scheduler, memory management
- **Databases:** Transactions, locks
- **Web Servers:** Worker pool threads
- **Multithreading Applications:** Safe data sharing
- **Languages:** Java (synchronized), Python (GIL), Rust (Mutex)

---

## 🎓 Conclusion

This project demonstrates that synchronization is fundamental in concurrent programming. Although software solutions (Peterson, Atomic) work, in real systems we use **OS primitives** like mutexes and semaphores, which combine these techniques with efficient sleep to save CPU.

**Key Learnings:**
1. Race conditions are real and dangerous
2. Multiple strategies exist, with different trade-offs
3. Hardware (atomicity) is essential for efficient synchronization
4. Software algorithms (Peterson) are elegant but still consume CPU
5. OS mutexes are the practical modern solution

---

## 📝 Project Structure

```
execution-coordenation-between-tasks-and-mutual-exclusion/
├── README.md                          # This file
└── c-archives-and-outputs/
    ├── problem-process-competition.c   # Problem demonstration
    ├── solution-inhibit-interruptions.c # Solution 1
    ├── solution-busy-variable.c        # Solution 2
    ├── solution-turn-alternation.c     # Solution 3
    ├── solution-peterson.c             # Solution 4
    ├── solution-atomic.c               # Solution 5
    └── output/                         # Compiled files (.exe)
```

---

**Author:** Educational Project  
**Topics:** Concurrency, Synchronization, Mutual Exclusion, Race Conditions  
**Language:** C (POSIX Threads)  
**Level:** Intermediate/Advanced