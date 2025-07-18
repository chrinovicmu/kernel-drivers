#include <linux/init.h>
#include <linux/module.h> 
#include <linux/moduleparam.h>

static int my_int = 0; 
module_param(my_int, int , 0644); 
MODULE_PARM_DESC(my_int, "An integer"); 

static char *my_str = "default"; 
module_param(my_str, charp, 0644); 
MODULE_PARM_DESC(my_str, "A character string"); 


static int  __init my_module_init(void)
{
    printk(KERN_INFO "myint : %d\n", my_int); 
    printk(KERN_INFO "mychar : %s\n", my_str); 

    return 0;
}

static void __exit my_module_exit(void)
{
    printk("Exiting module...\n"); 
}

module_init(my_module_init); 
module_exit(my_module_exit); 

MODULE_LICENSE("GPL"); 
