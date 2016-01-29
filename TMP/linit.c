#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

struct lentry locks[NLOCKS];
int nextlock;
int lockholdtab[NPROC][NLOCKS];

void linit()
{

	int i = 0;
	int j = 0;
	nextlock = 0;

	for(i = 0; i < NLOCKS; i++)
	{
		locks[i].lstate = LFREE;
		locks[i].locknum = i;
		locks[i].lqtail = 1 + (locks[i].lqhead = newqueue());
		locks[i].nreaders = 0;
		locks[i].nwriters = 0;
		locks[i].version = 0;
	}

	for(i = 0; i < NPROC; i ++)
		for(j = 0; j < NLOCKS; j ++)
			lockholdtab[i][j] = 0;
}

