/*
 * Utkarsh Chanchlani
 * 11CS10049
 * Lab test: OS
 * Producer-Consumer problem
 */

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<time.h>

#define BMAX 5 // maximum capacity
#define P_SLEEP_MAX 3 // maximum sleep time of producer, make it bigger to slow down the production rate
#define C_SLEEP_MAX 3 // maximum sleep time of consumer, make it bogger to slow down the consumption rate
#define NO_WAIT 0 // if the producer and consumer are working perfectly fine, then shared state is NO_WAIT
#define PROD_WAIT 1 // if producer waits for consumer to consume, shared state is PROD_WAIT
#define CONS_WAIT 2 // if consumer waits for producer to produce, shared state is CONS_WAIT
#define NEXT (n+1)%BMAX // the next element index
#define PREV (BMAX+n-1)%BMAX // the previous element index
#define WAIT(s) semop(s, &pop, 1) //wait function for semaphore s
#define SIGNAL(s) semop(s, &vop, 1) //signal function for semaphore s

struct sembuf pop, vop; //pop to subtract 1 from the semaphore and vop to add 1
int check = 1; // To perform a clean exit, by deleting all the shared/semaphore memories

/* this function makes check = 0 which will take the process out of the infinite while loop */
void unchecker(int sig) {
    if(sig==SIGINT)
	check = 0;
}

/* the main function */
int main() {
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1; vop.sem_op = 1;

    int shmarrid, *array, statearrid, *statearr, i, n;
    int sem[2]; //semaphore array, 1 semaphore for each process
    int mutex; //mutual exclusion semaphore
	
    /* shared memory initialization */
    shmarrid = shmget(IPC_PRIVATE, BMAX*sizeof(int), 0777|IPC_CREAT); // initiallize the shared memory which stores the produced/consumable items
    array = (int*)shmat(shmarrid, 0,0); // pointer to above shared memory
    for(i = 0; i < BMAX; i++) // initialize it as empty buffer
	array[i] = -1;
	
    statearrid = shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT); // shared memory to store the wait state of both the processes
    statearr = (int*)shmat(statearrid, 0,0); // pointer to above shared memory
    *statearr = NO_WAIT; // initialize as no wait, because intially both are not waiting

    /* semaphore memory */
    mutex = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT); // mutual exclusion semaphore
    semctl(mutex, 0, SETVAL, 1); // set the mutex to 1

    for(i=0;i<2;i++) {
	sem[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT); // semaphore of both processes
	semctl(sem[i], 0, SETVAL, 0); // initialized to 0
    }

    if(fork()==0) { // producer
	srand(time(NULL) + 100);
	signal(SIGINT, unchecker);           /* Register SIGINT signal handler for a clean exit */

	array = (int*)shmat(shmarrid, 0,0);
	statearr = (int*)shmat(statearrid, 0,0);
	n = 0; // index of current item
	int production = 0; //value of current item produced
	while(check) {
	    WAIT(mutex);
	    if(array[n]!=-1) { // if array is full, wait
		printf("Producer inserts item: BUFFER FULL!!\nProducer waits\n"); fflush(stdout);
		*statearr = PROD_WAIT;
		SIGNAL(mutex);
		WAIT(sem[0]);
	    }
	    else { // else produce the item and check if the consumer was waiting, if yes, wake it up
		array[n] = production;
		printf("Producer inserts item: %d\n",production); fflush(stdout);
		production++;
		n = NEXT;
		if((*statearr)==CONS_WAIT) {
		    SIGNAL(sem[1]);
		    *statearr = NO_WAIT;
		}
		SIGNAL(mutex);
	    }
	    sleep(1 + rand()%P_SLEEP_MAX); // take a power nap
	}
	shmdt(array);
	shmdt(statearr);
	exit(0);
	
    } else { // consumer
	srand(time(NULL));
	signal(SIGINT, unchecker);           /* Register SIGINT signal handler for a clean exit */

	n = 0; // index of current item
	int consumption; //value of current item to be consumed
	while(check) {
	    WAIT(mutex);
	    if(array[n]==-1) { // if array is empty, wait
		printf("Consumer consumes: BUFFER EMPTY!!\nConsumer waits\n"); fflush(stdout);
		*statearr = CONS_WAIT;
		SIGNAL(mutex);
		WAIT(sem[1]);
	    }
	    else { // else consume the item and check if the producer was waiting, if yes, wake it up
		consumption = array[n];
		printf("Consumer consumes: %d\n",consumption); fflush(stdout);
		array[n] = -1;
		n = NEXT;
		if((*statearr)==PROD_WAIT) {
		    SIGNAL(sem[0]);
		    *statearr = NO_WAIT;
		}
		SIGNAL(mutex);
	    }
	    sleep(1 + rand()%C_SLEEP_MAX); // take a power nap
	}
	shmdt(array);
	shmdt(statearr);
    }

    /* clear all the shared and semaphore memories */
    shmctl(shmarrid, IPC_RMID, 0);
    shmctl(statearrid, IPC_RMID, 0);
    for(i=0;i<2;i++)
	semctl(sem[i], 0, IPC_RMID, 0);
    semctl(mutex, 0, IPC_RMID, 0);

    //	printf("\n\nAll semaphores and shared memories cleared, clean exit performed\n\n");
    return 0;
}

/*
 * Note: when ctrl+c is pressed, the processes catches the signal and perform a clean exit by clearing all the shared and semaphore memories
 */
