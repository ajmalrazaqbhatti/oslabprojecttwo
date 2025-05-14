#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Calendar
# A simple calendar and event management utility
# ----------------

# ##########################################
# DISPLAY CONFIGURATION
# ##########################################
clear
echo "===== Calendar ====="
echo "This application displays calendar and manages events."
echo "Type 'options' for menu or 'q' to quit"
echo ""

# Create data directory if it doesn't exist
DATA_DIR="$HOME/.calendar"
EVENT_FILE="$DATA_DIR/events.txt"

if [ ! -d "$DATA_DIR" ]; then
    mkdir -p "$DATA_DIR"
    touch "$EVENT_FILE"
fi

# ##########################################
# FUNCTIONS
# ##########################################
show_calendar() {
    if [ -z "$1" ] && [ -z "$2" ]; then
        # Current month
        cal
    elif [ -n "$1" ] && [ -z "$2" ]; then
        # Specific month of current year
        cal "$1"
    else
        # Specific month and year
        cal "$1" "$2"
    fi
}

add_event() {
    if [[ "$1" =~ ^[0-9]{4}-[0-9]{2}-[0-9]{2}\ .+ ]]; then
        echo "$1" >> "$EVENT_FILE"
        echo "Event added successfully!"
    else
        echo "Error: Invalid format. Use YYYY-MM-DD followed by event description."
        echo "Example: 2023-09-01 Meeting with team"
    fi
}

list_events() {
    if [ -s "$EVENT_FILE" ]; then
        echo "Your events:"
        cat -n "$EVENT_FILE" | sort -k1,1
    else
        echo "No events found."
    fi
}

delete_event() {
    if [ -s "$EVENT_FILE" ]; then
        if [[ "$1" =~ ^[0-9]+$ ]]; then
            event_count=$(wc -l < "$EVENT_FILE")
            if [ "$1" -gt 0 ] && [ "$1" -le "$event_count" ]; then
                sed -i "${1}d" "$EVENT_FILE"
                echo "Event deleted successfully!"
            else
                echo "Error: Event number $1 does not exist."
            fi
        else
            echo "Error: Please provide a valid event number."
        fi
    else
        echo "No events found."
    fi
}

# ##########################################
# MAIN PROGRAM LOOP
# ##########################################
while true; do
    clear
    echo "===== Calendar ====="
    echo "This application displays calendar and manages events."
    echo "Type 'options' for menu or 'q' to quit"
    echo ""
    
    echo "1. Show calendar for current month"
    echo "2. Show calendar for specific month"
    echo "3. Add new event"
    echo "4. List all events"
    echo "5. Delete an event"
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
        echo "Closing Calendar..."
        sleep 1
        exit 0
    fi
    
    case $input in
        1) 
            show_calendar
            echo "Press Enter to continue..."
            read
            ;;
        2)
            echo "Enter month [1-12]: "
            read month
            echo "Enter year: "
            read year
            show_calendar "$month" "$year"
            echo "Press Enter to continue..."
            read
            ;;
        3)
            echo "Enter event date and description (YYYY-MM-DD Event description):"
            read event_data
            add_event "$event_data"
            echo "Press Enter to continue..."
            read
            ;;
        4)
            list_events
            echo "Press Enter to continue..."
            read
            ;;
        5)
            list_events
            echo "Enter event number to delete:"
            read event_num
            delete_event "$event_num"
            echo "Press Enter to continue..."
            read
            ;;
        *)
            echo "Invalid option. Press Enter to try again..."
            read
            ;;
    esac
done
