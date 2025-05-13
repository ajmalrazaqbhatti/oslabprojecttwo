#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Prime Number Checker
# A simple application that checks if a number is prime
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Prime Number Checker ====="
echo "This application checks if a number is prime."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
  echo "Enter a positive integer:"
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
    echo "Closing Prime Number Checker..."
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
  
  # ##########################################
  # PRIME NUMBER CHECKING LOGIC
  # ##########################################
  # Check if number is prime
  if [ "$input" -eq 1 ]; then
    echo "1 is not considered a prime number."
    continue
  fi
  
  if [ "$input" -eq 2 ]; then
    echo "2 is a prime number."
    continue
  fi
  
  # Check if even
  if [ $((input % 2)) -eq 0 ]; then
    echo "$input is not a prime number (divisible by 2)."
    continue
  fi
  
  is_prime=true
  
  # Check odd divisors up to square root
  for ((i=3; i*i<=input; i+=2)); do
    if [ $((input % i)) -eq 0 ]; then
      is_prime=false
      echo "$input is not a prime number (divisible by $i)."
      break
    fi
  done
  
  if [ "$is_prime" = true ]; then
    echo "$input is a prime number."
  fi
done
