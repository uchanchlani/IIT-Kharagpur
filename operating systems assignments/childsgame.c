/*
 * Assignment 2, Problem 1
 * Play a game with your children
 *
 * Submitted by: 
 * Utkarsh Chanchlani 11CS10049
 * Maru Sudarshan Avish 11CS10028
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_NUMBER 6
#define BUFSIZE 80

/* The signal handler for the left child process */
void lchildSigHandler (int sig)
{
	if (sig == SIGUSR1)
		printf("+++ Left Child: Received signal SIGUSR1 from parent... I've won the game \\m/...\n");
	else if (sig == SIGUSR2)
		printf("+++ Left Child: Received signal SIGUSR2 from parent... I've lost the game :'(...\n");

	exit(0);
}

/* The signal handler for the right child process */
void rchildSigHandler (int sig)
{
	if (sig == SIGUSR1)
		printf("+++ Right Child: Received signal SIGUSR1 from parent... I've won the game \\m/...\n");
	else if (sig == SIGUSR2)
		printf("+++ Right Child: Received signal SIGUSR2 from parent... I've lost the game :'(...\n");

	exit(0);
}


int main()
{
	/* lfd and rfd are the file descriptors for the pipes b/w left and right child resp., lscore and rscore is the score of both the childs managed by the parent, n will provide random number when required*/
	int lfd[2], rfd[2], lscore=0, rscore=0, n;
	char buffer[BUFSIZE];

	/* lpid and rpid are the ids of the left and right child. mypid and parpid are understood!! */
	pid_t lpid, rpid, mypid, parpid;
    
	mypid = getpid();
	parpid = getppid();
	printf("Parent: PID = %u, PPID = %u\n",mypid, parpid);
	pipe(lfd);
	pipe(rfd);

    
	if(fork() == 0) { // left child process
		srand((unsigned int)time(NULL));

		close(lfd[0]); //this process will only write to the left pipe

		/* left child reveals it's id to it's parent */
		mypid = getpid();
		parpid = getppid();
		sprintf(buffer,"%u",mypid);
		write(lfd[1], buffer, BUFSIZE);

		signal(SIGUSR1, lchildSigHandler);           /* Register SIGUSR1 handler */
		signal(SIGUSR2, lchildSigHandler);           /* Register SIGUSR2 handler */

		/* left child generates random numbers till the kill signal is not recieved */
		while (1) {
			n = (int)rand()%MAX_NUMBER + 1;
			sprintf(buffer,"%d",n);
			write(lfd[1], buffer, BUFSIZE);
			sleep(1);
		}

		exit(0);
	}


	else if(fork() == 0) { // right child process
		/* seeding the random number generator with a different number as in the left child process */
		srand(((unsigned int)time(NULL)) + (unsigned int)(1+rand()%100));

		close(rfd[0]); //this process will only write to the right pipe

		/* right child reveals it's id to it's parent */
		mypid = getpid();
		parpid = getppid();
		sprintf(buffer,"%u",mypid);
		write(rfd[1], buffer, BUFSIZE);

		signal(SIGUSR1, rchildSigHandler);           /* Register SIGUSR1 handler */
		signal(SIGUSR2, rchildSigHandler);           /* Register SIGUSR2 handler */

		/* right child generates random numbers till the kill signal is not recieved */
		while (1) {
			n = (int)rand()%MAX_NUMBER + 1;
			sprintf(buffer,"%d",n);
			write(rfd[1], buffer, BUFSIZE);
			sleep(1);
		}

		exit(0);
	}

	else { //parent process
		/* seeding the random number generator with a different number as in its children processes */
		srand(((unsigned int)time(NULL)) + (unsigned int)(3+rand()%500));

		/* parent never writes from the pipes */
		close(lfd[1]);
		close(rfd[1]);

		/* initially parent reads the ids of both the child processes and stores it in their respective variables */
		read(lfd[0], buffer, BUFSIZE);
		sscanf(buffer,"%u",&lpid);
		read(rfd[0], buffer, BUFSIZE);
		sscanf(buffer,"%u",&rpid);

		/* used to read the random numbers from the children */
		int i, j;

		/* reads the numbers till some winner is not obtained */
		while(1) {
			/* reads the two numbers from its two children */
			read(lfd[0], buffer, BUFSIZE);
			sscanf(buffer,"%d",&i);
			read(rfd[0], buffer, BUFSIZE);
			sscanf(buffer,"%d",&j);
			/* parent declares the numbers obtained from it's children */
			printf("Parent: Left child got %d and Right child got %d\n",i,j);

			/* parent decides if it wants MIN to win or MAX to win */
			n = (int)rand()%2; // if n is 0 then parent wants min, else it wants max

			/* if we recieve the same numbers then cancel the round */
			if(i==j) {
				printf("Parent: Left child and Right child got same numbers, round ignored...\n");
			}

			/* case when n is 1 means parent demands MAX */
			else if(n) {
				printf("Parent: I say MAX\n");
				if(i>j) {
					lscore++;
					printf("Parent: Left child wins the round\n");
				}
				else {
					rscore++;
					printf("Parent: Right child wins the round\n");
				}
			}

			/* case when n is 0 means parent demands MIN */
			else {
				printf("Parent: I say MIN\n");
				if(i<j) {
					lscore++;
					printf("Parent: Left child wins the round\n");
				}
				else {
					rscore++;
					printf("Parent: Right child wins the round\n");
				}
			}

			/* print the score after each round */
			printf("Parent: The score-> Left child %d, Right child %d\n\n",lscore, rscore);

			/* after every round, check if anyone has won the game or not */
			if(lscore==10 || rscore==10) {
				/* Send signal to left child telling it if it has won/lost the game and wait for it to exit */
				kill(lpid, (lscore == 10) ? SIGUSR1 : SIGUSR2);
				wait(NULL);
				/* Send signal to right child telling it if it has won/lost the game and wait for it to exit */
				kill(rpid, (rscore == 10) ? SIGUSR1 : SIGUSR2);
				wait(NULL);
				/* lazy parent wants to take a nap!! */
				sleep(3);
				break;
			}
		}
	}
	exit(0);
}
