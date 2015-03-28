/*
 * Utkarsh Chanchlani
 * 11CS10049
 * Lab test: OS
 * Producer-Consumer problem
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<time.h>
#include <fcntl.h>

#define FILENAME buffer.txt // shared file
#define N_READ 2 // number of readers
#define T_READ_MAX 10 // read time interval
#define T_WRITE_MAX 10 // writer max time interval
#define MAX 10 // max value written by the writer
#define WAIT(s) semop(s, &pop, 1) //wait function for semaphore s
#define SIGNAL(s) semop(s, &vop, 1) //signal function for semaphore s

struct sembuf pop, vop; //pop to subtract 1 from the semaphore and vop to add 1
int check = 1; // To perform a clean exit, by deleting all the shared/semaphore memories

/* this function makes check = 0 which will take the process out of the infinite while loop */
void unchecker(int sig) {
    if(sig==SIGUSR1)
	check = 0;
}

int main() {
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1; vop.sem_op = 1;

    int shmarrid, *array;
    int mutex, lock;
    char infile[] = "FILENAME";
    int fd;
    int i;
    int pid[N_READ];

    /* shared memory initialization */
    shmarrid = shmget(IPC_PRIVATE, 1*sizeof(int), 0777|IPC_CREAT); // initiallize the shared memory which stores the produced/consumable items
    array = (int*)shmat(shmarrid, 0,0); // pointer to above shared memory
    *array = 0;

    /* semaphore memory */
    mutex = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT); // mutual exclusion semaphore
    semctl(mutex, 0, SETVAL, 1); // set the mutex to 1
    lock = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT); // mutual exclusion semaphore
    semctl(lock, 0, SETVAL, 1); // set the mutex to 1

    for(i = 0; i < N_READ; i++) {
	if((pid[i] = fork()) == 0) { // nth reader
	    sleep(5);
	    srand(time(NULL) + 100*(i+1));
	    signal(SIGUSR1, unchecker);           /* Register SIGINT signal handler for a clean exit */
	    char str[5];
	    array = (int*)shmat(shmarrid, 0,0); // pointer to above shared memory
	    while(check) {
		WAIT(mutex);
		if((*array)==0) {
		    (*array)++;
		    //		    printf("Reader %u locks writer\n",getpid());
		    printf("First reader enters\n");
		    WAIT(lock);
		}
		SIGNAL(mutex);
		fd = open(infile, O_RDONLY);
		read(fd, str, 5);
		printf("Reader %u reads %d\n",getpid(),atoi(str));
		sleep(2);
		close(fd);
		//		sleep(2);
		WAIT(mutex);
		if((*array)==1) {
		    (*array)--;
		    //		    printf("Reader %u releases writer\n",getpid());
		    printf("Last reader leaves\n");
		    SIGNAL(lock);
		}
		SIGNAL(mutex);
		sleep(1+rand()%T_READ_MAX);
	    }
	    shmdt(array);
	    exit(0);
	}
    }
    // writer
    srand(time(NULL));
    int n = 0;
    char buf[5];
    while(check) {
	WAIT(lock);
	printf("writer enters\n");
	fd = open(infile, O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	printf("Writer %u writes %d\n",getpid(),n);
	sprintf(buf,"%d",n);
	write(fd, buf, 5);
	close(fd);
	n++;
	printf("writer leaves\n");
	SIGNAL(lock);
	sleep(1+rand()%T_WRITE_MAX);
	if(n>=MAX) {
	    for(i = 0; i < N_READ; i++) {
		kill(pid[i], SIGUSR1);
	    }
	    check = 0;
	}
    }
    shmdt(array);
    shmctl(shmarrid, IPC_RMID, 0);
    semctl(mutex, 0, IPC_RMID, 0);
    semctl(lock, 0, IPC_RMID, 0);

    return 0;
}
