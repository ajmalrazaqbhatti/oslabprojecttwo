# Semester Project Self-Evaluation (Spring 2025)

## BCS-4A

**Name:** \***\*\*\*\*\*\*\***\_\***\*\*\*\*\*\*\***  
**Name:** \***\*\*\*\*\*\*\***\_\***\*\*\*\*\*\*\***  
**Reg. No.:** \***\*\*\*\*\***\_\***\*\*\*\*\***  
**Reg. No.:** \***\*\*\*\*\***\_\***\*\*\*\*\***

## Contribution to the Project's development:

### First Team Member

Developed the following modules:

1. Process Management and Task Scheduling
2. Memory and Resource Management
3. Context Switching Implementation
4. User Interface and Task Manager

### Second Team Member

Developed the following modules:

1. Multilevel Queue Scheduling
2. Thread Management and Concurrency
3. Process Creation with Fork/Exec
4. Synchronization Mechanisms

## Work Done (in %): 100%

## Project Modules Successfully Implemented:

1. Multitasking - Supporting multiple applications running simultaneously
2. Context Switching - Saving and restoring process states when minimizing/resuming
3. Resource Allocation - Managing CPU, RAM, and HDD resources
4. User and Kernel Mode Distinction - Implementing privilege levels
5. Process Creation - Using both system() and fork()/exec() methods
6. Thread Implementation - Creating worker threads to handle tasks
7. EXEC Commands - Using exec functions to launch applications
8. Mutual Exclusion - Using mutex, semaphores, and condition variables
9. Multilevel Queue Scheduling - Implementing priority-based scheduling

## Modules That Were Not Implemented:

1. Virtual Memory Management - No paging or segmentation implemented
2. File System Implementation - Limited to basic file operations
3. Real Inter-Process Communication - Using basic synchronization instead

## Challenges faced & how they were overcome:

1. Process Management Complexity: Implementing a system that could properly track and manage multiple processes was initially challenging. Overcame this by using a structured process control block (PCB) and a centralized process table. Added synchronization with semaphores to ensure safe concurrent access to the process table, preventing race conditions when multiple processes needed to be updated simultaneously.

2. Resource Allocation Issues: Early implementations had problems with resource leaks when processes terminated unexpectedly. Solved by implementing a robust signal handler for SIGCHLD to catch and properly clean up terminated processes, along with mutex-protected resource allocation functions to ensure atomicity of operations even during unexpected failures.

3. Scheduling Implementation: Creating a multilevel queue with different scheduling policies was complex. Addressed by designing a clear structure for the queue system and carefully implementing the priority-based scheduling mechanism. Used condition variables to efficiently signal when processes were ready to be scheduled, avoiding busy-waiting that would waste CPU cycles.

4. Context Switching: Saving and restoring process states for minimizing/resuming applications was difficult. Implemented a clean solution using process status flags in the PCB and proper resource management that preserved application states correctly.

## Learnings from the Project:

1. Real-world OS Concepts: Gained practical understanding of core OS concepts including process management, scheduling algorithms, and resource allocation. The implementation of these concepts in working code provided deeper insights than theoretical knowledge alone. Particularly valuable was seeing how abstract concepts like multilevel feedback queues translate into actual code structures.

2. Synchronization Mechanisms: Learned how to effectively use different synchronization primitives like mutexes, semaphores, and condition variables to coordinate access to shared resources. Understanding the specific use cases for each mechanism and their performance implications was important for building a responsive system.

3. System Design: Developed skills in designing a complex system with multiple interacting components. The modular approach taken in the project, with clear separation between different subsystems (process management, scheduling, resource allocation, etc.), demonstrated the importance of good system architecture in managing complexity.

4. Process Creation and Management: Gained practical experience with fork/exec model of process creation and management. Understanding the differences between using system() versus fork()/exec() provided insights into the security and efficiency implications of different process creation methods.

## Comments (if any):

The NexOS project successfully demonstrates all core operating system concepts required by the course. The system allows multiple applications to run concurrently, allocates and manages resources effectively, and implements different scheduling algorithms. The code structure is modular and well-documented, making it easy to understand and extend.
