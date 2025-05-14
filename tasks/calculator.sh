#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Calculator
# A simple calculator application
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Calculator ====="
echo "This application performs basic arithmetic calculations."
echo "Type 'options' for menu or 'q' to quit"
echo "----------------------------------------"

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
  echo "Enter expression (e.g., 2 + 3) or 'options':"
  read input
  
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
    echo "Closing Calculator..."
    sleep 1
    exit 0
  fi
    # ##########################################
  # CALCULATION LOGIC
  # ##########################################
  # Calculate the result using bc
  result=$(echo "scale=6; $input" | bc -l 2>/dev/null)
  
  # Check if calculation was successful
  if [ $? -eq 0 ]; then
    echo "Result: $result"
  else
    echo "Error: Invalid expression. Please try again."
  fi
  
  echo ""
done