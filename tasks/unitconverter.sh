#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Unit Converter
# A simple application that converts between various units of measurement
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Unit Converter ====="
echo "This application converts between different units of measurement."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
  echo "Select conversion type:"
  echo "1. Length (m to ft)"
  echo "2. Length (ft to m)"
  echo "3. Weight (kg to lb)"
  echo "4. Weight (lb to kg)" 
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
    echo "Closing Unit Converter..."
    sleep 1
    exit 0
  fi
  
  # ##########################################
  # INPUT VALIDATION
  # ##########################################
  # Validate choice
  if ! [[ "$choice" =~ ^[1-4]$ ]]; then
    echo "Error: Please select a valid option (1-4)."
    continue
  fi
  
  echo "Enter value to convert:"
  read value
  
  # Validate value is a number
  if ! [[ "$value" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
    echo "Error: Please enter a valid number."
    continue
  fi
  
  # ##########################################
  # CONVERSION LOGIC
  # ##########################################
  case "$choice" in
    1) # m to ft
      result=$(echo "scale=2; $value * 3.28084" | bc)
      echo "$value meters = $result feet"
      ;;
    2) # ft to m
      result=$(echo "scale=2; $value * 0.3048" | bc)
      echo "$value feet = $result meters"
      ;;
    3) # kg to lb
      result=$(echo "scale=2; $value * 2.20462" | bc)
      echo "$value kilograms = $result pounds"
      ;;
    4) # lb to kg
      result=$(echo "scale=2; $value * 0.453592" | bc)
      echo "$value pounds = $result kilograms"
      ;;
  esac
  
  echo ""
done
