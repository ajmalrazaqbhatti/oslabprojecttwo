#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Notepad
# A simple text editor for creating and editing notes
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Notepad ====="
echo "This application allows you to create and edit text files."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# Create a notes directory if it doesn't exist
NOTES_DIR="./notes"
mkdir -p "$NOTES_DIR"

# ##########################################
# FUNCTIONS
# ##########################################
create_new_note() {
    echo "Enter filename (without extension): "
    read note_name
    
    if [ -z "$note_name" ]; then
        echo "Error: Filename cannot be empty"
        sleep 2
        return 1
    fi
    
    # Add .txt extension if not provided
    if [[ "$note_name" != *.txt ]]; then
        note_name="${note_name}.txt"
    fi
    
    # Check if file already exists
    if [ -f "$NOTES_DIR/$note_name" ]; then
        echo "Note already exists. Opening for editing..."
        sleep 1
    else
        echo "Creating new note: $note_name"
        touch "$NOTES_DIR/$note_name"
        sleep 1
    fi
    
    # Edit the file using nano
    nano "$NOTES_DIR/$note_name"
    return 0
}

list_notes() {
    echo "SELECT A NOTE TO OPEN"
    echo "----------------------------------------"
    
    # Get list of files in the notes directory
    files=($(ls -1 "$NOTES_DIR" 2>/dev/null))
    
    if [ ${#files[@]} -eq 0 ]; then
        echo "No notes found"
    else
        for i in "${!files[@]}"; do
            printf "[%2d] %s\n" $((i+1)) "${files[$i]}"
        done
    fi
    
    echo "----------------------------------------"
    echo "[0] Back"
    echo ""
    echo -n "Enter your choice: "
}

open_existing_note() {
    while true; do
        list_notes
        read choice
        
        # Check if user wants to go back
        if [ "$choice" = "0" ]; then
            return 0
        fi
        
        # Validate the choice
        if [[ "$choice" =~ ^[0-9]+$ ]]; then
            # Convert to zero-based index
            index=$((choice-1))
            
            # Get list of files
            files=($(ls -1 "$NOTES_DIR" 2>/dev/null))
            
            # Check if the index is valid
            if [ $index -ge 0 ] && [ $index -lt ${#files[@]} ]; then
                # Edit the selected file
                nano "$NOTES_DIR/${files[$index]}"
                return 0
            else
                echo "Invalid selection. Please try again."
                sleep 1
            fi
        else
            echo "Please enter a number."
            sleep 1
        fi
    done
}

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
    clear
    echo "===== Notepad ====="
    echo "This application allows you to create and edit text files."
    echo "Type 'options' for menu or 'q' to quit"
    echo ""
    
    echo "1. Create New Note"
    echo "2. Open Existing Note"
    echo ""
    echo -n "Enter choice (1-2) or 'options' or 'q': "
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
        echo "Closing Notepad..."
        sleep 1
        exit 0
    fi
    
    case $input in
        1) create_new_note ;;
        2) open_existing_note ;;
        *) 
            echo "Invalid choice. Please try again."
            sleep 1
            ;;
    esac
done