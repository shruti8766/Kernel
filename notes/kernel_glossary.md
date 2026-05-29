"""

# Kernel Glossary – Very Simple Definitions

1. **Kernel** – The core of the OS. Runs in Ring 0.
2. **Ring 0** – The “god mode” of the CPU. Only kernel runs here.
3. **Ring 3** – The restricted mode. Normal apps run here.
4. **System Call** – A door from Ring 3 to Ring 0.
5. **Kernel Module** – A program you can plug into the kernel while it’s running.
6. **kprobe** – A tool to insert your code into any kernel function (like a hook).
7. **printk** – The kernel’s print function.
8. **dmesg** – Command to see kernel print messages.
9. **Process** – A running program. Has a unique PID.
10. **task_struct** – The kernel’s data structure for a process.
11. **Linked list** – A chain of processes; `for_each_process` walks it.
12. **PID bitmap** – A table the kernel uses to track used PIDs.
13. **Hidden process** – A process that is not in the linked list (so `ps` can’t see it).
14. **Kernel thread** – A thread that runs only in kernel space.
15. **insmod** – Command to load a kernel module.
16. **rmmod** – Command to remove a kernel module.
17. **lsmod** – Command to list loaded modules.
18. **modinfo** – Show information about a module.
19. **tainted kernel** – A kernel with a non-GPL or unsigned module loaded.

---

# 🖥️ How to Create the Repository on Your Computer (VS Code)

## Step 1: Create the main folder

```bash
mkdir kernel-security-toolkit
cd kernel-security-toolkit
```

---

## Step 2: Create each file

For each file, run `nano <filename>` (or use VS Code). Copy the corresponding content from above.

* `README.md`
* `Makefile`
* `sec_toolkit.c`

(optional) `test_scripts/` and `notes/` folders and their files.

---

## Step 3: Initialize git and push to GitHub

```bash
git init
git add .
git commit -m "Initial commit: kernel security toolkit"
git remote add origin https://github.com/yourusername/kernel-security-toolkit.git
git push -u origin main
```

"""
