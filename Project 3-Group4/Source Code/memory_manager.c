#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

static int pid = 0;
module_param(pid, int, 0);

static int vmaCounter = 0;

pgd_t* pgd;
p4d_t* p4d;
pud_t* pud;
pmd_t* pmd;
pte_t* ptep, pte;

static struct hrtimer restartTimer;

ktime_t currtime , interval, noInterval;

unsigned long timerInterval = 10e9;
unsigned long wCounter = 0;
unsigned long rCounter = 0;
unsigned long sCounter = 0;
unsigned long i = 0;

struct task_struct* task;
struct mm_struct* mm;
struct vm_area_struct * vma;

int testP (struct vm_area_struct *vma, unsigned long addr, pte_t *ptep) {
    int r = 0;
    if (pte_young(*ptep)) {
        r= test_and_clear_bit(_PAGE_BIT_ACCESSED, (unsigned long *) &ptep->pte);
    }
    return r;
}



enum hrtimer_restart restart(struct hrtimer* in) {
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    mm = task->mm;
    if(mm != NULL){
        vma = mm->mmap;
        while(vma != NULL) {
            for (i = vma->vm_start; i < vma->vm_end; i += PAGE_SIZE) {
                pgd = pgd_offset(mm, i);
                p4d = p4d_offset(pgd, i);
                if (p4d_none(*p4d) || p4d_bad(*p4d)) {
                    continue;
                }

                pud = pud_offset(p4d, i);
                if (pud_none(*pud) || pud_bad(*pud)) {
                    continue;
                }

                pmd = pmd_offset(pud, i);
                if (pmd_none(*pmd) || pmd_bad(*pmd)) {
                    continue;
                }

                ptep = pte_offset_map(pmd, i);
                if (!ptep) {
                    continue;
                }
                pte = *ptep;
                if (pte_present(pte)) {
                    rCounter += 4;
                    if (testP(vma, i, ptep) == 1) {
                        wCounter += 4;
                    }
                } else {
                    sCounter += 4;
                }
            }
            vmaCounter++;
            vma = vma->vm_next;
        }
    }
    currtime = ktime_get();
    interval = ktime_set(0, timerInterval);
    hrtimer_forward(in, currtime, interval);
    printk("PID %d : RSS = %lu KB, SWAP = %lu KB, WSS = %lu KB", pid, rCounter, sCounter, wCounter);
    rCounter = 0;
    sCounter = 0;
    wCounter = 0;
    return HRTIMER_RESTART;
}

enum hrtimer_restart noRestart(struct hrtimer* in) {
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    mm = task->mm;
    if(mm != NULL){
        vma = mm->mmap;
        while(vma != NULL) {
            for (i = vma->vm_start; i < vma->vm_end; i += PAGE_SIZE) {
                pgd = pgd_offset(mm, i);
                p4d = p4d_offset(pgd, i);
                if (p4d_none(*p4d) || p4d_bad(*p4d)) {
                    continue;
                }
                pud = pud_offset(p4d, i);
                if (pud_none(*pud) || pud_bad(*pud)) {
                    continue;
                }

                pmd = pmd_offset(pud, i);
                if (pmd_none(*pmd) || pmd_bad(*pmd)) {
                    continue;
                }

                ptep = pte_offset_map(pmd, i);
                if (!ptep) {
                    continue;
                }
                pte = *ptep;
                if (pte_present(pte)) {
                    rCounter += 4;
                    if (testP(vma, i, ptep) == 1) {
                        wCounter += 4;
                    }
                }
                else {
                    sCounter += 4;
                }

            }
            vmaCounter++;
            vma = vma->vm_next;
        }

    }
    printk("PID %d : RSS = %lu KB, SWAP = %lu KB, WSS = %lu KB", pid, rCounter ,sCounter, wCounter);
    rCounter = 0;
    sCounter = 0;
    wCounter = 0;
    return HRTIMER_NORESTART;
}

static void startTimer(void){
    interval = ktime_set(0, timerInterval);
    hrtimer_init(&restartTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    restartTimer.function = &restart;
    hrtimer_start(&restartTimer, interval, HRTIMER_MODE_REL);
}

static int __init initial(void) {
    printk("inserting the module for project 3");
    startTimer();
    return(0);

}

static void __exit term(void) {
    printk("removing the module for project 3\n");
    hrtimer_cancel(&restartTimer);
}

module_init(initial);
module_exit(term);
