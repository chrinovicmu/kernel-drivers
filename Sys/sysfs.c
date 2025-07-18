#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>

static struct kobject *mymodule; 

static int my_variable = 0; 

static ssize_t my_variable_show(struct kobject *kobj, struct kobj_attribute * attr, char *buf)
{
    return sprintf(buf, "%d\n", my_variable); 
}

static ssize_t my_variable_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%d", &my_variable);
    return count; 
}

static struct kobj_attribute my_variable_attribute = __ATTR(my_variable, 0660, my_variable_show, my_variable_store); 

static int __init my_module_init(void)
{
    int err = 0; 

    pr_info("mymodule: initialized\n");

    mymodule = kobject_create_and_add("mymodule", kernel_kobj);
    if(!mymodule){
        return -ENOMEM; 
    }

    err = sysfs_create_file(mymodule, &my_variable_attribute.attr); 
    if(err)
    {
        kobject_put(mymodule); 
        pr_info("failed to created the variable file in sys/kernel/mymodule\n"); 
    }
    return err; 
}
static void __exit my_module_exit(void)
{
    pr_info("mymodule: exit success\n");
    kobject_put(mymodule); 
}

module_init(my_module_init);
module_exit(my_module_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chrinovic M "); 
