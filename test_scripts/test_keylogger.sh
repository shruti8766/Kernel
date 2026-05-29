#!/bin/bash
# Test keylogger (you must type something in another terminal)

echo "=== Testing Keylogger ==="
echo "Loading module..."
sudo insmod ../sec_toolkit.ko keylog=1
echo "Open a second terminal and type some letters (e.g., 'hello')."
echo "Press ENTER here when you have typed something in the other terminal."
read -p "Press ENTER to continue..."
echo "Checking kernel log for KEY messages:"
sudo dmesg | grep "KEY:" | tail -10
sudo rmmod sec_toolkit
echo "=== Test finished ==="