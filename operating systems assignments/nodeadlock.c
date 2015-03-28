#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<time.h>

#define PH_NUMB 5 //number of philosophers
#define THINKING 0 //state when the philosopher doesn't aquire any forks
#define HUNGRY 1 //state when philosopher is hungry and tries to aquire forks
#define EATING 2 //state when it got both forks, and now will eat happily
#define LEFT (n + PH_NUMB - 1)%PH_NUMB //left philosopher index
#define RIGHT (n + 1)%PH_NUMB //right philosopher index
#define W(s) semop(s, &pop, 1) //wait function
#define S(s) semop(s, &vop, 1) //signal function

struct sembuf pop, vop; //pop to subtract 1 from the semaphore and vop to add 1
int sem[PH_NUMB]; //semaphore array, 1 for each philosopher
int mutex; //mutual exclusion semaphore

/*
 * the function which lets a philosopher think for random amount of time
 */
void think(int n) {
	int time = 1 + rand()%5;
	printf("Philosopher %d: I'll think for %d secs\n",n,time);
	sleep(time);
}

/*
 * the function which lets a philosopher eat for random amount of time
 */
void eat(int n) {
	int time = 1 + rand()%10;
	printf("Philosopher %d: Got forks. I'll eat for %d secs\n",n,time);
	sleep(time);
}

/*
 * checks if the fork it's trying to grab is availible or not
 */
void test(int n, int *state) {
	if(state[n]==HUNGRY && state[LEFT]!=EATING && state[RIGHT]!=EATING) {
		state[n] = EATING;
		S(sem[n]);
	}
}

/*
 * this function changes the state of philosopher to hungry and tries to grab the forks
 * the philosopher waits till the state of the philosopher comes to eating
 */
void take_forks(int n, int *state) {
	W(mutex);
	state[n] = HUNGRY;
	printf("Philosopher %d: I'm hungry\n",n);
	test(n, state);
	S(mutex);
	W(sem[n]);
}

/*
 * philosopher done eating, state changes to thinking and it keeps both it's forks
 */
void put_forks(int n, int *state) {
	W(mutex);
	state[n] = THINKING;
	printf("Philosopher %d: I'm done eating. putting %d (left) and %d (right) forks\n",n,n,RIGHT);
	test(LEFT, state);
	test(RIGHT, state);
	S(mutex);
}

/*
 * the main function
 */
int main(int argc, char* argv) {
	pop.sem_num = vop.sem_num = 0;
	pop.sem_flg = vop.sem_flg = 0;
	pop.sem_op = -1; vop.sem_op = 1;

	/*
	 * state array is used to store the states of all the Philosophers
	 * state_arrid is is used to appocate the shared memory for the state array
	 */
	int *state, state_arrid, i;

	// shared array declared
	state_arrid = shmget(IPC_PRIVATE, PH_NUMB*sizeof(int), 0777|IPC_CREAT);
	state = (int*)shmat(state_arrid,0,0);

	/* initiallize state variable and the semaphore memory for all the philosophers */
	for(i=0;i<PH_NUMB;i++) {
		state[i] = THINKING;
		sem[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
		semctl(sem[i], 0, SETVAL, 0);
	}

	/* mutual exclusion semaphore */
	mutex = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semctl(mutex, 0, SETVAL, 1);

	/* create PH_NUMB philosopher childs */
	for(i = 0; i < PH_NUMB; i++) {
		if(fork()==0) {
			srand(time(NULL) + 100*(i+1));
			state = (int*)shmat(state_arrid,0,0);

			// the philosopher algorithm implementation
			while(1) {
				think(i);
				take_forks(i, state);
				eat(i);
				put_forks(i, state);
			}
			shmdt(state);
			exit(0);
		}
	}

	/* parent loops for infinite time */
	while(1);

	/* clear all shared and semaphore memories */
	shmdt(state);
	shmctl(state_arrid, IPC_RMID, 0);
	for(i=0;i<PH_NUMB;i++)
		semctl(sem[i], 0, IPC_RMID, 0);
	semctl(mutex, 0, IPC_RMID, 0);

	exit(0);
}
