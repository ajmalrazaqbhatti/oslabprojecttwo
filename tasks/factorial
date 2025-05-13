#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Factorial Calculator
# A simple application that calculates factorial of a number
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Factorial Calculator ====="
echo "This application calculates the factorial of a number."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# ##########################################
# FUNCTIONS
# ##########################################
calculate_factorial() {
  local num=$1
  local factorial=1
  
  for ((i=1; i<=num; i++)); do
    factorial=$((factorial * i))
  done
  
  echo "$factorial"
}

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
  echo "Enter a positive integer (0-20):"
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
    echo "Closing Factorial Calculator..."
    sleep 1
    exit 0
  fi
  
  # ##########################################
  # INPUT VALIDATION
  # ##########################################
  # Validate input is a positive integer
  if ! [[ "$input" =~ ^[0-9]+$ ]]; then
    echo "Error: Please enter a valid positive integer."
    continue
  fi
  
  # Check range (to avoid integer overflow)
  if [ "$input" -gt 20 ]; then
    echo "Error: Number too large. Please enter a number between 0 and 20."
    continue
  fi
  
  # ##########################################
  # FACTORIAL CALCULATION
  # ##########################################
  result=$(calculate_factorial "$input")
  echo "$input! = $result"
  echo ""
done
