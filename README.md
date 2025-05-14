# NexOS - Operating System Simulator

An operating system simulator demonstrating core OS concepts through terminal-based interactions.

## Features

- **OS Identity and Booting**: Custom OS name with simulated boot sequence.
- **Resource Management**: Simulated RAM, HDD, and CPU core allocation.
- **Process Management**: Process creation, scheduling, and termination.
- **Mode Switching**: Kernel mode and user mode support.
- **Task Applications**: Multiple built-in applications demonstrating different OS functionality.

## Getting Started

### Prerequisites

- GCC compiler
- pthread library
- Bash shell environment
- X terminal (gnome-terminal or xterm)

### Building the Project

```bash
# Set up the directory structure and task files
make setup

# Compile the project
make

# Run the OS simulator
make run
```

## Usage

1. When started, the simulator will display a boot sequence and ask for hardware specifications.
2. After initialization, a menu of available applications will be displayed.
3. Select an application by entering its corresponding number.
4. Use system controls to:
   - List running processes
   - Terminate a process (only in Kernel Mode)
   - Switch between User Mode and Kernel Mode
   - Shutdown the system

## Applications

The simulator includes the following applications:

- Notepad: Create and save text notes
- Calculator: Perform basic calculations
- Clock: Display current time and system uptime
- File Manager: Perform basic file operations
- System Monitor: Display system resource usage
- And more...

## Task Application Structure

All task applications follow a standardized structure:

1. **Header**: Contains script overview and display configuration
2. **Menu Handling**: Consistent handling of options menu (close/minimize)
3. **Functions**: Task-specific helper functions
4. **Main Program Loop**: Core functionality of the task

To create a new task, you can use the template in `tasks/template.sh`.

## Project Structure

- `main.c`: Core OS simulator functionality
- `tasks/`: Contains all task applications (see [tasks/README.md](tasks/README.md))
- `Makefile`: Build configuration
- `create_structure.sh`: Script to set up the project structure
- `standardize_tasks.sh`: Script to standardize task structures
- `TASK_DOCUMENTATION.md`: Detailed documentation for task development

## Academic Purpose

This project is designed as an educational tool to demonstrate operating system concepts including:

- Multitasking
- Context Switching
- Resource Allocation
- Process Creation
- Scheduling
- Synchronization
- User Mode vs Kernel Mode

## License

This project is for educational purposes. Feel free to modify and extend it.
