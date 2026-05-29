#!/bin/bash
# Test one-time hidden process scan

echo "=== Testing Hidden Process Scan (one-time) ==="
sudo insmod ../sec_toolkit.ko scan_hidden=1
echo "Waiting for scan to complete..."
sleep 2
sudo dmesg | grep -E "Hidden process scan|HIDDEN PROCESS|No hidden processes" | tail -10
sudo rmmod sec_toolkit
echo "=== Test finished ==="