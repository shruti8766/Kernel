#!/bin/bash
# Test real-time monitor (runs for 15 seconds)

echo "=== Testing Real-time Monitor ==="
sudo insmod ../sec_toolkit.ko rt_monitor=1
echo "Monitor running. Waiting 15 seconds to see scans..."
sleep 15
echo "Showing recent kernel log entries from the monitor:"
sudo dmesg | grep -E "Hidden process scan|HIDDEN PROCESS|No hidden processes" | tail -15
sudo rmmod sec_toolkit
echo "=== Test finished ==="