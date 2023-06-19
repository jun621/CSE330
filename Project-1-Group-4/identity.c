#include <linux/kernel.h>
#include <linux/syscalls.h>
SYSCALL_DEFINE0(identity)
{
    
    printk("This is the new system call Sagario Koizumi implemented"); 
    return 0; 
}
