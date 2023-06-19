//Group 4
#define BUFF_MAX 1000
#define CONS_MAX 100

#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/sched/signal.h>


MODULE_LICENSE("GPL");

static int totalTime = 0;

//setting up input param
static int buff_size = 10;
static int p = 1;
static int c = 0;
static int uid = 1000;

module_param(buff_size, int, 0);
module_param(p, int, 0);
module_param(c, int, 0);
module_param(uid, int, 0);

//defining semaphores
static struct semaphore mutex;
static struct semaphore full;
static struct semaphore empty;

//Producer threads and counter
static struct task_struct* producerThread;
static int prodCounter = 0;
static int prodItems = 0;

//Consumer threads and counter
static struct task_struct* consumerThreads[CONS_MAX];
static int consCounter = 0;
static int consItems = 0;

//buffer
static struct task_struct* readBuffer[BUFF_MAX];
static int prodBufferIndex = 0;
static int consBufferIndex = 0;

//producer_thread function
static int producer_thread(void* data){
	struct task_struct* task;

    printk("[kProducer-%d] kthread Producer Created Successfully\n", ++prodCounter);
    //while (!kthread_should_stop()) {
        for_each_process(task){
            if(task->cred->uid.val == uid){

                if(down_interruptible(&empty)){
                    return -ERESTARTSYS;
                }

                if(down_interruptible(&mutex)){
                    return -ERESTARTSYS;
                }

                readBuffer[prodBufferIndex] = task;
                printk("[kProducer-%d] Produced-Item #-%d at buffer index:%d for PID%d\n", p, ++prodItems, prodBufferIndex, task->pid);
                prodBufferIndex = (prodBufferIndex + 1) % buff_size;
                up(&mutex);
                up(&full);
            }
        }
    //}
    return 0;
}

//consumer thread function
static int consumer_thread(void* data){
    int threadNum = ++consCounter;
    int t, hours, min, sec;
    printk("[kConsumer-%d] kthread Consumer Created Successfully\n", threadNum);

    while(!kthread_should_stop()){

        if(down_interruptible(&full)){
            return -ERESTARTSYS;
        }
        if(kthread_should_stop()){
            break;
        }

        if(down_interruptible(&mutex)){
            return -ERESTARTSYS;
        }
        if(kthread_should_stop()){
            break;
        }

        t = ((ktime_get_ns() - readBuffer[consBufferIndex]->start_time)/10000+5)/10;
        totalTime += t;
        t = (t/100+5)/10;
        hours = t/3600;
        min = (t/60) %60;
        sec = t%60;

        printk("[kConsumer-%d] Consumed-Item-%d on buffer index:%d::PID%d\tElapsed Time %02d:%02d:%02d\n", threadNum, ++consItems, consBufferIndex, readBuffer[consBufferIndex]->pid, hours, min, sec);
        consBufferIndex = (consBufferIndex + 1) % buff_size;
        up(&mutex);
        up(&empty);
    }
    return 0;
}

//initializing module
int init(void){
    int l;
    printk("CSE330 Project-2 Kernel Module Inserted\n");
    printk("Kernel module received the following inputs: UID: %d, Buffer-Size: %d, No of Producer: %d, No of Consumers: %d\n", uid, buff_size, p, c);
    
    sema_init(&empty, buff_size);
    sema_init(&full, 0);
    sema_init(&mutex, 1);

    if(p >= 1){
        producerThread = kthread_run(producer_thread, NULL, "producer");
    }
    
    for(l = 0; l < c; l++){
        consumerThreads[l] = kthread_run(consumer_thread, NULL, "consumer");
    }
    return 0;
}

//exit module
void term(void){
    int i, ti, hou, mi, se;

    if(p >= 1){
        kthread_stop(producerThread);
        printk("[kProducer-%d] Producer Thread Stopped.\n", p);
    }

    for(i = 0; i < c; i++){
        up(&empty);
        up(&full);
        up(&mutex);
        kthread_stop(consumerThreads[i]);
        printk("[kConsumer-%d] Consumer Thread Stopped.\n", i+1);
    }

    printk("Total number of items produced: %d\n", prodItems);
    printk("Total number of items consumed: %d\n", consItems);

     ti = (totalTime/100+5)/10;
     hou = ti/3600;
     mi = (ti/60)%60;
     se = ti%60;

    printk("The total elapsed time of all processes for UID %d is %02d:%02d:%02d\n", uid, hou, mi, se);
    printk("CSE330 Project-2 Kernel Module Removed\n");
}

module_init(init);
module_exit(term);
