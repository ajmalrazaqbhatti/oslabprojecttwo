#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Text Reverser
# A simple application that reverses input text
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Text Reverser ====="
echo "This application reverses any text you enter."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
  echo "Enter text to reverse:"
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
    echo "Closing Text Reverser..."
    sleep 1
    exit 0
  fi
  
  # ##########################################
  # TEXT REVERSAL LOGIC
  # ##########################################
  # Reverse the string
  reversed=""
  for (( i=${#input}-1; i>=0; i-- )); do
    reversed="$reversed${input:$i:1}"
  done
  
  echo "Original: $input"
  echo "Reversed: $reversed"
  echo ""
done
