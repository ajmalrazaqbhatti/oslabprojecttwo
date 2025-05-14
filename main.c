#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <spawn.h>

// ##########################################
// OS CONFIGURATION
// ##########################################
#define MAX_TASKS 20
#define OS_NAME "NexOS"
#define MAX_PATH_LENGTH 100
#define TASK_NAME_LENGTH 50
#define MAX_THREADS 5
#define MAX_LEVELS 3 

// ##########################################
// CPU SCHEDULER TYPES
// ##########################################
typedef enum {
    SCHEDULER_FCFS,        // First-Come-First-Served
    SCHEDULER_SJF,         // Shortest Job First
    SCHEDULER_PRIORITY,    // Priority Scheduling
    SCHEDULER_RR           // Round Robin
} SchedulerType;

// ##########################################
// DATA STRUCTURES
// ##########################################
// Structure to hold hardware specifications
typedef struct {
    int ram_gb;
    int hdd_gb;
    int cpu_cores;
    int available_ram;
    int available_hdd;
    int available_cores;
} HardwareResources;

// Process Control Block
typedef struct {
    int pid;
    char name[TASK_NAME_LENGTH];
    int ram_required;
    int hdd_required;
    int is_active;
    int is_minimized;
    int priority;
    time_t start_time;
    char task_path[MAX_PATH_LENGTH];
} PCB;

// Structure for thread arguments
typedef struct {
    int task_id;
    int thread_id;
} ThreadArgs;

// Structure for multilevel queue scheduling
typedef struct {
    PCB* queue[MAX_LEVELS][MAX_TASKS]; // Array of queues for different priority levels
    int front[MAX_LEVELS];
    int rear[MAX_LEVELS];
    int count[MAX_LEVELS];
    int time_quantum[MAX_LEVELS]; // Time quantum for each level (for RR)
} MultiLevelQueue;

// ##########################################
// GLOBAL VARIABLES
// ##########################################
HardwareResources hardware;
PCB process_table[MAX_TASKS];
int process_count = 0;
int is_kernel_mode = 1; // 1 for kernel mode, 0 for user mode
SchedulerType current_scheduler = SCHEDULER_FCFS; // Default scheduler

// ##########################################
// SYNCHRONIZATION PRIMITIVES
// ##########################################
pthread_mutex_t resource_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *process_semaphore;

// NexOS Thread Management
pthread_t worker_threads[MAX_THREADS];
int thread_active[MAX_THREADS] = {0};
ThreadArgs thread_args[MAX_THREADS];

// NexOS Synchronization Primitives
pthread_cond_t process_ready_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t resources_available_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

// NexOS Process Scheduling Queue
MultiLevelQueue ml_queue;

// ##########################################
// FUNCTION DECLARATIONS
// ##########################################
void boot_sequence();
void initialize_hardware();
void display_main_menu();
void display_applications_menu();
void display_task_manager();
void launch_task(int task_id);
void launch_task_background(int task_id); // New function to launch tasks in background
int allocate_resources(int ram_required, int hdd_required);
void free_resources(int process_id);
void switch_mode();
void shutdown_system();
void list_running_processes();
void terminate_process(int index);
void minimize_process(int index);
void resume_process(int index);
void send_interrupt(int index, int signal_type);
void initialize_process_table();
int is_application_running(const char* app_name); // New function declaration
void handle_child_exit(int signum __attribute__((unused))); // New function declaration
void change_scheduler(); // New function declaration
const char* get_scheduler_name(SchedulerType scheduler); // New function declaration
void init_multilevel_queue();
void enqueue_process(PCB* process);
PCB* dequeue_process(int level);
void* thread_worker(void* arg);
void create_worker_threads();
void cleanup_worker_threads();
void launch_task_with_exec(int task_id);

// ##########################################
// TASK DEFINITIONS
// ##########################################
typedef struct {
    char name[TASK_NAME_LENGTH];
    char path[MAX_PATH_LENGTH];
    int ram_required;
    int hdd_required;
    int priority;
} Task;

Task available_tasks[] = {
    {"Notepad", "./tasks/notepad.sh", 256, 10, 2},
    {"Calculator", "./tasks/calculator.sh", 64, 2, 3},
    {"Clock", "./tasks/clock.sh", 64, 2, 3},
    {"Prime Checker", "./tasks/primechecker.sh", 64, 1, 2},
    {"Unit Converter", "./tasks/unitconverter.sh", 64, 2, 1},
    {"Calendar", "./tasks/calendar.sh", 128, 10, 2},
    {"Number Sorter", "./tasks/sorter.sh", 128, 2, 1},
    {"Text Reverser", "./tasks/reverser.sh", 64, 1, 2},
    {"Game - Minesweeper", "./tasks/minesweeper.sh", 256, 20, 0},
    {"Factorial Calculator", "./tasks/factorial.sh", 64, 1, 2},
    {"BMI Calculator", "./tasks/bmicalc.sh", 96, 2, 2},
    {"Temperature Converter", "./tasks/tempconverter.sh", 64, 2, 3},
    {"Password Generator", "./tasks/passwordgen.sh", 64, 2, 1},
    {"File Manager", "./tasks/filemanager.sh", 128, 5, 2}
};

int num_available_tasks = sizeof(available_tasks) / sizeof(Task);

// ##########################################
// MAIN FUNCTION
// ##########################################
int main() {
    // Initialize semaphore
    process_semaphore = sem_open("/process_sem", O_CREAT, 0644, 1);
    if (process_semaphore == SEM_FAILED) {
        perror("Failed to create semaphore");
        return EXIT_FAILURE;
    }
    
    // Set up signal handler for child processes
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_child_exit;
    sigaction(SIGCHLD, &sa, NULL);
    
    // Initialize the process table
    initialize_process_table();
    
    // Initialize the multilevel queue
    init_multilevel_queue();
    
    // Create worker threads
    create_worker_threads();
    
    // Start the OS
    boot_sequence();
    initialize_hardware();
    
    // Auto-start the clock in background mode
    launch_task_background(2); // Index for clock
    
    int choice = 0;
    int submenu_choice = 0;
    
    // ##########################################
    // MAIN OS LOOP
    // ##########################################
    while (1) {
        display_main_menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n'); 
        
        if (choice == 0) {
            shutdown_system();
            break;
        } else if (choice == 1) {
            // Applications menu
            display_applications_menu();
            printf("Select an application (0 to go back): ");
            scanf("%d", &submenu_choice);
            while (getchar() != '\n'); 
            
            if (submenu_choice > 0 && submenu_choice <= num_available_tasks) {
                // Use exec-based launcher instead of system-based one
                launch_task_with_exec(submenu_choice - 1);
            }
        } else if (choice == 2) {
            // Task Manager menu
            int task_action = 0;
            do {
                display_task_manager();
                printf("Enter action (0 to go back): ");
                scanf("%d", &task_action);
                while (getchar() != '\n'); 
                
                if (task_action == 1) {
                    // Terminate a process
                    if (!is_kernel_mode) {
                        printf("ERROR: Cannot terminate processes in User Mode!\n");
                        sleep(2);
                        continue;
                    }
                    
                    int proc_id;
                    printf("Enter process ID to terminate: ");
                    scanf("%d", &proc_id);
                    while (getchar() != '\n'); 
                    
                    for (int i = 0; i < MAX_TASKS; i++) {
                        if (process_table[i].pid == proc_id && process_table[i].is_active) {
                            terminate_process(i);
                            break;
                        }
                    }
                } else if (task_action == 2) {
                    // Minimize a process
                    int proc_id;
                    printf("Enter process ID to minimize: ");
                    scanf("%d", &proc_id);
                    while (getchar() != '\n'); 
                    
                    for (int i = 0; i < MAX_TASKS; i++) {
                        if (process_table[i].pid == proc_id && process_table[i].is_active) {
                            minimize_process(i);
                            break;
                        }
                    }
                } else if (task_action == 3) {
                    // Resume a minimized process
                    int proc_id;
                    printf("Enter process ID to resume: ");
                    scanf("%d", &proc_id);
                    while (getchar() != '\n'); 
                    
                    for (int i = 0; i < MAX_TASKS; i++) {
                        if (process_table[i].pid == proc_id && process_table[i].is_active && process_table[i].is_minimized) {
                            resume_process(i);
                            break;
                        }
                    }
                } else if (task_action == 4) {
                    // Send interrupt to a process
                    if (!is_kernel_mode) {
                        printf("ERROR: Cannot send interrupts in User Mode!\n");
                        sleep(2);
                        continue;
                    }
                    
                    int proc_id, signal_type;
                    printf("Enter process ID: ");
                    scanf("%d", &proc_id);
                    while (getchar() != '\n'); 
                    printf("Enter signal type (1-SIGSTOP, 2-SIGCONT, 3-SIGTERM): ");
                    scanf("%d", &signal_type);
                    while (getchar() != '\n'); 
                    
                    for (int i = 0; i < MAX_TASKS; i++) {
                        if (process_table[i].pid == proc_id && process_table[i].is_active) {
                            send_interrupt(i, signal_type);
                            break;
                        }
                    }
                }
            } while (task_action != 0);
        } else if (choice == 3) {
            switch_mode();
        } else if (choice == 4) {
            change_scheduler();
        } else {
            printf("Invalid choice. Please try again.\n");
            sleep(1);
        }
    }
    
    // ##########################################
    // CLEANUP SECTION
    // ##########################################
    // Clean up worker threads before exiting
    cleanup_worker_threads();
    
    // Clean up
    sem_close(process_semaphore);
    sem_unlink("/process_sem");
    
    // Destroy mutex and condition variables
    pthread_mutex_destroy(&resource_mutex);
    pthread_mutex_destroy(&thread_mutex);
    pthread_cond_destroy(&process_ready_cond);
    pthread_cond_destroy(&resources_available_cond);
    
    return 0;
}

void initialize_process_table() {
    for (int i = 0; i < MAX_TASKS; i++) {
        process_table[i].pid = -1;
        process_table[i].is_active = 0;
        process_table[i].is_minimized = 0;
        strcpy(process_table[i].name, "");
    }
}

void boot_sequence() {
    system("clear");
    
    
    printf("\n\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║ ███╗   ██╗███████╗██╗  ██╗ ██████╗ ███████╗           ║\n");
    printf("║ ████╗  ██║██╔════╝╚██╗██╔╝██╔═══██╗██╔════╝           ║\n");
    printf("║ ██╔██╗ ██║█████╗   ╚███╔╝ ██║   ██║███████╗           ║\n");
    printf("║ ██║╚██╗██║██╔══╝   ██╔██╗ ██║   ██║╚════██║           ║\n");
    printf("║ ██║ ╚████║███████╗██╔╝ ██╗╚██████╔╝███████║           ║\n");
    printf("║ ╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝           ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("         Operating System Simulator v1.0\n\n");
    
    sleep(2);
    
    // Boot animation
    printf("┌─────────────────────────────────────────────────┐\n");
    printf("│ System Initialization                           │\n");
    printf("└─────────────────────────────────────────────────┘\n");
    
    const char* boot_messages[] = {
        "Initializing kernel...",
        "Loading system modules...",
        "Checking hardware configuration...",
        "Setting up process management...",
        "Configuring memory management...",
        "Starting system services...",
        "Initializing user interface...",
        "Loading application registry..."
    };
    
    int num_messages = sizeof(boot_messages) / sizeof(boot_messages[0]);
    
    for (int i = 0; i < num_messages; i++) {
        printf("  %s", boot_messages[i]);
        fflush(stdout);
        
        // Animated dots
        for (int j = 0; j < 3; j++) {
            usleep(200000);  
            printf(".");
            fflush(stdout);
        }
        
        usleep(300000);  
        printf(" [DONE]\n");
        usleep(250000);  
    }
    
    printf("\n");
    printf("┌─────────────────────────────────────────────────────┐\n");
    printf("│ √ %s is now ready!                               │\n", OS_NAME);
    printf("└─────────────────────────────────────────────────────┘\n\n");
    
    sleep(1);
}

void initialize_hardware() {
    printf("Hardware Configuration\n");
    printf("---------------------\n");
    
    printf("Enter available RAM (GB): ");
    scanf("%d", &hardware.ram_gb);
    while (getchar() != '\n'); 
    
    printf("Enter hard disk space (GB): ");
    scanf("%d", &hardware.hdd_gb);
    while (getchar() != '\n'); 
    
    printf("Enter number of CPU cores: ");
    scanf("%d", &hardware.cpu_cores);
    while (getchar() != '\n'); 
    
    // Initialize available resources
    hardware.available_ram = hardware.ram_gb * 1024; // Convert to MB
    hardware.available_hdd = hardware.hdd_gb;
    hardware.available_cores = hardware.cpu_cores;
    
    printf("\nSystem initialized with %d GB RAM, %d GB HDD, and %d CPU cores.\n", 
           hardware.ram_gb, hardware.hdd_gb, hardware.cpu_cores);
    sleep(2);
}

void display_main_menu() {
    system("clear");
    
    // ASCII art header
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║ ███╗   ██╗███████╗██╗  ██╗ ██████╗ ███████╗           ║\n");
    printf("║ ████╗  ██║██╔════╝╚██╗██╔╝██╔═══██╗██╔════╝           ║\n");
    printf("║ ██╔██╗ ██║█████╗   ╚███╔╝ ██║   ██║███████╗           ║\n");
    printf("║ ██║╚██╗██║██╔══╝   ██╔██╗ ██║   ██║╚════██║           ║\n");
    printf("║ ██║ ╚████║███████╗██╔╝ ██╗╚██████╔╝███████║           ║\n");
    printf("║ ╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝           ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    // System status info
    printf("\n");
    printf("┌─────────────────────────────────────────────────────┐\n");
    printf("│ %-15s %-35s │\n", "MODE:", is_kernel_mode ? "[K] Kernel Mode" : "[U] User Mode");
    printf("│ %-15s %-35s │\n", "SCHEDULER:", get_scheduler_name(current_scheduler));
    printf("├─────────────────────────────────────────────────────┤\n");
    printf("│ %-15s %d/%d MB                       │\n", "RAM:", hardware.available_ram, hardware.ram_gb * 1024);
    printf("│ %-15s %d/%d GB                          │\n", "STORAGE:", hardware.available_hdd, hardware.hdd_gb);
    printf("│ %-15s %d/%d                                 │\n", "CPU CORES:", hardware.available_cores, hardware.cpu_cores);
    printf("└─────────────────────────────────────────────────────┘\n");
    
    // Menu options
    printf("\n");
    printf("┌─────────────────── MAIN MENU ───────────────────────┐\n");
    printf("│                                                     │\n");
    printf("│  [1]  Launch Applications                           │\n");
    printf("│  [2]  Task Manager                                  │\n");
    printf("│  [3]  Switch Mode (Current: %-20s   │\n", is_kernel_mode ? "Kernel)" : "User)  "); 
    printf("│  [4]  Change CPU Scheduler                          │\n");
    printf("│  [0]  Shutdown System                               │\n");
    printf("│                                                     │\n");
    printf("└─────────────────────────────────────────────────────┘\n");
}

void display_applications_menu() {
    system("clear");
    
    // Header
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║             APPLICATION LAUNCHER                      ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    printf("\n");
    printf("┌─────────────────────────────────────────────────────┐\n");
    printf("│ Current Mode: %-37s │\n", is_kernel_mode ? "[K] Kernel Mode" : "[U] User Mode");
    printf("└─────────────────────────────────────────────────────┘\n");
    
    printf("\n");
    printf("┌─────────────────────────────────────────────────────┐\n");
    
    for (int i = 0; i < num_available_tasks; i++) {
        
        printf("│  [%2d] %c %-20s RAM: %4d MB   HDD: %2d GB  │\n", 
               i + 1, available_tasks[i].name, 
               available_tasks[i].ram_required, 
               available_tasks[i].hdd_required);
        
        if (i < num_available_tasks - 1) {
            printf("├─────────────────────────────────────────────────────┤\n");
        }
    }
    
    printf("└─────────────────────────────────────────────────────┘\n");
    printf("\n");
    printf("┌─────────────────────────────────────────────────────┐\n");
    printf("│  [0] Back to Main Menu                              │\n");
    printf("└─────────────────────────────────────────────────────┘\n");
}

void display_task_manager() {
    system("clear");
    
    // Header
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║                  TASK MANAGER                         ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    printf("\n");
    printf("┌─────────────────────────────────────────────────────┐\n");
    printf("│ Mode: %-10s           Scheduler: %-12s │\n", 
           is_kernel_mode ? "[K] Kernel" : "[U] User", 
           get_scheduler_name(current_scheduler));
    printf("└─────────────────────────────────────────────────────┘\n");
    
    // Display running processes
    printf("\n");
    printf("┌─────────────────── RUNNING PROCESSES ───────────────────┐\n");
    printf("│ %-5s │ %-20s │ %-8s │ %-8s │ %-8s │\n", 
           "PID", "NAME", "RAM (MB)", "HDD (GB)", "STATUS");
    printf("├───────┼──────────────────────┼──────────┼──────────┼──────────┤\n");
    
    
    int active_count = 0;
    
    for (int i = 0; i < MAX_TASKS; i++) {
        if (process_table[i].is_active) {
            active_count++;
            char status[20] = "[A] Active";
            
            if (process_table[i].is_minimized) {
                strcpy(status, "[M] Minimized");
            }
            
            printf("│ %-5d │ %-20s │ %-8d │ %-8d │ %-8s │\n", 
                   process_table[i].pid, 
                   process_table[i].name, 
                   process_table[i].ram_required, 
                   process_table[i].hdd_required,
                   status);
            
            if (i < MAX_TASKS - 1 && active_count < process_count) {
                printf("├───────┼──────────────────────┼──────────┼──────────┼──────────┤\n");
            }
        }
    }
    printf("└───────┴──────────────────────┴──────────┴──────────┴──────────┘\n");
    
    if (active_count == 0) {
        printf("│                   No active processes.                    │\n");
        printf("└─────────────────────────────────────────────────────────┘\n");
    }
    
    // Display available actions
    printf("\n");
    printf("┌─────────────── TASK MANAGER ACTIONS ─────────────────┐\n");
    
    if (is_kernel_mode) {
        printf("│  [1] Terminate a Process                              │\n");
        printf("├─────────────────────────────────────────────────────┤\n");
    }
    
    printf("│  [2] Minimize a Process                               │\n");
    printf("├─────────────────────────────────────────────────────┤\n");
    printf("│  [3] Resume a Process                                 │\n");
    
    if (is_kernel_mode) {
        printf("├─────────────────────────────────────────────────────┤\n");
        printf("│  [4] Send Interrupt to a Process                    │\n");
    }
    
    printf("├─────────────────────────────────────────────────────┤\n");
    printf("│  [0] Back to Main Menu                              │\n");
    printf("└─────────────────────────────────────────────────────┘\n");
}

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

void free_resources(int index) {
    pthread_mutex_lock(&resource_mutex);
    
    hardware.available_ram += process_table[index].ram_required;
    hardware.available_hdd += process_table[index].hdd_required;
    hardware.available_cores++;
    
    pthread_mutex_unlock(&resource_mutex);
}

// Check if an application is already running
int is_application_running(const char* app_name) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (process_table[i].is_active && strcmp(process_table[i].name, app_name) == 0) {
            return 1; // Application is already running
        }
    }
    return 0; // Application is not running
}

void launch_task(int task_id) {
    // Check if the application is already running
    if (is_application_running(available_tasks[task_id].name)) {
        // If it's already running but minimized, we can resume it
        for (int i = 0; i < MAX_TASKS; i++) {
            if (process_table[i].is_active && 
                strcmp(process_table[i].name, available_tasks[task_id].name) == 0 &&
                process_table[i].is_minimized) {
                printf("Resuming %s...\n", available_tasks[task_id].name);
                resume_process(i);
                return;
            }
        }
        
        // If it's active and not minimized, we can't start another instance
        printf("ERROR: %s is already running!\n", available_tasks[task_id].name);
        sleep(2);
        return;
    }
    
    // Check if we have available slots
    if (process_count >= MAX_TASKS) {
        printf("ERROR: Maximum number of processes reached!\n");
        sleep(2);
        return;
    }
    
    int ram_required = available_tasks[task_id].ram_required;
    int hdd_required = available_tasks[task_id].hdd_required;
    
    // Try to allocate resources
    if (!allocate_resources(ram_required, hdd_required)) {
        printf("ERROR: Not enough system resources to start %s!\n", 
               available_tasks[task_id].name);
        sleep(2);
        return;
    }
    
    // Check if the task is executable
    if (access(available_tasks[task_id].path, X_OK) != 0) {
        printf("ERROR: %s is not executable!\n", available_tasks[task_id].name);
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sleep(2);
        return;
    }
    
    // Reserve a slot in the process table before executing the task
    int index = -1;
    if (sem_wait(process_semaphore) < 0) {
        perror("sem_wait failed");
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sleep(2);
        return;
    }
    
    // Find an empty slot
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!process_table[i].is_active) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        // No empty slot found
        printf("ERROR: No empty slot in process table!\n");
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sem_post(process_semaphore);
        sleep(2);
        return;
    }
    
    // Set up information in the process table
    process_table[index].pid = getpid(); // Use our own PID for now
    process_table[index].is_active = 1;
    process_table[index].is_minimized = 0;
    process_table[index].ram_required = ram_required;
    process_table[index].hdd_required = hdd_required;
    process_table[index].priority = available_tasks[task_id].priority;
    process_table[index].start_time = time(NULL);
    strcpy(process_table[index].name, available_tasks[task_id].name);
    strcpy(process_table[index].task_path, available_tasks[task_id].path);
    
    process_count++;
    
    // Here we could add scheduler-specific logic
    // For example, with Priority scheduling, we might want to prioritize this task
    // based on its priority value
    if (current_scheduler == SCHEDULER_PRIORITY) {
        printf("Using Priority scheduling for this task (priority: %d).\n", 
               available_tasks[task_id].priority);
        sleep(1);
    }
    // For SJF, we might want to show estimated completion time
    else if (current_scheduler == SCHEDULER_SJF) {
        printf("Using Shortest Job First scheduling. Task size: %d MB.\n", 
               available_tasks[task_id].ram_required);
        sleep(1);
    }
    // For Round Robin, we might want to mention the time quantum
    else if (current_scheduler == SCHEDULER_RR) {
        printf("Using Round Robin scheduling with default time quantum.\n");
        sleep(1);
    }

    printf("Starting %s...\n", available_tasks[task_id].name);
    sem_post(process_semaphore);
    
    // Clear the screen before launching the task
    system("clear");
    
    // Execute the task directly in the current terminal
    int status;
    pid_t result = system(available_tasks[task_id].path);
    
    if (WIFEXITED(result)) {
        status = WEXITSTATUS(result);
        
        // Check the exit status to see if we need to minimize instead of close
        if (status == 10) {
            // Application requested to be minimized
            if (sem_wait(process_semaphore) < 0) {
                perror("sem_wait failed");
            } else {
                process_table[index].is_minimized = 1;
                sem_post(process_semaphore);
                printf("%s was minimized. You can resume it later.\n", available_tasks[task_id].name);
                sleep(2);
            }
        } else {
            // Application exited normally, free resources
            if (sem_wait(process_semaphore) < 0) {
                perror("sem_wait failed");
            } else {
                process_table[index].is_active = 0;
                process_table[index].is_minimized = 0;
                process_count--;
                sem_post(process_semaphore);
            }
            
            // Free resources allocated to this process
            free_resources(index);
            
            printf("%s was closed.\n", available_tasks[task_id].name);
            sleep(2);
        }
    } else {
        printf("ERROR: Failed to execute %s!\n", available_tasks[task_id].name);
        
        // Free resources and update process table
        if (sem_wait(process_semaphore) < 0) {
            perror("sem_wait failed");
        } else {
            process_table[index].is_active = 0;
            process_table[index].is_minimized = 0;
            process_count--;
            sem_post(process_semaphore);
        }
        
        // Free resources allocated to this process
        free_resources(index);
        
        sleep(2);
    }
    
    // Clear the screen after the task finishes
    system("clear");
}

void launch_task_background(int task_id) {
    // Check if the application is already running
    if (is_application_running(available_tasks[task_id].name)) {
        // If it's already running, we don't need to start it again
        return;
    }
    
    // Check if we have available slots
    if (process_count >= MAX_TASKS) {
        printf("ERROR: Maximum number of processes reached!\n");
        sleep(1);
        return;
    }
    
    int ram_required = available_tasks[task_id].ram_required;
    int hdd_required = available_tasks[task_id].hdd_required;
    
    // Try to allocate resources
    if (!allocate_resources(ram_required, hdd_required)) {
        printf("ERROR: Not enough system resources to start %s!\n", 
               available_tasks[task_id].name);
        sleep(1);
        return;
    }
    
    // Check if the task is executable
    if (access(available_tasks[task_id].path, X_OK) != 0) {
        printf("ERROR: %s is not executable!\n", available_tasks[task_id].name);
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sleep(1);
        return;
    }
    
    // Reserve a slot in the process table before executing the task
    int index = -1;
    if (sem_wait(process_semaphore) < 0) {
        perror("sem_wait failed");
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sleep(1);
        return;
    }
    
    // Find an empty slot
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!process_table[i].is_active) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        // No empty slot found
        printf("ERROR: No empty slot in process table!\n");
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sem_post(process_semaphore);
        sleep(1);
        return;
    }
    
    // Set up information in the process table
    process_table[index].pid = getpid(); // Use our own PID for now
    process_table[index].is_active = 1;
    process_table[index].is_minimized = 1; // Start minimized
    process_table[index].ram_required = ram_required;
    process_table[index].hdd_required = hdd_required;
    process_table[index].priority = available_tasks[task_id].priority;
    process_table[index].start_time = time(NULL);
    strcpy(process_table[index].name, available_tasks[task_id].name);
    strcpy(process_table[index].task_path, available_tasks[task_id].path);
    
    process_count++;
    
    // This is a background task, so don't show messages or clear the screen
    sem_post(process_semaphore);
}

void list_running_processes() {
    printf("\nRunning Processes:\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("| %-5s | %-20s | %-8s | %-8s | %-8s | %-15s |\n", 
           "PID", "Name", "RAM (MB)", "HDD (GB)", "Priority", "Running Time");
    printf("--------------------------------------------------------------------------------\n");
    
    time_t current_time = time(NULL);
    int active_count = 0;
    
    for (int i = 0; i < MAX_TASKS; i++) {
        if (process_table[i].is_active) {
            active_count++;
            int running_seconds = (int)difftime(current_time, process_table[i].start_time);
            int minutes = running_seconds / 60;
            int seconds = running_seconds % 60;
            
            printf("| %-5d | %-20s | %-8d | %-8d | %-8d | %02d:%02d           |\n", 
                   process_table[i].pid, 
                   process_table[i].name, 
                   process_table[i].ram_required, 
                   process_table[i].hdd_required,
                   process_table[i].priority,
                   minutes, seconds);
        }
    }
    printf("--------------------------------------------------------------------------------\n");
    
    if (active_count == 0) {
        printf("No active processes.\n");
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n'); // Clear any remaining input
    getchar(); // Wait for Enter key
}

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
    } else if (process_table[index].is_minimized) {
        printf("Process %s is already minimized.\n", process_table[index].name);
        sleep(1);
    }
}

void resume_process(int index) {
    if (process_table[index].is_active && process_table[index].is_minimized) {
        printf("Resuming process %s...\n", 
               process_table[index].name);
        
        // Set the process as not minimized
        if (sem_wait(process_semaphore) < 0) {
            perror("sem_wait failed");
            sleep(2);
            return;
        }
        process_table[index].is_minimized = 0;
        sem_post(process_semaphore);
        
        // Clear the screen before launching the task
        system("clear");
        
        // Execute the task directly in the current terminal
        int status;
        pid_t result = system(process_table[index].task_path);
        
        if (WIFEXITED(result)) {
            status = WEXITSTATUS(result);
            
            // Check the exit status to see if we need to minimize instead of close
            if (status == 10) {
                // Application requested to be minimized
                if (sem_wait(process_semaphore) < 0) {
                    perror("sem_wait failed");
                } else {
                    process_table[index].is_minimized = 1;
                    sem_post(process_semaphore);
                    printf("%s was minimized again. You can resume it later.\n", process_table[index].name);
                    sleep(2);
                }
            } else {
                // Application exited normally, free resources
                if (sem_wait(process_semaphore) < 0) {
                    perror("sem_wait failed");
                } else {
                    process_table[index].is_active = 0;
                    process_table[index].is_minimized = 0;
                    process_count--;
                    sem_post(process_semaphore);
                }
                
                // Free resources allocated to this process
                free_resources(index);
                
                printf("%s was closed.\n", process_table[index].name);
                sleep(2);
            }
        } else {
            printf("ERROR: Failed to execute %s!\n", process_table[index].name);
            
            // Process will stay minimized, keeping its resources
            if (sem_wait(process_semaphore) < 0) {
                perror("sem_wait failed");
            } else {
                process_table[index].is_minimized = 1;
                sem_post(process_semaphore);
            }
            
            sleep(2);
        }
        
        // Clear the screen after the task finishes
        system("clear");
    } else if (!process_table[index].is_minimized) {
        printf("Process %s is already active.\n", 
               process_table[index].name);
        sleep(1);
    }
}

void send_interrupt(int index, int signal_type) {
    if (!process_table[index].is_active) {
        printf("Process does not exist or is not active.\n");
        sleep(1);
        return;
    }
    
    // Determine which action to take based on signal type
    switch (signal_type) {
        case 1:  // STOP/Minimize
            minimize_process(index);
            break;
        case 2:  // CONT/Resume
            resume_process(index);
            break;
        case 3:  // TERM/Terminate
            terminate_process(index);
            break;
        default:
            printf("Invalid signal type.\n");
            sleep(1);
            return;
    }
}

void terminate_process(int index) {
    if (process_table[index].is_active) {
        char process_name[TASK_NAME_LENGTH];
        strcpy(process_name, process_table[index].name);
        
        printf("Terminating process %s...\n", process_name);
        
        // First update the process table to mark it as inactive
        if (sem_wait(process_semaphore) < 0) {
            perror("sem_wait failed");
            sleep(2);
            return;
        }
        process_table[index].is_active = 0;
        process_table[index].is_minimized = 0;
        process_count--;
        sem_post(process_semaphore);
        
        // Free resources allocated to this process
        free_resources(index);
        
        // Force kill any related processes by name
        char pkill_cmd[200];
        sprintf(pkill_cmd, "pkill -f '%s'", process_name);
        system(pkill_cmd);
        
        printf("Process terminated successfully.\n");
        sleep(1);
    }
}

void switch_mode() {
    is_kernel_mode = !is_kernel_mode;
    printf("Switched to %s mode.\n", is_kernel_mode ? "Kernel" : "User");
    sleep(1);
}

void shutdown_system() {
    printf("\nShutting down %s...\n", OS_NAME);
    sleep(1);
    
    printf("Terminating all running processes...\n");
    
    // Terminate all active processes
    for (int i = 0; i < MAX_TASKS; i++) {
        if (process_table[i].is_active) {
            terminate_process(i); // Use our simplified termination function
        }
    }
    
    printf("Stopping worker threads...\n");
    cleanup_worker_threads();
    
    printf("Saving system state...\n");
    sleep(1);
    printf("Closing system services...\n");
    sleep(1);
    printf("System shutdown complete.\n");
    
    printf("\nThank you for using %s!\n", OS_NAME);
}

// Signal handler for child process termination
void handle_child_exit(int signum __attribute__((unused))) {
    // This function is no longer needed with the in-window approach,
    // but we keep it to maintain compatibility with any existing code
    // that might expect it to be defined
}

// Function to change the CPU scheduler
void change_scheduler() {
    if (!is_kernel_mode) {
        printf("ERROR: Cannot change scheduler in User Mode!\n");
        sleep(2);
        return;
    }
    
    system("clear");
    printf("\n%s - CPU Scheduler Configuration\n", OS_NAME);
    printf("Current Scheduler: %s\n\n", get_scheduler_name(current_scheduler));
    
    printf("Available Schedulers:\n");
    printf("1. First-Come-First-Served (FCFS)\n");
    printf("2. Shortest Job First (SJF)\n");
    printf("3. Priority Scheduling\n");
    printf("4. Round Robin (RR)\n");
    printf("0. Back to Main Menu\n\n");
    
    int choice;
    printf("Select scheduler: ");
    scanf("%d", &choice);
    while (getchar() != '\n'); 
    
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
    
    printf("CPU Scheduler changed to %s.\n", get_scheduler_name(current_scheduler));
    
    // When changing schedulers, we could optionally re-order the running processes
    // based on the new scheduling algorithm
    if (process_count > 0) {
        printf("Note: Running processes will be scheduled according to the new algorithm.\n");
    }
    
    sleep(2);
}

// Function to get the scheduler name as a string
const char* get_scheduler_name(SchedulerType scheduler) {
    switch (scheduler) {
        case SCHEDULER_FCFS:
            return "First-Come-First-Served";
        case SCHEDULER_SJF:
            return "Shortest Job First";
        case SCHEDULER_PRIORITY:
            return "Priority Scheduling";
        case SCHEDULER_RR:
            return "Round Robin";
        default:
            return "Unknown";
    }
}

// New function to initialize the multilevel queue
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

// New function to enqueue a process in the multilevel queue
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
    
    // Check if the queue at this level is full
    if (ml_queue.count[level] >= MAX_TASKS) {
        printf("Queue at level %d is full\n", level);
        return;
    }
    
    // Add the process to the queue
    ml_queue.rear[level] = (ml_queue.rear[level] + 1) % MAX_TASKS;
    ml_queue.queue[level][ml_queue.rear[level]] = process;
    ml_queue.count[level]++;
    
    // Signal that a process is ready
    pthread_cond_signal(&process_ready_cond);
}

// New function to dequeue a process from the multilevel queue
PCB* dequeue_process(int level) {
    // Check if the queue at this level is empty
    if (ml_queue.count[level] <= 0) {
        return NULL;
    }
    
    // Remove the process from the queue
    PCB* process = ml_queue.queue[level][ml_queue.front[level]];
    ml_queue.front[level] = (ml_queue.front[level] + 1) % MAX_TASKS;
    ml_queue.count[level]--;
    
    return process;
}

// New function to handle worker threads
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
            printf("Thread %d is processing task %s (priority level %d)\n", 
                  thread_id, process->name, level);
            
            // Execute the task
            if (!process->is_minimized) {
                // This would be where we actually run the process
                // For now, let's just simulate it
                sleep(ml_queue.time_quantum[level]);
            }
            
            // Return the process to the queue if it's still active
            if (process->is_active) {
                pthread_mutex_lock(&thread_mutex);
                enqueue_process(process);
                pthread_mutex_unlock(&thread_mutex);
            }
        }
    }
    
    return NULL;
}

// New function to create worker threads
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

// New function to clean up worker threads
void cleanup_worker_threads() {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (thread_active[i]) {
            pthread_cancel(worker_threads[i]);
            pthread_join(worker_threads[i], NULL);
            thread_active[i] = 0;
        }
    }
}

// Replace launch_task with a version that uses exec
void launch_task_with_exec(int task_id) {
    // Check if the application is already running
    if (is_application_running(available_tasks[task_id].name)) {
        // If it's already running but minimized, we can resume it
        for (int i = 0; i < MAX_TASKS; i++) {
            if (process_table[i].is_active && 
                strcmp(process_table[i].name, available_tasks[task_id].name) == 0 &&
                process_table[i].is_minimized) {
                printf("Resuming %s...\n", available_tasks[task_id].name);
                resume_process(i);
                return;
            }
        }
        
        // If it's active and not minimized, we can't start another instance
        printf("ERROR: %s is already running!\n", available_tasks[task_id].name);
        sleep(2);
        return;
    }
    
    // Check if we have available slots
    if (process_count >= MAX_TASKS) {
        printf("ERROR: Maximum number of processes reached!\n");
        sleep(2);
        return;
    }
    
    int ram_required = available_tasks[task_id].ram_required;
    int hdd_required = available_tasks[task_id].hdd_required;
    
    // Try to allocate resources
    if (!allocate_resources(ram_required, hdd_required)) {
        printf("ERROR: Not enough system resources to start %s!\n", 
               available_tasks[task_id].name);
        sleep(2);
        return;
    }
    
    // Check if the task is executable
    if (access(available_tasks[task_id].path, X_OK) != 0) {
        printf("ERROR: %s is not executable!\n", available_tasks[task_id].name);
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sleep(2);
        return;
    }
    
    // Reserve a slot in the process table
    int index = -1;
    if (sem_wait(process_semaphore) < 0) {
        perror("sem_wait failed");
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sleep(2);
        return;
    }
    
    // Find an empty slot
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!process_table[i].is_active) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        // No empty slot found
        printf("ERROR: No empty slot in process table!\n");
        // Return the resources
        pthread_mutex_lock(&resource_mutex);
        hardware.available_ram += ram_required;
        hardware.available_hdd += hdd_required;
        hardware.available_cores++;
        pthread_mutex_unlock(&resource_mutex);
        sem_post(process_semaphore);
        sleep(2);
        return;
    }
    
    // Set up information in the process table
    process_table[index].is_active = 1;
    process_table[index].is_minimized = 0;
    process_table[index].ram_required = ram_required;
    process_table[index].hdd_required = hdd_required;
    process_table[index].priority = available_tasks[task_id].priority;
    process_table[index].start_time = time(NULL);
    strcpy(process_table[index].name, available_tasks[task_id].name);
    strcpy(process_table[index].task_path, available_tasks[task_id].path);
    
    process_count++;
    sem_post(process_semaphore);
    
    // Use fork and exec to launch the task
    pid_t pid = fork();
    
    if (pid == -1) {
        // Fork failed
        perror("fork failed");
        
        // Free resources and update process table
        if (sem_wait(process_semaphore) < 0) {
            perror("sem_wait failed");
        } else {
            process_table[index].is_active = 0;
            process_table[index].is_minimized = 0;
            process_count--;
            sem_post(process_semaphore);
        }
        
        // Free resources allocated to this process
        free_resources(index);
        
        sleep(2);
        return;
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
        if (sem_wait(process_semaphore) < 0) {
            perror("sem_wait failed");
        } else {
            process_table[index].pid = pid;
            sem_post(process_semaphore);
        }
        
        printf("Started %s with PID %d\n", available_tasks[task_id].name, pid);
        
        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
        
        // Check the exit status to see if we need to minimize instead of close
        if (WIFEXITED(status) && WEXITSTATUS(status) == 10) {
            // Application requested to be minimized
            if (sem_wait(process_semaphore) < 0) {
                perror("sem_wait failed");
            } else {
                process_table[index].is_minimized = 1;
                sem_post(process_semaphore);
                printf("%s was minimized. You can resume it later.\n", available_tasks[task_id].name);
                sleep(2);
            }
        } else {
            // Application exited normally, free resources
            if (sem_wait(process_semaphore) < 0) {
                perror("sem_wait failed");
            } else {
                process_table[index].is_active = 0;
                process_table[index].is_minimized = 0;
                process_count--;
                sem_post(process_semaphore);
            }
            
            // Free resources allocated to this process
            free_resources(index);
            
            printf("%s was closed.\n", available_tasks[task_id].name);
            sleep(2);
        }
        
        // Clear the screen after the task finishes
        system("clear");
    }
}
