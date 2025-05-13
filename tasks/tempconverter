#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Temperature Converter
# A simple application that converts between Celsius and Fahrenheit
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Temperature Converter ====="
echo "This application converts temperatures between Celsius and Fahrenheit."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
  echo "Select conversion type:"
  echo "1. Celsius to Fahrenheit"
  echo "2. Fahrenheit to Celsius"
  read choice
  
  # ##########################################
  # MENU HANDLING
  # ##########################################
  # Check for options menu
  if [[ "$choice" == "options" ]]; then
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
  if [[ "$choice" == "q" || "$choice" == "Q" ]]; then
    echo "Closing Temperature Converter..."
    sleep 1
    exit 0
  fi
  
  # ##########################################
  # INPUT VALIDATION
  # ##########################################
  # Validate choice
  if ! [[ "$choice" =~ ^[1-2]$ ]]; then
    echo "Error: Please select 1 or 2."
    continue
  fi
  
  echo "Enter temperature value:"
  read temp
  
  # Validate temperature is a number
  if ! [[ "$temp" =~ ^-?[0-9]+(\.[0-9]+)?$ ]]; then
    echo "Error: Please enter a valid temperature number."
    continue
  fi
  
  # ##########################################
  # CONVERSION LOGIC
  # ##########################################
  if [[ "$choice" == "1" ]]; then
    # Celsius to Fahrenheit: (C × 9/5) + 32 = F
    result=$(echo "scale=1; ($temp * 9/5) + 32" | bc)
    echo "$temp°C = $result°F"
  else
    # Fahrenheit to Celsius: (F - 32) × 5/9 = C
    result=$(echo "scale=1; ($temp - 32) * 5/9" | bc)
    echo "$temp°F = $result°C"
  fi
  
  echo ""
done
