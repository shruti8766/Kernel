"""
# Kernel Security Toolkit

**A single Linux kernel module that can log, block, keylog, and detect hidden processes.**  
Built for learning low-level systems security.

---

## 📖 What is the Kernel? (Simple Definition)

The **kernel** is the core of your operating system. It runs in a special CPU mode called **Ring 0** (also called *kernel mode*). It has unlimited power: it controls hardware, memory, processes, and security. Normal programs (like your browser, terminal, `cat`) run in **Ring 3** (*user mode*) and must ask the kernel for everything via **system calls**.

A **kernel module** is a piece of code that you can insert into the running kernel (without rebooting). It runs in Ring 0, so it can do anything – including monitoring, blocking, or hiding.

---

## 🎯 What This Toolkit Does

This module (`sec_toolkit.ko`) can perform six security tasks. You choose which ones by passing **parameters** when you load it.

| Feature | Parameter | What it does |
| :--- | :--- | :--- |
| **File Logger** | `logfile=1` | Prints every file opened (to kernel log) |
| **File Blocker** | `block_file=/path` | Prevents opening that specific file |
| **Directory Blocker** | `block_dir=/path` | Prevents opening any file inside that directory |
| **Keylogger** | `keylog=1` | Records every keystroke typed in any terminal |
| **One-time Hidden Process Scan** | `scan_hidden=1` | Compares two process lists, reports hidden PIDs |
| **Real-time Monitor** | `rt_monitor=1` | Runs a background thread that scans for hidden processes every 5 seconds |

You can combine them, e.g.:

```bash
sudo insmod sec_toolkit.ko logfile=1 block_dir=/etc keylog=1
```

---

## 🛠️ How It Works (Very Simple)

The module hooks (intercepts) the `openat` system call – the door used by every program to open files.

→ When a program tries to open a file, our code runs first.  
→ We can log the filename or block it by returning an error (`Permission denied`).

The module also hooks the `write` system call on file descriptors `1` (stdout) and `2` (stderr) – that’s what terminals use.

→ When you type, the shell echoes your keys back to the screen.  
→ We capture that echo and print the character to the kernel log → keylogger.

To find hidden processes, we use two methods:

1. Walk the kernel’s linked list of processes (what `ps` uses).  
2. Scan the PID bitmap (a low-level array that the kernel uses to allocate PIDs).

→ Any PID that exists in the bitmap but not in the linked list is a hidden process (classic rootkit trick).

A kernel thread runs in the background when you enable real-time monitor. It repeats the hidden-process scan every 5 seconds.

---

## 📦 Requirements

- Kali Linux (or any Linux, but tested on Kali)
- Root access (you'll use `sudo`)
- Kernel headers installed:

```bash
sudo apt install linux-headers-$(uname -r)
```

---

## 🚀 Build & Test Steps

### 1. Clone or create the repository

```bash
git clone https://github.com/yourname/kernel-security-toolkit
cd kernel-security-toolkit
```

### 2. Compile the module

```bash
make
```

If successful, you'll get a file `sec_toolkit.ko`.

---

## 3. Test each feature (run as root or with sudo)

### a. File Logger

```bash
sudo insmod sec_toolkit.ko logfile=1
cat /etc/passwd
sudo dmesg | grep OPEN
sudo rmmod sec_toolkit
```

---

### b. Block a specific file

```bash
sudo insmod sec_toolkit.ko block_file=/etc/passwd
cat /etc/passwd           # Should show Permission denied
sudo dmesg | grep BLOCKED
sudo rmmod sec_toolkit
```

---

### c. Block a directory

```bash
sudo insmod sec_toolkit.ko block_dir=/etc
cat /etc/hosts            # Permission denied
cat ~/.bashrc             # Works fine
sudo rmmod sec_toolkit
```

---

### d. Keylogger

```bash
sudo insmod sec_toolkit.ko keylog=1

# In another terminal, type "hello"

sudo dmesg | grep KEY
sudo rmmod sec_toolkit
```

---

### e. One-time hidden process scan

```bash
sudo insmod sec_toolkit.ko scan_hidden=1
sudo dmesg | tail -30
sudo rmmod sec_toolkit
```

---

### f. Real-time monitor

```bash
sudo insmod sec_toolkit.ko rt_monitor=1

# Wait 10-15 seconds

sudo dmesg | tail -40     # You'll see repeated scans
sudo rmmod sec_toolkit
```

---

### g. Combine features

```bash
sudo insmod sec_toolkit.ko logfile=1 block_dir=/etc keylog=1

# Now test:
# cat /etc/passwd (blocked)
# type something (keylogger)
# then check dmesg

sudo dmesg | tail -20
sudo rmmod sec_toolkit
```

---

## 📝 Code Explanation (Key Concepts)

- `module_param` – Lets you pass values when loading the module.
- `kprobe` – Kernel probe; allows you to insert your code before or after any kernel function.
- `__x64_sys_openat` – The actual kernel function that handles the `openat` system call (64-bit).
- `strncpy_from_user` – Safely copies a string from user space to kernel space.
- `regs->ax = -13` – Sets the return value to `-EACCES` (`Permission denied`).
- `return 1` from a pre-handler – Skips the real kernel function.
- `for_each_process(task)` – Iterates over all processes in the kernel’s linked list.
- `find_get_pid()` – Looks up a PID in the bitmap.
- `kthread_run()` – Creates a new kernel thread (background task).

---

## 🧠 Glossary of Terms (Simple Definitions)

| Term | Definition |
|---|---|
| Kernel | The boss of the OS; runs in Ring 0. |
| Ring 0 | The most privileged CPU mode; kernel mode. |
| Ring 3 | Unprivileged CPU mode; user mode. |
| System call (syscall) | A request from a user program to the kernel. |
| Kernel module | Code you can insert into the kernel at runtime. |
| kprobe | A tool to dynamically insert code into any kernel function. |
| Process | A running program. Each has a unique PID. |
| PID | Process ID; a number that identifies a process. |
| task_struct | The kernel’s data structure that holds all info about a process. |
| Linked list | A chain of data structures; the kernel links all `task_struct` together. |
| PID bitmap | A low-level array where each bit represents whether a PID is in use. |
| Hidden process | A process that is not in the linked list (so `ps` can’t see it) but still runs. |
| Kernel thread | A lightweight thread that runs only in kernel mode. |
| printk | The kernel’s version of `printf`; prints to the kernel log. |
| dmesg | Command to view the kernel log. |

---

## 📚 References & Further Learning

- Linux Kernel Module Programming Guide (free book)
- Kprobes Documentation
- Understanding the Linux Kernel (O'Reilly)
- Linux Inside – Kernel Internals

---

## 🙌 Acknowledgements

Built as part of a self-study low-level systems security path.  
All code is original and licensed GPLv2.

---

## 📜 License

GPL v2 (because kernel modules must be GPL compatible).

---

## 📄 2. `Makefile`

```make
obj-m += sec_toolkit.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

"""