#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int ldelete(int lock_value)
{
        STATWORD ps;
        int     pid, i;
        struct  lentry  *lptr;
		int lock_index;
		int lock_version;
		
        disable(ps);
		lock_index = lock_value/MAXVERSION;
		lock_version = lock_value%MAXVERSION;
		lptr=&locks[lock_index];
		
        if (locks[lock_index].lstate==LFREE) 
			{
            restore(ps);
            return(SYSERR);
        	}
		
		if(locks[lock_index].version != lock_version)
			{
            restore(ps);
            return(SYSERR);
        	}

        locks[lock_index].lstate = LFREE;
        if(nonempty(lptr->lqhead)) 
			{
            while( (pid=getfirst(lptr->lqhead)) != EMPTY)
                {
                proctab[pid].plwaitret = DELETED;
                ready(pid,RESCHNO);
                }
                resched();
        	}
		locks[lock_index].nreaders = 0;
		locks[lock_index].nwriters = 0;

		for(i = 0; i < NPROC; i++)
			lockholdtab[i][lock_index] = 0;

        restore(ps);
        return(OK);
}

