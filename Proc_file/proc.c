#include <linux/kernel.h>
#include <linux/module.h> 
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h> 
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/mm.h> 
#include <linux/slab.h> 
#include <asm/io.h> 
#include <linux/types.h> 
#include <linux/delay.h> 
#include <linux/atomic.h>
#include <linux/minmax.h> 
#include <linux/err.h> 
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    #define  HAVE_PROC_OPS
#endif 

#define PROCFS_NAME "helloworld"
#define PROCFS_BUFFER 1024 

struct procfs_t 
{
    char * name; 
    char buffer[PROCFS_BUFFER];
    unsigned long profs_buffer_size; 
};

static struct procfs_t procfs = {
    .name = PROCFS_NAME, 
    .profs_buffer_size = 0,
};

static struct proc_dir_entry *our_proc_file; 

static ssize_t procfile_read(struct file * file_p, char __user *buffer, size_t buf_len, loff_t * offset)
{
    char str[] = "helloworld!\n"; 
    int len = sizeof(str); 
    ssize_t ret = len; 

    if (*offset >= len)
        return 0;

    if (copy_to_user(buffer, str, len))
        return -EFAULT;

    *offset += len;
    return len;
}

static ssize_t procfile_write(struct file *file_p, const char __user * buffer, size_t len, loff_t *offset)
{
    procfs.profs_buffer_size = len; 

    if(procfs.profs_buffer_size >= PROCFS_BUFFER)
    {
        procfs.profs_buffer_size = PROCFS_BUFFER - 1; 
    }
    if(copy_from_user(procfs.buffer, buffer, procfs.profs_buffer_size))
    {
        return -EFAULT; 
    }
    
    procfs.buffer[procfs.profs_buffer_size] = '\0'; 
    *offset += procfs.profs_buffer_size; 
    pr_info("procfile write %s\n", procfs.buffer);

    return procfs.profs_buffer_size; 
}

static int procfile_open(struct inode *inode, struct file *file_p)
{
    if(!try_module_get(THIS_MODULE))
    {
        return -ENODEV; 
    }
    return 0; 
}
static int procfile_release(struct inode *inode, struct file * file_p)
{
    module_put(THIS_MODULE);
    return 0; 
}
#ifdef HAVE_PROC_OPS
static const struct proc_ops pro_file_fops = {
    .proc_open = procfile_open,
    .proc_release = procfile_release, 
    .proc_read = procfile_read, 
    .proc_write = procfile_write, 
}; 
#else 

static const struct file_operation pro_file_fops = {
    .open = procfile_open, 
    .release = procfile_release,
    .read = procfile_read, 
    .write = procfile_write, 
}; 
#endif

static int __init procfs1_init(void)
{
    our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &pro_file_fops); 
    if(NULL == our_proc_file)
    {
        pr_alert("Error: Could not initialize /proc/%s\n", PROCFS_NAME); 
        return -ENOMEM; 
    }
    
    pr_info("/proc/%s created\n", PROCFS_NAME);
    return 0;
}
static void __exit procfs1_exit(void)
{
    proc_remove(our_proc_file); 
    pr_info("/proc/%s removed\n", PROCFS_NAME); 
}

module_init(procfs1_init);
module_exit(procfs1_exit); 

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Chrinovic M"); 

