#!/bin/bash

# Check if a port number is provided as an argument
if [ $# -eq 0 ]; then
    echo "Usage: $0 <port>"
    exit 1
fi

# Assign the first argument (port number) to a variable
port=$1

# Make the application
make

# Run the main_app executable with the specified port
./main_app --docroot . --http-listen 0.0.0.0:$port