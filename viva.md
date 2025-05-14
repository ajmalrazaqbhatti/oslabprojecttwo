# NexOS - Operating System Concepts Implementation

## 1. Multitasking

NexOS demonstrates multitasking by allowing multiple applications to run simultaneously. The system maintains a process table and allocates resources to different tasks.

```c
// Process table implementation
PCB process_table[MAX_TASKS];
int process_count = 0;

// Example from main() - running clock in background while allowing other tasks
// Auto-start the clock in background mode
launch_task_background(2); // Index for clock
```

The OS allows users to launch different applications while keeping track of all running processes and enabling switching between them.

## 2. Context Switching

Context switching is demonstrated when minimizing and resuming processes, effectively saving and restoring process states.

```c
// Minimizing a process (saving context)
void minimize_process(int index) {
    if (process_table[index].is_active && !process_table[index].is_minimized) {
        printf("Minimizing process %s...\n", process_table[index].name);

        // Set the process as minimized
        if (sem_wait(process_semaphore) < 0) {
            perror("sem_wait failed");
            sleep(2);
            return;
        }
        process_table[index].is_minimized = 1;
        sem_post(process_semaphore);

        printf("Process minimized successfully.\n");
        sleep(1);
    }
}

// Resuming a process (restoring context)
void resume_process(int index) {
    if (process_table[index].is_active && process_table[index].is_minimized) {
        printf("Resuming process %s...\n", process_table[index].name);

        // Set the process as not minimized
        if (sem_wait(process_semaphore) < 0) {
            perror("sem_wait failed");
            sleep(2);
            return;
        }
        process_table[index].is_minimized = 0;
        sem_post(process_semaphore);

        // Execute the task again...
    }
}
```

## 3. Resource Allocation

The system manages CPU, RAM, and hard disk resources, allocating and freeing them as processes start and terminate.

```c
// Resource allocation
int allocate_resources(int ram_required, int hdd_required) {
    pthread_mutex_lock(&resource_mutex);

    if (hardware.available_ram >= ram_required &&
        hardware.available_hdd >= hdd_required &&
        hardware.available_cores > 0) {

        hardware.available_ram -= ram_required;
        hardware.available_hdd -= hdd_required;
        hardware.available_cores--;

        pthread_mutex_unlock(&resource_mutex);
        return 1; // Resources allocated successfully
    }

    pthread_mutex_unlock(&resource_mutex);
    return 0; // Not enough resources
}

// Resource deallocation
void free_resources(int index) {
    pthread_mutex_lock(&resource_mutex);

    hardware.available_ram += process_table[index].ram_required;
    hardware.available_hdd += process_table[index].hdd_required;
    hardware.available_cores++;

    pthread_mutex_unlock(&resource_mutex);
}
```

## 4. User Mode and Kernel Mode

NexOS implements a distinction between user mode and kernel mode, with certain operations restricted to kernel mode for system protection.

```c
// Global variable tracking current mode
int is_kernel_mode = 1; // 1 for kernel mode, 0 for user mode

// Mode switching function
void switch_mode() {
    is_kernel_mode = !is_kernel_mode;
    printf("Switched to %s mode.\n", is_kernel_mode ? "Kernel" : "User");
    sleep(1);
}

// Example of privilege check before performing a sensitive operation
// From task manager - terminating a process
if (!is_kernel_mode) {
    printf("ERROR: Cannot terminate processes in User Mode!\n");
    sleep(2);
    continue;
}
```

## 5. Process Creation

NexOS demonstrates process creation using both system() calls and fork()/exec() approaches:

```c
// Process creation using fork() and exec()
void launch_task_with_exec(int task_id) {
    // ... resource allocation and preparation ...

    // Use fork and exec to launch the task
    pid_t pid = fork();

    if (pid == -1) {
        // Fork failed
        perror("fork failed");
        // ... cleanup ...
    } else if (pid == 0) {
        // Child process
        // Clear the screen before launching the task
        system("clear");

        // Execute the task
        execl(available_tasks[task_id].path, available_tasks[task_id].path, NULL);

        // If execl fails
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        // Update the PID in the process table
        // ... wait for child process to complete ...
    }
}
```

## 6. Threads

NexOS implements a thread pool to handle processing tasks in parallel:

```c
// Thread management
pthread_t worker_threads[MAX_THREADS];
int thread_active[MAX_THREADS] = {0};
ThreadArgs thread_args[MAX_THREADS];

// Thread worker function
void* thread_worker(void* arg) {
    ThreadArgs* thread_args = (ThreadArgs*)arg;
    int thread_id = thread_args->thread_id;

    printf("Worker thread %d started\n", thread_id);

    while (1) {
        pthread_mutex_lock(&thread_mutex);

        // Wait for a process to be ready
        while (ml_queue.count[0] <= 0 && ml_queue.count[1] <= 0 && ml_queue.count[2] <= 0) {
            pthread_cond_wait(&process_ready_cond, &thread_mutex);
        }

        // Find a process to run (starting from the highest priority queue)
        PCB* process = NULL;
        int level = -1;

        for (int i = 0; i < MAX_LEVELS; i++) {
            if (ml_queue.count[i] > 0) {
                process = dequeue_process(i);
                level = i;
                break;
            }
        }

        pthread_mutex_unlock(&thread_mutex);

        if (process != NULL) {
            // Process the task...
        }
    }

    return NULL;
}

// Creating worker threads
void create_worker_threads() {
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].task_id = -1;

        if (pthread_create(&worker_threads[i], NULL, thread_worker, &thread_args[i]) != 0) {
            perror("Failed to create worker thread");
        } else {
            thread_active[i] = 1;
        }
    }
}
```

## 7. EXEC Commands

NexOS uses the exec family of functions to replace the current process image with a new one:

```c
// Using execl to run tasks
execl(available_tasks[task_id].path, available_tasks[task_id].path, NULL);

// The code also uses posix_spawn capability indicated by the header inclusion
#include <spawn.h>
```

## 8. Scheduling using Mutual Exclusion, Semaphore, and Condition Variables

### Mutex (Mutual Exclusion)

```c
// Mutex declarations
pthread_mutex_t resource_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

// Example of mutex use for resource allocation
pthread_mutex_lock(&resource_mutex);
// ... critical section - modify shared resources ...
pthread_mutex_unlock(&resource_mutex);
```

### Semaphores

```c
// Semaphore for process table access
sem_t *process_semaphore;

// Initializing semaphore
process_semaphore = sem_open("/process_sem", O_CREAT, 0644, 1);

// Example usage in process table modification
if (sem_wait(process_semaphore) < 0) {
    perror("sem_wait failed");
} else {
    process_table[index].is_active = 0;
    process_table[index].is_minimized = 0;
    process_count--;
    sem_post(process_semaphore);
}
```

### Condition Variables

```c
// Condition variable declarations
pthread_cond_t process_ready_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t resources_available_cond = PTHREAD_COND_INITIALIZER;

// Example of condition variable use in thread_worker
// Wait for a process to be ready
while (ml_queue.count[0] <= 0 && ml_queue.count[1] <= 0 && ml_queue.count[2] <= 0) {
    pthread_cond_wait(&process_ready_cond, &thread_mutex);
}

// Signaling when a process is added to the queue
pthread_cond_signal(&process_ready_cond);
```

## 9. Multilevel Queue Scheduling

NexOS implements a multilevel queue with different scheduling approaches:

```c
// Multilevel queue definition
typedef struct {
    PCB* queue[MAX_LEVELS][MAX_TASKS]; // Array of queues for different priority levels
    int front[MAX_LEVELS];
    int rear[MAX_LEVELS];
    int count[MAX_LEVELS];
    int time_quantum[MAX_LEVELS]; // Time quantum for each level (for RR)
} MultiLevelQueue;

// Queue initialization
void init_multilevel_queue() {
    for (int i = 0; i < MAX_LEVELS; i++) {
        ml_queue.front[i] = 0;
        ml_queue.rear[i] = -1;
        ml_queue.count[i] = 0;

        // Set time quantum based on level
        // Lower levels get higher time quantum
        ml_queue.time_quantum[i] = (i + 1) * 2;
    }
}

// Enqueueing processes based on priority
void enqueue_process(PCB* process) {
    // Determine which level to place the process based on priority
    int level;
    if (process->priority >= 3) {
        level = 0; // High priority
    } else if (process->priority >= 1) {
        level = 1; // Medium priority
    } else {
        level = 2; // Low priority
    }

    // Add the process to the queue
    ml_queue.rear[level] = (ml_queue.rear[level] + 1) % MAX_TASKS;
    ml_queue.queue[level][ml_queue.rear[level]] = process;
    ml_queue.count[level]++;
}

// Process selection from multilevel queue
// Find a process to run (starting from the highest priority queue)
PCB* process = NULL;
int level = -1;

for (int i = 0; i < MAX_LEVELS; i++) {
    if (ml_queue.count[i] > 0) {
        process = dequeue_process(i);
        level = i;
        break;
    }
}
```

The system supports multiple scheduling algorithms that can be selected at runtime:

```c
// Different scheduling algorithms
typedef enum {
    SCHEDULER_FCFS,        // First-Come-First-Served
    SCHEDULER_SJF,         // Shortest Job First
    SCHEDULER_PRIORITY,    // Priority Scheduling
    SCHEDULER_RR           // Round Robin
} SchedulerType;

// Switching between scheduling algorithms
void change_scheduler() {
    if (!is_kernel_mode) {
        printf("ERROR: Cannot change scheduler in User Mode!\n");
        sleep(2);
        return;
    }

    // ... display menu and get user choice ...

    switch (choice) {
        case 1:
            current_scheduler = SCHEDULER_FCFS;
            break;
        case 2:
            current_scheduler = SCHEDULER_SJF;
            break;
        case 3:
            current_scheduler = SCHEDULER_PRIORITY;
            break;
        case 4:
            current_scheduler = SCHEDULER_RR;
            break;
        case 0:
            return;
        default:
            printf("Invalid choice. Scheduler not changed.\n");
            sleep(1);
            return;
    }
}
```

Each level in the multilevel queue can be configured with a different scheduling approach (e.g., highest priority queue using priority scheduling, middle queue using round robin, and lowest queue using FCFS).
