#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>
#include <sleep.h>

int lock(int lock_value, int type, int priority)
{
    STATWORD ps;
    struct  lentry  *lptr;
	int item;
	int lock_index;
	int lock_version;

    disable(ps);

	lock_index = lock_value/MAXVERSION;
	
	lock_version = lock_value%MAXVERSION;

	lptr = &locks[lock_index];
	
	if(locks[lock_index].lstate==LFREE)
		{
        restore(ps);
        return(SYSERR);
    	}
	
	if(lock_version != locks[lock_index].version)
		{
        restore(ps);
        return(SYSERR);
    	}

	if((locks[lock_index].nreaders == 0) && (locks[lock_index].nwriters == 0)) 
		{
		lockholdtab[currpid][lock_index] ++;	
		if(type == READ) 
			locks[lock_index].nreaders ++;
		else
			locks[lock_index].nwriters ++;
        restore(ps);
       	return(OK);
		}

	if((locks[lock_index].nreaders == 0) && (locks[lock_index].nwriters == 1)) 
		{
		proctab[currpid].pstate = PRLWAIT;
		proctab[currpid].plwaitret = OK;
		insert(currpid, lptr -> lqhead, priority);
		q[currpid].qtype = type;
		q[currpid].qtime = clktime;
		resched();
		restore(ps);
		return proctab[currpid].plwaitret;
		}
	
	if((locks[lock_index].nreaders > 0) && (locks[lock_index].nwriters == 0))
		{

		if(type == WRITE) 
			{
			proctab[currpid].pstate = PRLWAIT;
			proctab[currpid].plwaitret = OK;
			insert(currpid, lptr -> lqhead, priority);
			q[currpid].qtype = type;
			q[currpid].qtime = clktime;
			resched();
			restore(ps);
			return proctab[currpid].plwaitret;
			}
		
		if(type == READ)
			{
			item = q[lptr -> lqtail].qprev;
			while((item != lptr -> lqhead) && (priority < q[item].qkey))
				{
				if(q[item].qtype == WRITE)
					{
					proctab[currpid].pstate = PRLWAIT;
					proctab[currpid].plwaitret = OK;
					insert(currpid, lptr -> lqhead, priority);
					q[currpid].qtype = type;
					q[currpid].qtime = clktime;
					resched();
					restore(ps);
					return proctab[currpid].plwaitret;
					}
				item = q[item].qprev;
				}

			lockholdtab[currpid][lock_index] ++;	
			if(type == READ) 
				locks[lock_index].nreaders ++;
			else
				locks[lock_index].nwriters ++;
        	restore(ps);
       		return(OK);
	
			}
		}

}
