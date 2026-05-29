#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static int logfile = 0;
static char *block_file = NULL;
static char *block_dir = NULL;
static int keylog = 0;
static int scan_hidden = 0;
static int rt_monitor = 0;

module_param(logfile, int, 0644);
module_param(block_file, charp, 0644);
module_param(block_dir, charp, 0644);
module_param(keylog, int, 0644);
module_param(scan_hidden, int, 0644);
module_param(rt_monitor, int, 0644);

static struct task_struct *monitor_thread = NULL;
static int stop_monitor = 0;

static int starts_with(const char *str, const char *prefix)
{
    if (!str || !prefix) return 0;
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

static int openat_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    char __user *filename_ptr = (char __user *)regs->si;
    char filename[256];
    int ret;

    if (!filename_ptr) return 0;
    ret = strncpy_from_user(filename, filename_ptr, sizeof(filename)-1);
    if (ret <= 0) return 0;
    filename[ret] = '\0';

    if (logfile)
        printk(KERN_INFO "OPEN: %s (PID %d)\n", filename, current->pid);

    if (block_file && strcmp(filename, block_file) == 0) {
        printk(KERN_WARNING "BLOCKED: %s\n", filename);
        regs->ax = -13;
        return 1;
    }

    if (block_dir && starts_with(filename, block_dir)) {
        printk(KERN_WARNING "BLOCKED: %s (inside %s)\n", filename, block_dir);
        regs->ax = -13;
        return 1;
    }
    return 0;
}

static int write_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    int fd = (int)regs->di;
    char __user *buf = (char __user *)regs->si;
    size_t count = (size_t)regs->dx;
    char c;
    int i;

    if (!keylog) return 0;
    if (fd != 1 && fd != 2) return 0;

    for (i = 0; i < count; i++) {
        if (copy_from_user(&c, buf + i, 1)) break;
        if (c >= 0x20 && c <= 0x7E)
            printk(KERN_INFO "KEY: %c\n", c);
        else if (c == '\n')
            printk(KERN_INFO "KEY: [ENTER]\n");
        else if (c == '\b' || c == 0x7F)
            printk(KERN_INFO "KEY: [BACKSPACE]\n");
    }
    return 0;
}

static void detect_hidden_processes(void)
{
    struct task_struct *task;
    struct pid *pid_struct;
    int pid_number, found, hidden_count = 0;

    printk(KERN_INFO "=== Hidden process scan ===\n");
    for (pid_number = 1; pid_number < 10000; pid_number++) {
        pid_struct = find_get_pid(pid_number);
        if (!pid_struct) continue;
        found = 0;
        for_each_process(task) {
            if (task->pid == pid_number) {
                found = 1;
                break;
            }
        }
        if (!found) {
            printk(KERN_WARNING "HIDDEN PROCESS: PID %d\n", pid_number);
            hidden_count++;
        }
        put_pid(pid_struct);
    }
    if (hidden_count == 0)
        printk(KERN_INFO "No hidden processes found.\n");
}

static int monitor_thread_func(void *data)
{
    while (!stop_monitor) {
        detect_hidden_processes();
        ssleep(5);
    }
    return 0;
}

static void start_monitor_thread(void)
{
    if (rt_monitor) {
        monitor_thread = kthread_run(monitor_thread_func, NULL, "sec_monitor");
        if (IS_ERR(monitor_thread))
            printk(KERN_ERR "Failed to start monitor thread\n");
        else
            printk(KERN_INFO "Real-time monitor started (scan every 5s)\n");
    }
}

static void stop_monitor_thread(void)
{
    if (monitor_thread) {
        stop_monitor = 1;
        kthread_stop(monitor_thread);
        monitor_thread = NULL;
    }
}

static struct kprobe openat_kp = {
    .symbol_name = "__x64_sys_openat",
    .pre_handler = openat_pre_handler,
};

static struct kprobe write_kp = {
    .symbol_name = "__x64_sys_write",
    .pre_handler = write_pre_handler,
};

static int __init sec_toolkit_init(void)
{
    int ret;

    ret = register_kprobe(&openat_kp);
    if (ret < 0)
        printk(KERN_ERR "Failed to hook openat\n");
    else
        printk(KERN_INFO "openat hook installed\n");

    if (keylog) {
        ret = register_kprobe(&write_kp);
        if (ret < 0)
            printk(KERN_ERR "Failed to hook write\n");
        else
            printk(KERN_INFO "write hook installed (keylogger active)\n");
    }

    if (scan_hidden)
        detect_hidden_processes();

    if (rt_monitor)
        start_monitor_thread();

    printk(KERN_INFO "sec_toolkit loaded. Features: logfile=%d block_file=%s block_dir=%s keylog=%d scan=%d rt=%d\n",
           logfile, block_file ? block_file : "none", block_dir ? block_dir : "none",
           keylog, scan_hidden, rt_monitor);
    return 0;
}

static void __exit sec_toolkit_exit(void)
{
    stop_monitor_thread();
    unregister_kprobe(&openat_kp);
    if (keylog)
        unregister_kprobe(&write_kp);
    printk(KERN_INFO "sec_toolkit unloaded\n");
}

module_init(sec_toolkit_init);
module_exit(sec_toolkit_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("All-in-one kernel security toolkit");