#include <linux/kernel.h>
#include <linux/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define __NR_identity 450

long identity_syscall(void) {
    
    return syscall(__NR_identity); 
    
}

int main (int argc, char * argv[]) {
    
    long sys ;
    
    sys=identity_syscall();
    
    if (sys <0 ) {
        
        perror("sorry Junichi. Your syscall is invalid"); 
        
    }else{
        
        
        printf("error");
        
        
    }
    
    
    return 0; 
    
    
    
}


