#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Random Password Generator
# A simple application that generates random passwords
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Random Password Generator ====="
echo "This application generates secure random passwords."
echo "Type 'options' for menu or Enter password length to continue"
echo "----------------------------------------"

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
    echo "Enter password length:"
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
        echo "Closing Password Generator..."
        sleep 1
        exit 0
    fi
    
    # ##########################################
    # INPUT VALIDATION
    # ##########################################
    # Validate length is a number
    if ! [[ "$input" =~ ^[0-9]+$ ]]; then
        echo "Error: Please enter a valid number."
        continue
    fi
    
    length=$input
    
    # Validate reasonable length
    if (( length < 4 || length > 64 )); then
        echo "Error: Password length must be between 4 and 64 characters."
        continue
    fi
    
    echo "Include special characters? (y/n):"
    read special
    
    # ##########################################
    # PASSWORD GENERATION LOGIC
    # ##########################################
    # Generate the password
    if [[ "$special" == "y" || "$special" == "Y" ]]; then
        # With special characters
        password=$(< /dev/urandom tr -dc 'A-Za-z0-9!@#$%^&*()_+{}|:<>?=' | head -c "$length")
    else
        # Alphanumeric only
        password=$(< /dev/urandom tr -dc 'A-Za-z0-9' | head -c "$length")
    fi
    
    echo "Generated password: $password"
    echo ""
    echo "Enter new length or type 'options' for menu"
done
