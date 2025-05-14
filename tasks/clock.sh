#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Clock
# This task displays the current time and system uptime
# Implements standard close and minimize functionality:
# - When the task is closed, it exits with code 0
# - When the task is minimized, it exits with code 10
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Clock ====="
echo "This application displays the current time and date."
echo "Type 'options' for menu or 'q' to quit"
echo "----------------------------------------"

# ##########################################
# FUNCTIONS
# ##########################################
# Function to display current time and uptime
display_time() {
    echo "Current time: $(date +"%H:%M:%S")"
    echo "Current date: $(date +"%Y-%m-%d")"
    echo "System uptime: $(uptime -p)"
    echo "----------------------------------------"
    echo "Type 'options' for menu or 'q' to quit"
}

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
    # Display the current time
    display_time
    
    # Read user input
    read -t 3 -p "> " input
    
    # ##########################################
    # MENU HANDLING
    # ##########################################
    # Check for options menu
    if [[ "$input" == "options" ]]; then
        echo "OPTIONS:"
        echo "1. Close (exit)"
        echo "2. Minimize (return to main menu)"
        echo -n "Choose option: "
        read option
        
        case $option in
            1)
                echo "Closing task..."
                sleep 1
                exit 0
                ;;
            2)
                echo "Minimizing task..."
                sleep 1
                exit 10  # Special exit code for minimize
                ;;
            *)
                echo "Invalid option. Continuing..."
                sleep 1
                ;;
        esac
        continue
    fi
    
    # Check for exit condition
    if [[ "$input" == "q" || "$input" == "Q" ]]; then
        echo "Closing Clock..."
        sleep 1
        exit 0
    fi
    
    clear
done