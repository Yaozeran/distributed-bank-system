#!/bin/bash

# Ensure bin directory exists
mkdir -p bin

# Compile
echo "Compiling GUI..."
javac -d bin -sourcepath src -encoding UTF-8 src/gui/LoginFrame.java

# Run if compilation succeeded
if [ $? -eq 0 ]; then
    echo "Running GUI..."
    java -cp bin gui.LoginFrame
else
    echo "Compilation failed!"
    exit 1
fi
