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
#include <linux/delay.h> 
#include <linux/atomic.h>
#include <linux/minmax.h> 
#include <linux/err.h> 
#include <linux/version.h>
#define BUF_LEN 255 

#define MINOR_COUNT 3 

#define DEVICE_NAME "my_driver"

static dev_t dev_num; 
static struct cdev my_cdev; 

static int my_open(struct inode *, struct file*); 
static int my_release(struct inode *, struct file *); 
static ssize_t my_read(struct file *, char __user *, size_t , loff_t *); 
static ssize_t my_write(struct file *, const char __user *, size_t, loff_t*); 

enum state
{
    CDEV_NOT_USED, 
    CDEV_EXCLUSIVE_OPEN, 
}; 

static struct class *my_class;
static struct device *my_device; 

struct my_device
{
    atomic_t already_open; 
    char device_buffer[BUF_LEN]; 
    size_t device_data_size; 
    int error_pending; 
};

static struct my_device my_dev = {
    .already_open = ATOMIC_INIT(CDEV_NOT_USED),
    .error_pending = 0, 
    .device_data_size = 0,  
}; 

static struct file_operations fops = {
    .owner = THIS_MODULE, 
    .open = my_open, 
    .release = my_release, 
    .read = my_read, 
    .write = my_write, 
};

static int my_open(struct inode *inode, struct file * file_p)
{
    file_p->private_data = &my_dev; 
    static int counter = 0; 

    if(atomic_cmpxchg(&my_dev.already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
    {
        return -EBUSY; 
    }
    my_dev.device_data_size = snprintf(my_dev.device_buffer, BUF_LEN, "Already told you %d times hello world \n", counter++);

    if(!try_module_get(THIS_MODULE))
    {
        pr_err("Failed to get module reference\n"); 
        return -ENODEV; 
    }

    pr_info("my_driver: Device opened successfully\n"); 
    return 0; 
}

static int my_release(struct inode * inode, struct file *file_p)
{
    atomic_set(&my_dev.already_open, CDEV_NOT_USED); 

    module_put(THIS_MODULE);

    pr_info("my_driver: Device closed successfully\n"); 
    return 0; 
}

static ssize_t my_read(struct file * file_p, char __user *user_buffer, size_t len, loff_t *offset)
{
    struct my_device *my_dev = file_p->private_data; 

    if(!my_dev || !user_buffer)
    {
        return -EINVAL; 
    }

    if(my_dev->error_pending)
    {
        int err = my_dev->error_pending; 
        my_dev->error_pending = 0;
        return err; 
    }

    if(*offset > my_dev->device_data_size)
    {
        pr_info("my_driver: Read at EOF, offset %lld, size %zu\n", *offset, my_dev->device_data_size); 
        return 0; 
    }

    size_t available = my_dev->device_data_size - *offset;

    size_t bytes_to_read = min(len, available); 

    unsigned long not_copied = copy_to_user(user_buffer, my_dev->device_buffer + *offset, bytes_to_read); 
    size_t copied = bytes_to_read - not_copied;

    *offset += copied; 

    if(copied == 0 && bytes_to_read > 0)
    {
        pr_err("my_driver: Failed to copy any data to user space\n"); 
        return -EFAULT; 
    }

    pr_info("my_read: read %zd bytes, new offset %lld\n", copied, *offset); 
    return copied; 
}

static ssize_t my_write(struct file * file_p, const char __user *user_buffer, size_t len, loff_t *offset)
{
    pr_alert("Sorry, this operation is not supported\n"); 
    return - EINVAL; 
}
static int __init my_driver_init(void)
{
    int alloc_major; 
    alloc_major = alloc_chrdev_region(&dev_num, 0, MINOR_COUNT, DEVICE_NAME); 

    if(alloc_major < 0)
    {
        pr_info("my_driver: Failed to allocate char dev region\n"); 
        return alloc_major;  
    }

    cdev_init(&my_cdev, &fops); 
    my_cdev.owner = THIS_MODULE;

    int add_cdev; 
    add_cdev = cdev_add(&my_cdev, dev_num, MINOR_COUNT); 

    if (add_cdev)
    {
        unregister_chrdev_region(dev_num, MINOR_COUNT);
        pr_err("my_driver: failed to add device\n"); 
        return add_cdev; 
    }

    pr_info("my_driver: char driver registered\n"); 

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    my_class = class_create(DEVICE_NAME); 
    
#else 
    my_class = class_create(THIS_MODULE, DEVICE_NAME); 
#endif   

    if(IS_ERR(my_class))
    {
        pr_err("my_driver: failed to creae device class\n"); 
        cdev_del(&my_cdev); 
        unregister_chrdev_region(dev_num, MINOR_COUNT);
        return PTR_ERR(my_class); 
    }

    my_device =  device_create(my_class, NULL, dev_num, NULL,  DEVICE_NAME); 
    if(IS_ERR(my_device))
    {
        pr_err("my_driver : Failed to creat device\n"); 
        class_destroy(my_class);
        cdev_del(&my_cdev); 
        unregister_chrdev_region(dev_num, MINOR_COUNT); 
        return PTR_ERR(my_device); 
    }
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    return 0; 
}

static void __exit my_driver_exit(void)
{
    device_destroy(my_class, dev_num); 
    class_destroy(my_class); 
    cdev_del(&my_cdev); 
    unregister_chrdev_region(dev_num, MINOR_COUNT); 

    pr_info("my_driver: Module unloaded successfully\n"); 
}

module_init(my_driver_init); 
module_exit(my_driver_exit); 

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Chrinovic M");
MODULE_DESCRIPTION("Simple char device driver");

