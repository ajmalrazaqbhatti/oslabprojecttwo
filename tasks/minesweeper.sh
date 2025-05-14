#!/usr/bin/env bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Minesweeper
# A simple implementation of the classic minesweeper game
# ----------------

# ##########################################
# GAME LAUNCH
# ##########################################
# Launch minesweeper in a new terminal window
gnome-terminal -- bash -c "
echo '=== NexOS Minesweeper ==='
echo 'Type \"options\" for menu or Enter moves to play'
echo '----------------------------------------'

# ##########################################
# GAME CONFIGURATION
# ##########################################
# Size of the game board
ROWS=8
COLS=8
MINES=10

# Initialize arrays
declare -A board
declare -A revealed
declare -A flagged

# Game state
game_over=0
mines_placed=0
first_move=1

# ##########################################
# GAME INITIALIZATION FUNCTIONS
# ##########################################
# Function to initialize the board
init_board() {
    for ((i=0; i<ROWS; i++)); do
        for ((j=0; j<COLS; j++)); do
            board[\$i,\$j]=0
            revealed[\$i,\$j]=0
            flagged[\$i,\$j]=0
        done
    done
}

# Function to place mines randomly
place_mines() {
    local row=\$1
    local col=\$2
    
    while ((mines_placed < MINES)); do
        local r=\$((RANDOM % ROWS))
        local c=\$((RANDOM % COLS))
        
        # Don't place a mine on the first click or adjacent cells
        if [[ \$r -ne \$row || \$c -ne \$col ]] && \
           [[ \$r -lt \$((row-1)) || \$r -gt \$((row+1)) || \
              \$c -lt \$((col-1)) || \$c -gt \$((col+1)) ]] && \
           [[ \${board[\$r,\$c]} != 'M' ]]; then
            board[\$r,\$c]='M'
            ((mines_placed++))
            
            # Update adjacent cell counts
            for ((i=r-1; i<=r+1; i++)); do
                for ((j=c-1; j<=c+1; j++)); do
                    if ((i >= 0 && i < ROWS && j >= 0 && j < COLS)) && [[ \${board[\$i,\$j]} != 'M' ]]; then
                        board[\$i,\$j]=\$((board[\$i,\$j] + 1))
                    fi
                done
            done
        fi
    done
}

# ##########################################
# GAME DISPLAY FUNCTIONS
# ##########################################
# Function to display the board
display_board() {
    clear
    echo '=== NexOS Minesweeper ==='
    echo 'Type \"options\" for menu or Enter moves to play'
    echo '----------------------------------------'
    
    echo '  |' $(seq -s ' ' 0 $((COLS-1)))
    echo '--+' $(printf -- '- %.0s' $(seq 0 $((COLS-1))))
    
    for ((i=0; i<ROWS; i++)); do
        echo -n \"\$i | \"
        for ((j=0; j<COLS; j++)); do
            if ((flagged[\$i,\$j])); then
                echo -n \"F \"
            elif ((revealed[\$i,\$j])); then
                if [[ \${board[\$i,\$j]} == 'M' ]]; then
                    echo -n \"* \"
                elif [[ \${board[\$i,\$j]} == 0 ]]; then
                    echo -n \". \"
                else
                    echo -n \"\${board[\$i,\$j]} \"
                fi
            else
                echo -n \"# \"
            fi
        done
        echo
    done
    
    if ((game_over == 1)); then
        echo 'GAME OVER! You hit a mine!'
    elif ((game_over == 2)); then
        echo 'CONGRATULATIONS! You won!'
    else
        echo 'Enter move (row col): '
        echo 'Or flag/unflag (f row col): '
        echo 'Or type \"options\" for menu: '
    fi
}

# ##########################################
# GAME LOGIC FUNCTIONS
# ##########################################
# Function to reveal a cell
reveal_cell() {
    local row=\$1
    local col=\$2
    
    # Check if out of bounds or already revealed
    if ((row < 0 || row >= ROWS || col < 0 || col >= COLS || revealed[\$row,\$col] || flagged[\$row,\$col])); then
        return
    fi
    
    # Handle first move
    if ((first_move)); then
        first_move=0
        place_mines \$row \$col
    fi
    
    # Reveal the cell
    revealed[\$row,\$col]=1
    
    # Check if it's a mine
    if [[ \${board[\$row,\$col]} == 'M' ]]; then
        # Show all mines
        for ((i=0; i<ROWS; i++)); do
            for ((j=0; j<COLS; j++)); do
                if [[ \${board[\$i,\$j]} == 'M' ]]; then
                    revealed[\$i,\$j]=1
                fi
            done
        done
        game_over=1
        return
    fi
    
    # If it's a zero, reveal adjacent cells
    if [[ \${board[\$row,\$col]} == 0 ]]; then
        for ((i=row-1; i<=row+1; i++)); do
            for ((j=col-1; j<=col+1; j++)); do
                if ((i >= 0 && i < ROWS && j >= 0 && j < COLS && !revealed[\$i,\$j])); then
                    reveal_cell \$i \$j
                fi
            done
        done
    fi
    
    # Check if player has won
    check_win
}

# Function to toggle flag on a cell
toggle_flag() {
    local row=\$1
    local col=\$2
    
    # Check if out of bounds or already revealed
    if ((row < 0 || row >= ROWS || col < 0 || col >= COLS || revealed[\$row,\$col])); then
        return
    fi
    
    # Toggle flag
    flagged[\$row,\$col]=\$((1 - flagged[\$row,\$col]))
    
    # Check if player has won
    check_win
}

# Function to check if player has won
check_win() {
    for ((i=0; i<ROWS; i++)); do
        for ((j=0; j<COLS; j++)); do
            # If there's a non-mine cell that is not revealed, player hasn't won yet
            if [[ \${board[\$i,\$j]} != 'M' && \${revealed[\$i,\$j]} == 0 ]]; then
                return
            fi
        done
    done
    
    # All non-mine cells are revealed, player has won
    game_over=2
}

# ##########################################
# MAIN GAME LOOP
# ##########################################
init_board
while ((game_over == 0)); do
    display_board
    read -a input
    
    # ##########################################
    # MENU HANDLING
    # ##########################################
    if [[ \${input[0]} == 'options' ]]; then
        echo 'OPTIONS:'
        echo '1. Close (exit)'
        echo '2. Minimize (return to main menu)'
        echo -n 'Choose option: '
        read option
        
        case \$option in
            1)
                echo 'Closing task...'
                sleep 1
                exit 0
                ;;
            2)
                echo 'Minimizing task...'
                sleep 1
                exit 10  # Special exit code for minimize
                ;;
            *)
                echo 'Invalid option. Continuing...'
                sleep 1
                ;;
        esac
        continue
    elif [[ \${input[0]} == 'f' && \${#input[@]} == 3 ]]; then
        toggle_flag \${input[1]} \${input[2]}
    elif [[ \${#input[@]} == 2 ]]; then
        reveal_cell \${input[0]} \${input[1]}
    else
        echo 'Invalid input. Try again.'
        sleep 1
    fi
done

display_board
echo 'Game ended. Press Enter to exit...'
read
" || xterm -e "
# ----------------
# SCRIPT OVERVIEW:
# ----------------
# NexOS Task: Minesweeper
# A simple implementation of the classic minesweeper game
# ----------------

# ##########################################
# GAME LAUNCH
# ##########################################
# Launch minesweeper in a new terminal window
echo '=== NexOS Minesweeper ==='
echo 'Type \"options\" for menu or Enter moves to play'
echo '----------------------------------------'

# ##########################################
# GAME CONFIGURATION
# ##########################################
# Size of the game board
ROWS=8
COLS=8
MINES=10

# Initialize arrays
declare -A board
declare -A revealed
declare -A flagged

# Game state
game_over=0
mines_placed=0
first_move=1

# ##########################################
# GAME INITIALIZATION FUNCTIONS
# ##########################################
# Function to initialize the board
init_board() {
    for ((i=0; i<ROWS; i++)); do
        for ((j=0; j<COLS; j++)); do
            board[\$i,\$j]=0
            revealed[\$i,\$j]=0
            flagged[\$i,\$j]=0
        done
    done
}

# Function to place mines randomly
place_mines() {
    local row=\$1
    local col=\$2
    
    while ((mines_placed < MINES)); do
        local r=\$((RANDOM % ROWS))
        local c=\$((RANDOM % COLS))
        
        # Don't place a mine on the first click or adjacent cells
        if [[ \$r -ne \$row || \$c -ne \$col ]] && \
           [[ \$r -lt \$((row-1)) || \$r -gt \$((row+1)) || \
              \$c -lt \$((col-1)) || \$c -gt \$((col+1)) ]] && \
           [[ \${board[\$r,\$c]} != 'M' ]]; then
            board[\$r,\$c]='M'
            ((mines_placed++))
            
            # Update adjacent cell counts
            for ((i=r-1; i<=r+1; i++)); do
                for ((j=c-1; j<=c+1; j++)); do
                    if ((i >= 0 && i < ROWS && j >= 0 && j < COLS)) && [[ \${board[\$i,\$j]} != 'M' ]]; then
                        board[\$i,\$j]=\$((board[\$i,\$j] + 1))
                    fi
                done
            done
        fi
    done
}

# ##########################################
# GAME DISPLAY FUNCTIONS
# ##########################################
# Function to display the board
display_board() {
    clear
    echo '=== NexOS Minesweeper ==='
    echo 'Type \"options\" for menu or Enter moves to play'
    echo '----------------------------------------'
    
    echo '  |' $(seq -s ' ' 0 $((COLS-1)))
    echo '--+' $(printf -- '- %.0s' $(seq 0 $((COLS-1))))
    
    for ((i=0; i<ROWS; i++)); do
        echo -n \"\$i | \"
        for ((j=0; j<COLS; j++)); do
            if ((flagged[\$i,\$j])); then
                echo -n \"F \"
            elif ((revealed[\$i,\$j])); then
                if [[ \${board[\$i,\$j]} == 'M' ]]; then
                    echo -n \"* \"
                elif [[ \${board[\$i,\$j]} == 0 ]]; then
                    echo -n \". \"
                else
                    echo -n \"\${board[\$i,\$j]} \"
                fi
            else
                echo -n \"# \"
            fi
        done
        echo
    done
    
    if ((game_over == 1)); then
        echo 'GAME OVER! You hit a mine!'
    elif ((game_over == 2)); then
        echo 'CONGRATULATIONS! You won!'
    else
        echo 'Enter move (row col): '
        echo 'Or flag/unflag (f row col): '
        echo 'Or type \"options\" for menu: '
    fi
}

# ##########################################
# GAME LOGIC FUNCTIONS
# ##########################################
# Function to reveal a cell
reveal_cell() {
    local row=\$1
    local col=\$2
    
    # Check if out of bounds or already revealed
    if ((row < 0 || row >= ROWS || col < 0 || col >= COLS || revealed[\$row,\$col] || flagged[\$row,\$col])); then
        return
    fi
    
    # Handle first move
    if ((first_move)); then
        first_move=0
        place_mines \$row \$col
    fi
    
    # Reveal the cell
    revealed[\$row,\$col]=1
    
    # Check if it's a mine
    if [[ \${board[\$row,\$col]} == 'M' ]]; then
        # Show all mines
        for ((i=0; i<ROWS; i++)); do
            for ((j=0; j<COLS; j++)); do
                if [[ \${board[\$i,\$j]} == 'M' ]]; then
                    revealed[\$i,\$j]=1
                fi
            done
        done
        game_over=1
        return
    fi
    
    # If it's a zero, reveal adjacent cells
    if [[ \${board[\$row,\$col]} == 0 ]]; then
        for ((i=row-1; i<=row+1; i++)); do
            for ((j=col-1; j<=col+1; j++)); do
                if ((i >= 0 && i < ROWS && j >= 0 && j < COLS && !revealed[\$i,\$j])); then
                    reveal_cell \$i \$j
                fi
            done
        done
    fi
    
    # Check if player has won
    check_win
}

# Function to toggle flag on a cell
toggle_flag() {
    local row=\$1
    local col=\$2
    
    # Check if out of bounds or already revealed
    if ((row < 0 || row >= ROWS || col < 0 || col >= COLS || revealed[\$row,\$col])); then
        return
    fi
    
    # Toggle flag
    flagged[\$row,\$col]=\$((1 - flagged[\$row,\$col]))
    
    # Check if player has won
    check_win
}

# Function to check if player has won
check_win() {
    for ((i=0; i<ROWS; i++)); do
        for ((j=0; j<COLS; j++)); do
            # If there's a non-mine cell that is not revealed, player hasn't won yet
            if [[ \${board[\$i,\$j]} != 'M' && \${revealed[\$i,\$j]} == 0 ]]; then
                return
            fi
        done
    done
    
    # All non-mine cells are revealed, player has won
    game_over=2
}

# ##########################################
# MAIN GAME LOOP
# ##########################################
init_board
while ((game_over == 0)); do
    display_board
    read -a input
    
    # ##########################################
    # MENU HANDLING
    # ##########################################
    if [[ \${input[0]} == 'options' ]]; then
        echo 'OPTIONS:'
        echo '1. Close (exit)'
        echo '2. Minimize (return to main menu)'
        echo -n 'Choose option: '
        read option
        
        case \$option in
            1)
                echo 'Closing task...'
                sleep 1
                exit 0
                ;;
            2)
                echo 'Minimizing task...'
                sleep 1
                exit 10  # Special exit code for minimize
                ;;
            *)
                echo 'Invalid option. Continuing...'
                sleep 1
                ;;
        esac
        continue
    elif [[ \${input[0]} == 'f' && \${#input[@]} == 3 ]]; then
        toggle_flag \${input[1]} \${input[2]}
    elif [[ \${#input[@]} == 2 ]]; then
        reveal_cell \${input[0]} \${input[1]}
    else
        echo 'Invalid input. Try again.'
        sleep 1
    fi
done

display_board
echo 'Game ended. Press Enter to exit...'
read
"

exit 0
