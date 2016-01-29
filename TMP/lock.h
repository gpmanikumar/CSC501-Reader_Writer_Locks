#define NLOCKS	50

#define MAXVERSION 100

#define READ	1
#define WRITE	2

#define	LFREE	1 
#define	LUSED	2

struct lentry {
	char	lstate;
	int		locknum;
	int		lqhead;
	int 	lqtail;
	int		nreaders;
	int 	nwriters;
	int		version;
};

extern struct lentry locks[];

extern int nextlock;

extern int lockholdtab[][NLOCKS];

void linit();
int lcreate();
int ldelete(int lockdescriptor);
int lock(int ldes1, int type, int priority);
int releaseall(int numlocks, int ldes1, ...);
int get_best(int lock);
int release(int pid, int ldes);

