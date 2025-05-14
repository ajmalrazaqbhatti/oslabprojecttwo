#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Number Sorter
# A simple application that sorts a list of numbers
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Number Sorter ====="
echo "This application sorts a list of numbers."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
  echo "Enter numbers separated by spaces:"
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
    echo "Closing Number Sorter..."
    sleep 1
    exit 0
  fi
  
  # ##########################################
  # INPUT VALIDATION
  # ##########################################
  # Validate input contains only numbers and spaces
  if ! [[ "$input" =~ ^[0-9\ ]+$ ]]; then
    echo "Error: Please enter only numbers separated by spaces."
    continue
  fi
  
  # ##########################################
  # SORTING LOGIC
  # ##########################################
  # Convert input to array
  IFS=' ' read -r -a numbers <<< "$input"
  
  # Sort the numbers
  sorted=($(echo "${numbers[@]}" | tr ' ' '\n' | sort -n | tr '\n' ' '))
  
  echo "Original: ${numbers[@]}"
  echo "Sorted (ascending): ${sorted[@]}"
  
  # Sort in descending order
  desc_sorted=($(echo "${numbers[@]}" | tr ' ' '\n' | sort -nr | tr '\n' ' '))
  echo "Sorted (descending): ${desc_sorted[@]}"
  echo ""
done
