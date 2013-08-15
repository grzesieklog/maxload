/*
 *
 * Can't send manulany SIGSTOP and SIGCONT to controled process
 *
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


/* nice leve to I/O prio
 * 0-4 		-> 4
 * 5-9 		-> 5
 * 10-14 	-> 6
 * 15-19 	-> 7
 */
#define LEVEL_0 5
#define LEVEL_1 10
#define LEVEL_2 15
#define LEVEL_3 19

char stop=0, ch=0;
pid_t comm=-1;
unsigned char sec=5;
pid_t pid;

void _sigchild(int sig)
{
	int status=0;
	if (!stop && !ch)
	{
		waitpid(comm, &status, WNOHANG);
		if (WIFEXITED(status))
		{
			exit (0);
		}
	}
	if (ch) ch=0;
}

int main (int argc, char *argv[])
{
	pid = getpid();
	char ret = 0, isroot=0;
	double loadborder;
	
	signal (SIGCHLD,_sigchild);
	
	if (argc >= 3 )
	{
		loadborder = atof(argv[1]);
		if ( !loadborder ) exit(EXIT_FAILURE);
		comm = fork();
		if (comm == 0)
		{
			ret = execvp (argv[2], &argv[2]);
			if (ret == -1)
				exit(EXIT_FAILURE);
			return 0;
		}
	}
	else {
		printf("\nUsage maxload [load] [command] [arg ...]\ne.g.\n\
           # maxload 1.9 backup.sh\n\n");
		exit(0);
	}
	
	isroot = getuid() == 0? 1: 0;
	int n=0,load,status=0,loadlow=0,loadheight=0;
	double avg[1], avg_sav;
	while ( 1 )
	{
		fflush (stdout);
		sleep( sec );
		
		avg_sav = avg[0];
		load = getloadavg (avg, 1);
		//printf ("\nload average: %.2f sec: %i", avg[0],sec);

		if (!stop)
		{
			if ( avg[0] >= loadborder*0.4 && avg[0] <= loadborder )
			{
				if ( avg_sav < avg[0] ) { loadheight=1; loadlow=0; }
				if ( avg_sav > avg[0] ) { loadheight=0; loadlow=1; }
				if ( loadheight ) sec = 5;
				else sec = 5;

				if ( n+1 <= 3 && loadheight )
				{
					++n;
					setpriority (PRIO_PROCESS, comm,
								 n==0?LEVEL_0:
								 n==1?LEVEL_1:
								 n==2?LEVEL_2:
								 n==3?LEVEL_3:LEVEL_3); //printf("\nnice +1 n: %i ",n);
					continue;
				}
				if ( n-1 >= 0 && loadlow && avg[0] < 0.75*loadborder && isroot )
				{
					--n;
					setpriority (PRIO_PROCESS, comm,
								 n==0?LEVEL_0:
								 n==1?LEVEL_1:
								 n==2?LEVEL_2:
								 n==3?LEVEL_3:LEVEL_3);//printf("\nnice -1 n: %i ",n);
					//else printf("\nERROR (You are not root!) to nice -1 n: %i ",n);
					continue;
				}
				continue;
			}
			if ( avg[0] > loadborder )
			{
				stop=1;
				ch=1; // synch
				kill(comm,SIGSTOP); //printf("\nSTOP");
				continue;
			}
			if ( avg[0] <= 0.25*loadborder )
				sec = 10;
			if ( avg[0] <= 0.10*loadborder )
				sec = 15;
			else
				sec = 5;
			continue;
		}
		if (stop && avg[0] < loadborder)
		{
			stop =0;
			ch=1; // synch
			kill(comm,SIGCONT);//printf("\nCONT");
			continue;
		}
	}
}
