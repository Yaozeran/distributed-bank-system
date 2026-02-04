#!/bin/bash

# Ensure bin directory exists
mkdir -p bin

# Compile
echo "Compiling..."
javac -d bin -sourcepath src -encoding UTF-8 src/client/ClientMain.java

# Run if compilation succeeded
if [ $? -eq 0 ]; then
    echo "Running..."
    java -cp bin client.ClientMain
else
    echo "Compilation failed!"
    exit 1
fi
