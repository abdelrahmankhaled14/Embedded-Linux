#!/bin/bash

file="$HOME/.bashrc"

# Check if it exists
if [ -f "$file" ]; then
    echo "file exists"
    echo "export HELLO=$(hostname)" >> "$file"
    echo "LOCAL=$(whoami)" >> "$file"
    echo "done"
else
    echo "❌ The file '.bashrc' does NOT exist in your home directory."
fi
gnome-terminal


