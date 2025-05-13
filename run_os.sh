#!/bin/bash

# ----------------
# SCRIPT OVERVIEW:
# ----------------
# This script prepares and launches the NexOS operating system simulator
# Functions:
# - Compiles the main executable
# - Ensures all task scripts have execute permissions
# - Runs the NexOS simulator
# ----------------

# ##########################################
# COMPILATION
# ##########################################
# Compile the project using the Makefile
make

# ##########################################
# PERMISSIONS SETUP
# ##########################################
# Make sure all task scripts are executable
chmod +x tasks/*

# ##########################################
# LAUNCH OPERATING SYSTEM
# ##########################################
# Run the OS simulator
./nexos
