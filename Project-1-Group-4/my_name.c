#include <linux/kernel.h>
#include <linux/module.h>
int my_name_init(void)
{
    
    printk("[Group4][Junichi Koizumi, Marc Sagario] Hello, I am Junichi Koizumi a student of CSE330 Summer 2023\n");
    return 0; 
    
}

void my_name_exit(void)
{
    printk("Good Bye Junichi Koizumi");
    
    
}

module_init(my_name_init);
module_exit(my_name_exit);
MODULE_LICENSE("GPL"); 
