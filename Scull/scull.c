#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include "scull.h"

static struct scull_qset * scull_follow(struct scull_dev *dev, int n)
{
    struct scull_qset *qs = dev->data;

    if(!qs)
    {
        qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERKEL); 

        if(qs == NULL)
        {
            printk(KERN_ERR "Qset allocation failed\n"); 
            return NULL; 
        }
        memset(qs, 0, sizeof(scull_qset)); 
    }

    while(--n)
    {
        if(!qs->next)
        {
            qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERKEL); 

            if(qs->next == NULL)
            {
                printk(KERN_ERR "Qset allocation failed\n"); 
                return NULL; 
            }
            memset(qs->next, 0, sizeof(struct scull_qset)); 
        }
        qs = qs->next; 
        continue; 
    }

    return qs; 
}

static int scull_trim(struct scull_dev)
{
    struct scull_qset *next; 
    struct scull_qset *dptr; 

    int qset = dev->curr_qset_size;
    int i; 

    for(dptr = dev->data; dptr; dptr = next)
    {
        if(dptr->data)
        {
            for(i =0; i < qset; ++i)
            {
                kfree(dptr->quantum[i]);
            }

            kfree(dptr->quantum);
            dptr->quantum = NULL; 
        }

        next = dptr->next; 
        kfree(dptr); 
    }

    dev->size = 0; 
    dev->curr_quantum_size = scull_quantum;
    dev->curr_qset_size = scull_qset_size;
    dev->data = NULL;

    return 0; 
}

static ssize_t read(struct file *file_p, char __user *buff, size_t count, loff_t *offp)
{


}

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
    int err, devno; 

    devno = MKDEV(scull_major, scull_minor + index);

    cdev_init(&dev->cdev, &scull_fops); 
    dev->cdev.owner = THIS_MODULE; 
    dev->cdev.ops = scull_fops; 
    
    err = cdev_add(&dev->cdev, devno, 1); 
    if(err)
    {
        printk(KERN_NOTICE "Error %d adding scull%d\n", err, index); 
    }
}

static int scull_open(struct inode *inode, struct file * file_p)
{
    struct scull_dev *dev; 

    dev = container_of(inode->i_cdev, struct scull_dev, cdev); 
    file_p->private_data = dev; 

    if((file_p->f_flags & O_ACCMODE) == O_WRONLY)
    {
         scull_trim(dev); 
    }     
}
static int scull_release(struct *inode, struct file *file_p)
{
    return 0;
}

 e
