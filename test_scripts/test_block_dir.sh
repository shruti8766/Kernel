#!/bin/bash
# Test blocking a whole directory (/etc)

echo "=== Testing Block Directory ==="
sudo insmod ../sec_toolkit.ko block_dir=/etc
echo "Trying to read /etc/hosts (should be blocked)..."
cat /etc/hosts
if [ $? -ne 0 ]; then
    echo "Success: Permission denied."
else
    echo "FAIL: Directory was not blocked."
fi
echo "Trying to read ~/.bashrc (should work)..."
cat ~/.bashrc > /dev/null
if [ $? -eq 0 ]; then
    echo "Success: Outside directory works."
else
    echo "Unexpected error."
fi
sudo dmesg | grep "BLOCKED:" | tail -3
sudo rmmod sec_toolkit
echo "=== Test finished ==="