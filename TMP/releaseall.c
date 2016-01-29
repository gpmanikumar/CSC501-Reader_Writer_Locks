#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>
//#include <stdlib.h>


int get_best(int lock_index)
{
	struct lentry *lptr;
    int best;
	int item;
	int wait_diff;
	lptr = &locks[lock_index];
    item = q[lptr -> lqtail].qprev;
    best = q[lptr -> lqtail].qprev;

        if(best == lptr -> lqhead) 
			return -1; //null

		item = q[best].qprev; 

        while(q[item].qprev != lptr -> lqhead)
        	{
        	
            item = q[item].qprev;
			
            if(q[item].qkey < q[best].qkey) 
				{
				return best;
            	}

            if(q[item].qkey == q[best].qkey)
                {
                wait_diff = abs(q[best].qtime - q[item].qtime);

                if(wait_diff < 1)
                	{
                	
					if((q[best].qtype == READ) && (q[item].qtype == WRITE))
						{
						best = item;
						}
		
                    else
                    	{
                    	best = best;
                    	}
                    
                    }

				if(q[best].qtime > q[item].qtime) 
					{
					best = item;
					}

				if(q[best].qtime < q[item].qtime) 
					{
					best = best;
					}
                }
        	}
        return best;
} 


int releaseall(int numlocks, int ldes1, ...)
{
	int lock_value;
	int flag = 0;
	unsigned long *a = (unsigned long *)(&ldes1);
    for ( ; numlocks > 0 ; numlocks--)
		{
        lock_value = *a++; 
		if(release(currpid, lock_value) == SYSERR) 
			flag = 1;
		}
		
	resched();
	
	if(flag == 1) 
		return(SYSERR);
	else 
		return(OK);
}

int release(int pid, int lock_value)
{
    STATWORD ps;
    struct lentry  *lptr;
	int best;
	int lock_index;
	int lock_version;

	disable(ps);
	lock_index = lock_value/MAXVERSION;
	lock_version = lock_value%MAXVERSION;
	lptr= &locks[lock_index];
	
	if (locks[lock_index].lstate==LFREE) 
        {
        restore(ps);
        return(SYSERR);
        }
	
	if (locks[lock_index].version != lock_version)
		{
        restore(ps);
        return(SYSERR);
        }
	
	if(lockholdtab[pid][lock_index] > 0) 
		{
		lockholdtab[pid][lock_index]--;
		}
	else
		{
		restore(ps);
		return(SYSERR);
		}

	if((locks[lock_index].nreaders > 0) && (locks[lock_index].nwriters == 0)) 
		{
		locks[lock_index].nreaders --;
		}
	
	else if((locks[lock_index].nreaders == 0) && (locks[lock_index].nwriters == 1)) 
		{
		locks[lock_index].nwriters --;
		}
	
	else 
		{
		kprintf("Impossible to get here. Something is wrong!!!\n");
		}
		
	if((locks[lock_index].nreaders == 0) && (locks[lock_index].nwriters == 0))
		{
		best = get_best(lock_index);
		while(best != -1)
			{
			if(q[best].qtype == READ)
				{
				locks[lock_index].nreaders ++;
				lockholdtab[best][lock_index] ++;
				dequeue(best);
				ready(best, RESCHNO);
				best = get_best(lock_index);
				
				if((best != -1) && (q[best].qtype == WRITE))
					{
					best = -1;
					break;
					}
				}
			else if(q[best].qtype == WRITE)
				{
				locks[lock_index].nwriters ++;
				lockholdtab[best][lock_index] ++;
				dequeue(best);
				ready(best, RESCHNO);
				break;
				}
			}
		}
	restore(ps);
	return OK;
}

