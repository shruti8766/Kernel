#!/bin/bash
# Test blocking a specific file (/etc/passwd)

echo "=== Testing Block File ==="
sudo insmod ../sec_toolkit.ko block_file=/etc/passwd
echo "Trying to read /etc/passwd (should be blocked)..."
cat /etc/passwd
if [ $? -ne 0 ]; then
    echo "Success: Permission denied."
else
    echo "FAIL: File was not blocked."
fi
sudo dmesg | grep "BLOCKED:" | tail -3
sudo rmmod sec_toolkit
echo "=== Test finished ==="