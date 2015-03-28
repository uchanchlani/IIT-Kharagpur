#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<time.h>

/* uncomment the below "#define" line, it will definitely lead to a deadlock initially*/
#define deadlock

/*
 * Uncomment the below "#define" line for the debug mode, it will print the directed resource graph
 * Note: for n in [0,PH_NUMB-1], it's the node for respective philosopher
 * for n in [PH_NUMB, 2*PH_NUMB-1], it's the node for the fork (n-PH_NUMB)
 * an edge printed like "node x -> node -1" implies there is no edge originating from node x
 */
//#define debug

#define PH_NUMB 5 //number of philosophers
#define THINKING 0 //state when the philosopher doesn't aquire any forks
#define GRAB_LEFT 1 //state when it tries to aquire left fork
#define GRAB_RIGHT 2 //state when it tries to aquire right fork
#define EATING 3 //state when it got both forks, and now will eat happily
#define LEFT (n + PH_NUMB - 1)%PH_NUMB //left philosopher index
#define RIGHT (n + 1)%PH_NUMB //right philosopher index
#define W(s) semop(s, &pop, 1) //wait function
#define S(s) semop(s, &vop, 1) //signal function

struct sembuf pop, vop; //pop to subtract 1 from the semaphore and vop to add 1
int sem[PH_NUMB]; //semaphore array, 1 for each philosopher
int mutex; //mutual exclusion semaphore

/*
 * resource graph array is used as explained:
 * rs_grph[i] is the node corresponding to i'th philosopher
 * rs_grph[PH_NUMB + i] corresponds to i'th fork
 * the value of the array element is -1 if there is no edge coming out from that node
 * else it's value is the node index to which it points
 */
int rs_grphid, *rs_grph; //shared memory variables for resource graph

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
	if(state[n]==GRAB_LEFT && state[LEFT]!=EATING) {
		printf("Philosopher %d: %d (left) fork acquired\n",n,n);
		state[n] = GRAB_RIGHT;
		rs_grph[PH_NUMB + n] = n;
		rs_grph[n] = PH_NUMB + RIGHT;
		S(sem[n]);
	}

	else if(state[n]==GRAB_RIGHT && state[LEFT]!=EATING && state[RIGHT]!=EATING && state[RIGHT]!=GRAB_RIGHT) {
		printf("Philosopher %d: %d (right) fork acquired\n",n,RIGHT);
		state[n] = EATING;
		rs_grph[PH_NUMB + RIGHT] = n;
		rs_grph[n] = -1;
		S(sem[n]);
	}
}

/*
 * this function changes the state of philosopher to hungry and tries to grab the forks
 * the philosopher waits till the state of the philosopher comes to eating
 * it loops till the state of the philosopher comes to eating
 */
void take_forks(int n, int *state) {
	printf("Philosopher %d: I'm hungry\n",n);
	state[n] = GRAB_LEFT;
	rs_grph[n] = PH_NUMB + n;
	while(state[n]!=EATING) {
		W(mutex);
		test(n, state);
		S(mutex);
		W(sem[n]);
		sleep(2);
	}
}

/*
 * philosopher done eating, state changes to thinking and it keeps both it's forks
 */
void put_forks(int n, int *state) {
	W(mutex);
	state[n] = THINKING;
	rs_grph[n] = rs_grph[PH_NUMB + n] = rs_grph[PH_NUMB + RIGHT] = -1;
	printf("Philosopher %d: I'm done eating. putting %d (left) and %d (right) forks\n",n,n,RIGHT);
	test(LEFT, state);
	test(RIGHT, state);
	S(mutex);
}

/*
 * function returns the smallest unvisited node else -1
 */
int is_node_left(int *A, int n) {
	int i;
	for(i=0;i<n;i++)
		if(A[i]==0)
			return i;
	return -1;
}

/*
 * this function returns 1 on deadlock condition and 0 otherwise
 * it also returns the deadlocked processes in the dead_process array
 */
int is_deadlock(int *dead_process) {
	int i, n = 2*PH_NUMB, node;
	int *check = (int*)malloc(n*sizeof(int));
	for(i=0;i<n;i++)
		check[i] = 0;
	while((node = is_node_left(check,n)) != -1) {
		for(i=0;i<PH_NUMB;i++)
			dead_process[i] = 0;
		while(1) {
			if(node<PH_NUMB) {
				if(dead_process[node] == 0)
					dead_process[node] = 1;
				else {
					free(check);
					return 1;
				}
			}
			check[node] = 1;
			node = rs_grph[node];
			if(node==-1)
				break;
		}
	}
	return 0;
}

/*
 * this function is used by parent to check deadlock in the children and take necessary steps if required
 */
void check_deadlock(int *state) {
	int i;
	int *dead_process = (int*)malloc(PH_NUMB*sizeof(int));
	W(mutex);
	/*	for(i=0;i<PH_NUMB;i++)
		if(state[i]!=GRAB_RIGHT) {
			check = 0;
			break;
		}
	*/
	//	check = is_deadlock(dead_process);
	//	if(check) { // deadlock

	if(is_deadlock(dead_process)) { // deadlock
		printf("Parent: Detected Deadlock. Going for recovery.\n");
#ifdef debug
		for(i=0;i<2*PH_NUMB;i++)
			printf("node %d -> node %d\n",i,rs_grph[i]);
		for(i=0;i<PH_NUMB;i++)
			printf("Parent: Is process %d dead (1 for yes, 0 for no): %d\n",i,dead_process[i]);
#endif
		int n = rand()%PH_NUMB;
		while(dead_process[n]==0)
			n = rand()%PH_NUMB;
		state[n] = GRAB_LEFT;
		printf("Parent: preemts Philosopher %d to release the fork\n",n);
		printf("Parent: signals the %d left philosopher to pick it's right fork\n",LEFT);
		S(sem[LEFT]);
	}
	else {
		printf("Parent: No deadlock\n");
#ifdef debug
		for(i=0;i<2*PH_NUMB;i++)
			printf("node %d -> node %d\n",i,rs_grph[i]);
#endif
	}

	free(dead_process);
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

	// shared resource graph initialized
	rs_grphid = shmget(IPC_PRIVATE, 2*PH_NUMB*sizeof(int), 0777|IPC_CREAT);
	rs_grph = (int*)shmat(rs_grphid,0,0);

	/* initiallize state variable and the semaphore memory for all the philosophers */
	for(i=0;i<PH_NUMB;i++) {
		state[i] = THINKING;
		rs_grph[i] = rs_grph[PH_NUMB + i] = -1;
		sem[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
		semctl(sem[i], 0, SETVAL, 0);
	}

	/* mutual exclusion semaphore */
	mutex = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semctl(mutex, 0, SETVAL, 1);

	/* create PH_NUMB philosopher childs */
	for(i = 0; i < PH_NUMB; i++) {
		if(fork()==0) {
#ifdef deadlock
			srand(time(NULL)); // seed all childs with same number, for a definate deadlock initially
#else
			srand(time(NULL) + 100*(i+1)); // seed all childs with different numbers
#endif

			state = (int*)shmat(state_arrid,0,0);
			rs_grph = (int*)shmat(rs_grphid,0,0);
	
			// the philosopher algorithm implementation
			while(1) {
				think(i);
				take_forks(i, state);
				eat(i);
				put_forks(i, state);
			}
			shmdt(state);
			exit(0);
			return 0;
		}
	}

	srand(time(NULL)); // seed parent with different number

	/* parent checks for deadlocks in every 10 secs */
	while(1) {
		sleep(10);
		check_deadlock(state);
	}

	/* clear all shared and semaphore memories */
	shmdt(state);
	shmctl(state_arrid, IPC_RMID, 0);
	for(i=0;i<PH_NUMB;i++)
		semctl(sem[i], 0, IPC_RMID, 0);
	semctl(mutex, 0, IPC_RMID, 0);

	exit(0);
}
