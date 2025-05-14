#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: BMI Calculator
# A simple application that calculates Body Mass Index
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== BMI Calculator ====="
echo "This application calculates your Body Mass Index (BMI)."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
  echo "Enter your weight in kg:"
  read weight
  
  # ##########################################
  # MENU HANDLING
  # ##########################################
  # Check for options menu
  if [[ "$weight" == "options" ]]; then
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
  if [[ "$weight" == "q" || "$weight" == "Q" ]]; then
    echo "Closing BMI Calculator..."
    sleep 1
    exit 0
  fi
  
  # ##########################################
  # INPUT VALIDATION
  # ##########################################
  # Validate weight is a number
  if ! [[ "$weight" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
    echo "Error: Please enter a valid weight number."
    continue
  fi
  
  echo "Enter your height in meters:"
  read height
  
  # Validate height is a number
  if ! [[ "$height" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
    echo "Error: Please enter a valid height number."
    continue
  fi
  
  # ##########################################
  # BMI CALCULATION LOGIC
  # ##########################################
  # Calculate BMI (weight / height^2)
  bmi=$(echo "scale=2; $weight / ($height * $height)" | bc)
  
  echo "Your BMI is: $bmi"
  
  # Interpret BMI
  if (( $(echo "$bmi < 18.5" | bc -l) )); then
    echo "Interpretation: Underweight"
  elif (( $(echo "$bmi < 25" | bc -l) )); then
    echo "Interpretation: Normal weight"
  elif (( $(echo "$bmi < 30" | bc -l) )); then
    echo "Interpretation: Overweight"
  else
    echo "Interpretation: Obesity"
  fi
  
  echo ""
done
