#ifndef SCULL_H 
#define SCULL_H

#undef PDEBUG 

#ifdef SCULL_DEBUG 
#   ifdef __KERNEL__ 
        /*kernel space debugging */ 
#       define PDEBUG(fmt, args...) printk(KERN_DEBUG "scull: " fmt, ,## args);
#   else
    /*user space debugging */ 
#       define PDEBUG(fmt, args..)fprintf(stderr, fmt, ## args)
#endif

struct scull_qset
{
    void *quantum; 
    struct scull_qset *next; 
}


struct scull_dev
{
    struct scull_qset *data;    /*pointer to first quantum set */ 
    int cur_quantum_size;             /* current quantum size */ 
    int curr_qset_size;                   /*curernt array size */ 
    unsigned long size;         /*amount of data stored here */  
    unsigned itn access_key;    /* used by scullid and scullprv */ 
    struct semaphore sem;       /* mutual exlusion semaphore */ 
    struct cdev cdev;           /*character device structure *? 
}; 


#endif // !SCULL_H 
