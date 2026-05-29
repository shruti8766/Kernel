#!/bin/bash
# Test the File Logger feature

echo "=== Testing File Logger ==="
sudo insmod ../sec_toolkit.ko logfile=1
echo "Module loaded. Opening /etc/passwd..."
cat /etc/passwd > /dev/null
echo "Checking kernel log for OPEN messages:"
sudo dmesg | grep "OPEN:" | tail -5
sudo rmmod sec_toolkit
echo "=== Test finished ==="