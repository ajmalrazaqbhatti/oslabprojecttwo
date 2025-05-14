#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: File Manager
# A simple file manager for creating, copying, moving, and deleting files
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== File Manager ====="
echo "This application allows you to manage files and directories."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# Create a workspace directory if it doesn't exist
WORKSPACE_DIR="./workspace"
mkdir -p "$WORKSPACE_DIR"
cd "$WORKSPACE_DIR"

# ##########################################
# FUNCTIONS
# ##########################################
create_file() {
    echo "Enter filename: "
    read file_name
    
    if [ -z "$file_name" ]; then
        echo "Error: Filename cannot be empty"
        sleep 2
        return 1
    fi
    
    # Check if file already exists
    if [ -f "$file_name" ]; then
        echo "File already exists. Do you want to overwrite it? (y/n): "
        read answer
        if [[ "$answer" != "y" && "$answer" != "Y" ]]; then
            echo "Operation cancelled."
            sleep 1
            return 1
        fi
    fi
    
    # Create the file
    echo "Creating file: $file_name"
    touch "$file_name"
    echo "File created successfully."
    sleep 1
    
    # Ask if user wants to edit the file
    echo "Do you want to edit this file? (y/n): "
    read answer
    if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
        nano "$file_name"
    fi
    
    return 0
}

copy_file() {
    echo "Enter source file path: "
    read source
    
    if [ -z "$source" ] || [ ! -f "$source" ]; then
        echo "Error: Source file doesn't exist or is invalid"
        sleep 2
        return 1
    fi
    
    echo "Enter destination path: "
    read destination
    
    if [ -z "$destination" ]; then
        echo "Error: Destination cannot be empty"
        sleep 2
        return 1
    fi
    
    # Check if destination is a directory
    if [ -d "$destination" ]; then
        # If destination is a directory, we'll copy the file into it
        destination="${destination%/}/$(basename "$source")"
    fi
    
    # Check if destination file already exists
    if [ -f "$destination" ]; then
        echo "Destination file already exists. Do you want to overwrite it? (y/n): "
        read answer
        if [[ "$answer" != "y" && "$answer" != "Y" ]]; then
            echo "Operation cancelled."
            sleep 1
            return 1
        fi
    fi
    
    # Copy the file
    cp "$source" "$destination"
    echo "File copied successfully."
    sleep 1
    return 0
}

move_file() {
    echo "Enter source file path: "
    read source
    
    if [ -z "$source" ] || [ ! -f "$source" ]; then
        echo "Error: Source file doesn't exist or is invalid"
        sleep 2
        return 1
    fi
    
    echo "Enter destination path: "
    read destination
    
    if [ -z "$destination" ]; then
        echo "Error: Destination cannot be empty"
        sleep 2
        return 1
    fi
    
    # Check if destination is a directory
    if [ -d "$destination" ]; then
        # If destination is a directory, we'll move the file into it
        destination="${destination%/}/$(basename "$source")"
    fi
    
    # Check if destination file already exists
    if [ -f "$destination" ]; then
        echo "Destination file already exists. Do you want to overwrite it? (y/n): "
        read answer
        if [[ "$answer" != "y" && "$answer" != "Y" ]]; then
            echo "Operation cancelled."
            sleep 1
            return 1
        fi
    fi
    
    # Move the file
    mv "$source" "$destination"
    echo "File moved successfully."
    sleep 1
    return 0
}

delete_file() {
    echo "Enter file path to delete: "
    read file_path
    
    if [ -z "$file_path" ] || [ ! -f "$file_path" ]; then
        echo "Error: File doesn't exist or is invalid"
        sleep 2
        return 1
    fi
    
    echo "Are you sure you want to delete '$file_path'? (y/n): "
    read answer
    if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
        rm "$file_path"
        echo "File deleted successfully."
    else
        echo "Operation cancelled."
    fi
    
    sleep 1
    return 0
}

list_files() {
    echo "CURRENT DIRECTORY: $(pwd)"
    echo "----------------------------------------"
    
    # List files with details
    ls -la
    
    echo "----------------------------------------"
    echo "Press Enter to continue..."
    read
    return 0
}

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
    clear
    echo "===== File Manager ====="
    echo "Working Directory: $(pwd)"
    echo "Type 'options' for menu or 'q' to quit"
    echo ""
    
    echo "1. Create New File"
    echo "2. Copy File"
    echo "3. Move File"
    echo "4. Delete File"
    echo "5. List Files"
    echo ""
    echo -n "Enter choice (1-5) or 'options' or 'q': "
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
        echo "Closing File Manager..."
        sleep 1
        exit 0
    fi
    
    case $input in
        1) create_file ;;
        2) copy_file ;;
        3) move_file ;;
        4) delete_file ;;
        5) list_files ;;
        *) 
            echo "Invalid choice. Please try again."
            sleep 1
            ;;
    esac
done
