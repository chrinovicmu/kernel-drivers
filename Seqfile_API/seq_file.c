#include <linux/kernel.h>
#include <linux/module.h> 
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h> 
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/seq_file.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    #define HAVE_PROC_OPS 
#endif 

#defien PROC_NAME "iter"

static void *my_seq_start(struct seq_file *s, loff_t *offset)
{
    static unsigned long counter = 0; 

    if(*offset == 0)
    {
        return &counter;
    }
    *offset =0; 
    return NULL; 
}

static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    unsigned long *temp_v = (unsigned long *)v;
    (*temp_v)++;
    (*pos)++; 
    return NULL; 
}

static void my_seq_stop(struct seq_file *, void *v )
{

}

static int my_seq_show(struct seq_file *s, void *v)
{
    loff_t *spos = (loff_t *)v;
    seq_printf(s, "%Ld\n", *spos); 
    return 0; 
}
static struct seq_operations my_seq_ops = {
    .start = my_seq_start, 
    .next = my_seq_next, 
    .stop = my_seq_stop, 
    .show = my_seq_show, 
}; 

static int my_open(struct inode *inode , struct file *file_p)
{
    seq_open(file, &my_seq_ops); 
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops my_file_ops = {
    .proc_open = my_open, 
    .read = seq_read, 
    .llseek = seq_lseek, 
    .release = seq_release, 
}; 

static int __init procfs4_init(void)
{
    struct proc_dir_entry *entry; 

    if(NULL == entry)
    {
        pr_debug("Error: Could not initialize /proc/%s\n", PROC_NAME); 
        return -ENONEM; 
    }

    return 0; 
}

static void __exit procfs4_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    pr_debug("proc/%s removed \n", PROC_NAME);
}

module_init(procfs4_init); 
module_exit(procfs4_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chrinovic M"); 


