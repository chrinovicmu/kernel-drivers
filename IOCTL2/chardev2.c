#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h> /* Specifically, a module */
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/errno.h> 

#include "chardev.h"
#include "linux/atomic/atomic-instrumented.h"
#include "linux/device/class.h"
#include "linux/export.h"
#define DEVICE_NAME "char_dev"
#define BUF_LEN     80

static char kbuf[BUF_LEN]; 

enum{
    CDEV_NOT_USED, 
    CDEV_EXCLUSIVE_OPEN, 
}; 

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char message[BUF_LEN + 1]; 

static struct class *class; 

static int device_open(struct inode *inode, struct file *file)
{
    pr_info("device_open(%p)\n", file); 
    try_module_get(THIS_MODULE);
    
    return 0; 
}

static int device_release(struct inode *inode, struct file *file)
{
    pr_info("device_release (%p,%p)\n", inode, file); 
    module_put(THIS_MODULE); 
    return 0; 
}


static ssize_t device_read(struct file *file_p, char __user * buffer, size_t length, loff_t *offset)
{
    int bytes_read = 0; 

    const char *message_ptr = message;

    if(*(message +  *offset) == "\0")
    {
        *offset = 0; 
        return 0; 
    }

    message_ptr += *offset; 

    while(length && *message_ptr)
    {
        if(put_user(*(message_ptr++), buffer++))
        {
            return -EFAULT; 
        }
        --length; 
        ++bytes_read;
    }
    *offset += bytes_read; 

    pr_info("Read %d bytes, %ld left \n", bytes_read, length); 

    return bytes_read; 
}

static ssize_t devce_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset) 
{
    int x; 
    pr_info("devce_write(%p, %p, %ld)", file , buffer, length); 
    
    for(x = 0; x < length && x < BUF_LEN; ++x)
    {
       if(get_user(message[x], buffer + x)) {
            return -EFAULT; 
        }
    }

    message[x] = '\0'; 

    return x; 
}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    long ret = 0;

    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) {
        return -EBUSY;
    }

    switch (ioctl_num) {

        case IOCTL_BUFFER_LEN: {

            size_t buffer_len = BUF_LEN;

            if (copy_to_user((int __user *)ioctl_param, &buffer_len, sizeof(int))) {
                pr_err("Failed to copy buffer len to user space\n");
                ret = -EFAULT;
            }
            break;
        }

        case IOCTL_SET_MSG: {

            char __user *temp = (char __user *)ioctl_param;
            char count;

            if (copy_from_user(&count, temp, 1)) {
                pr_err("Failed to copy length byte from user\n");
                ret = -EFAULT;
                break;
            }

            if (count > BUF_LEN) {
                count = BUF_LEN;
            }

            if (copy_from_user(kbuf, temp + 1, count)) {
                pr_err("Failed to copy data from user\n");
                ret = -EFAULT;
                break;
            }

            break;
        }

        case IOCTL_GET_MSG: {

            char __user *user_ptr = (char __user *)ioctl_param;
            char count = BUF_LEN;

            if (copy_to_user(user_ptr, &count, 1)) {
                ret = -EFAULT;
                break;
            }

            if (copy_to_user(user_ptr + 1, kbuf, count)) {
                ret = -EFAULT;
                break;
            }

            break;
        }

        case IOCTL_GET_NTH_BYTE: {

            ret = (long)message[ioctl_param];
            break;
        }

        default:
            ret = -EINVAL;
            break;
    }

    atomic_set(&already_open, CDEV_NOT_USED);
    return ret;
}

static struct file_operations fops = {
    .read = device_read, 
    .write = devce_write, 
    .unlocked_ioctl = device_ioctl, 
    .open = device_open, 
    .release = device_release, 
}; 

static int __init chardev2_init(void)
{
    int ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);

    if (ret_val < 0) {
        pr_alert("%s failed with %d\n", __func__, ret_val);
        return ret_val;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    class = class_create(DEVICE_FILE_NAME);
#else
    class = class_create(THIS_MODULE, DEVICE_FILE_NAME);
#endif

    if (IS_ERR(class)) 
    {
        unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
        pr_alert("Failed to create class\n");
        return PTR_ERR(class);
    }

    device_create(class, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_FILE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_FILE_NAME);

    return 0;
}
static void __exit chardev2_exit(void)
{
    device_destroy(class, MKDEV(MAJOR_NUM, 0)); 
    class_destroy(class); 

    unregister_chrdev(MAJOR_NUM, DEVICE_NAME); 
}

module_init(chardev2_init); 
module_exit(chardev2_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chrinovic M"); 
