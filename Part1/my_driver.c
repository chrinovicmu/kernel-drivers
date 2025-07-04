#include <linux/module.h>
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

#define MINOR_COUNT 4 
#define DEVICE_BUFFER_SIZE 256 

#define MY_IOCTL_SET_NUM _IOW('a', 1, int)
#define MY_IOCTL_GET_NUM _IOR('a', 2, int)
static int stored_number =0;

/*read*/ 
#define BUFFER_SIZE PAGE_SIZE
static char * kernel_buffer; 

/*poll*/ 
DECLARE_WAIT_QUEUE_HEAD(my_wait_queue); 
static int data_ready = 0; 

static dev_t dev_num;  
static struct cdev my_cdev;
static void fill_device_buffer(void); 


struct my_device 
{
    char device_buffer[DEVICE_BUFFER_SIZE] 
    size_t device_data_size; 
    int error_pending; 
}; 

static struct my_device my_device_data = {
    .device_data_size = 0, 
    .error_pending = 0. 
};

static int my_open(struct inode *inode, struct file *file)
{
    file->private_data = &my_device_data; 

    int minor = iminor(inode); 
    printk(KERN_INFO "my_driver: Device opened, minor = %d\n", minor); 
    fill_device_buffer();
    return 0; 
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "my_driver :Device closed\n");
    return 0; 
}

static void fill_device_buffer(void)
{
    const char *msg= "hello world"; 
    size_t len = strlen(msg); 

    if(len > DEVICE_BUFFER_SIZE )
    {
        len = DEVICE_BUFFER_SIZE; 
    }
    
    memcpy(device_buffer, msg, len); 
    device_data_size = len; 
}

static unsigned int my_poll(struct file *file, struct poll_table_struct *wait)
{
    unsigned int mask= 0; 

    /*register process in wait wait queue*/ 
    poll_wait(file, &my_wait_queue, wait); 

    if(data_ready)
    {
        mask |= POLLIN | POLLRDNORM; 
    }
    return mask; 
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int value; 

    switch (cmd) 
    {
        case MY_IOCTL_SET_NUM:
            if(copy_from_user(&value, (int __user *)arg, sizeof(int)))
            {
                return -EFAULT;
            }
            stored_number = value; 
            printk(KERN_INFO "stored number set to %d\n", stored_number);
            return 0; 

        case MY_IOCTL_GET_NUM:

            if(copy_to_user((int __user *)arg, &stored_number, sizeof(int)))
            {
                return -EFAULT; 
            }
            printk(KERN_INFO "sent to user %d\n", stored_number); 
            return 0;

        default:
            return -ENOTTY; 
    }
}

static int my_mmap(struct file * file, struct vm_area_struct *vma)
{
    unsigned long pfn; 
    unsigned long size = vma->vm_end - vma->vm_start;

    if(size > BUFFER_SIZE )
    {
        printk(KERN_ERR "requested mmap size too large\n"); 
        return -EINVAL; 
    }

    pfn = virt_to_phys(kernel_buffer) >> PAGE_SHIFT; 

    if(remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot))
    {
        printk(KERN_ERR "remap_pfn_rage failed\n"); 
        return -EAGAIN; 
    }

    printk(KERN_INFO "mmap success: mapped kernel buffe to %p to user space", kernel_buffer); 
    return 0; 
}


static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    struct my_device *dev = file->private_data; 

    if(dev->error_pending)
    {
        int err = dev->error_pending; 
        dev->error_pending = 0; 
        return err; 
    }

    size_t avaiable = dev->device_data_size - *pos; 

    if(avaiable ==0)
    {
        return 0; 
    }

    size_t bytes_to_read = (count < avaiable) ? count : avaiable; 

    unsigned long not_copied = copy_to_user(buf, dev->device_buffer + *pos, bytes_to_read); 
    ssize_t copied = bytes_to_read - not_copied; 

    *pos += bytes_to_read;

    if(not_copied >0)
    {
        dev->error_pending = -EFAULT; 
        printk(KERN_WARNING "my_read: partial copy (%zd/%zd), will return error next time\n",
               copied, bytes_to_read); 
    }
 
    printk(KERN_INFO "my_read: read %zd bytes, new pos %lld\n", bytes_to_read, *pos);

    return bytes_to_read; 
}

static struct file_operations fops = {
    .owner  = THIS_MODULE, 
    .open = my_open, 
    .release = my_release, 
    .read = my_read, 
    .poll = my_poll, 
    .unlocked_ioctl = my_ioctl, 
    .mmap = my_mmap, 
};

static int __init my_driver_init(void)
{
    int result;

    result = alloc_chrdev_region(&dev_num, 0, MINOR_COUNT, "my_driver"); 
    if(result < 0)
    {
        printk(KERN_INFO "my_driver: Failed to allocate char dev region\n"); 
        return result; 
    }

    printk(KERN_INFO "my_driver: Allocated Major = %d, Minor = %d\n", MAJOR(dev_num), MINOR(dev_num)); 

    /*initialize characteer device */ 
    cdev_init(&my_cdev, &fops); 
    my_cdev.owner = THIS_MODULE; 

    result = cdev_add(&my_cdev, dev_num, MINOR_COUNT); 
    
    if(result < 0)
    {
        unregister_chrdev_region(dev_num, MINOR_COUNT); 
        printk(KERN_INFO "my_driver: Failed to add cdev\n"); 
        return result; 
    }

    printk(KERN_INFO "my_driver: Char device registered\n"); 
    return 0; 
}

static void __exit my_driver_exit(void)
{
    cdev_del(&my_cdev); 
    unregister_chrdev_region(dev_num, MINOR_COUNT);
    printk(KERN_INFO "my_driver: Char device unregistered\n");
}

module_init(my_driver_init); 
module_exit(my_driver_exit); 

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Chrinovic M"); 
MODULE_DESCRIPTION("Simple demo of alloc_chrdev_region and releasing dev_t");
