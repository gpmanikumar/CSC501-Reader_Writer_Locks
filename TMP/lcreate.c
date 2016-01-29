#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int lcreate(void)
{
		STATWORD ps;
        int lock;
		int i = 0;
		int j = 0;
		int lock_value;

        disable(ps);
		
		for (i=0 ; i<NLOCKS ; i++) 
		{
        	lock = nextlock++;
            if (nextlock > NLOCKS-1)
            	nextlock = 0;
            if (locks[lock].lstate==LFREE) 
			{
            	locks[lock].lstate = LUSED;
				locks[lock].nreaders = 0;
				locks[lock].nwriters = 0;
				locks[lock].version++;
				if(locks[lock].version >= MAXVERSION)
					locks[lock].version = 0;
				lock_value = (locks[lock].locknum*MAXVERSION)+locks[lock].version;
				for(j = 0; j < NPROC; j++) 
					lockholdtab[j][lock] = 0;
				restore(ps);
                return(lock_value);
            }
        }
		
        restore(ps);
        return(SYSERR);
}

