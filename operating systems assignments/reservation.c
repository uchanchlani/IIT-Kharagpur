/*
 * Assgnment on threads
 * Submitted By:
 * Utkarsh Chanchlani: 11CS10049
 * Sudarshan Maru: 11CS10028
 * NOTE: in this assignment, we've done queries of the three types randomly namely:
 * Type 1: check seats
 * Type 2: reserve some tickets in some train, maximum seats at a time is 6 (as in standard indian railways booking counter)
 * Type 3: cancel a ticket previously booked by the same thread (note partial cancellation is not permitted, a ticket is cancelled as a whole)
 * Type 1 query occurs with 0.3 probability, Type 2 query occurs with 0.5 probability and Type 3 query occurs with 0.2 probability
 * This is because if all probabilities are same then in long run (10mins) the remaining seats in all the trains is almost 500, because cancellation number is same as reservation number
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define t 100 // number of trains
#define c 500 // number of seats per train
#define s 20 //  maximum no of threads
#define READ 1 // READ state of the main table
#define WRITE 2 // WRITE state of the main table
#define MAX 5 // max queries at a time
#define SLEEP_T 5 // maximum sleep time of a particular thread
#define MAX_TCKT 100 // max tickets to book in a single transaction
#define W_Time 6 // parent's wait time

/*
 * every thread maintains a table of reservations, which is denoted by this struct
 */
typedef struct _node {
  int tno;
  int nresv;
  struct _node *next;
} node, *BOOKINGS;

/*
 * this is the table to check the present read write state of the trains
 * main thread initializes this table
 * used by all the threads
 */
typedef struct {
  int tno;
  int state;
  int nthread;
} STATUS;

int nqueries = 0; // global shared variable, to see how many threads are doing the queries at that time
int *train; // array to store the number of seats available in the train
STATUS *main_table; // main table which shows the current status of the database accesses provided
int check = 1; // shared variable, shich will be made 0 when the main thread wants all the threads to end

pthread_mutex_t pmutex;
pthread_cond_t query_cond; // pmutex is the mutex to access the critical zone. query_cond is to make sure that at a particular time, only MAX queries are active


/*
 * allocates a new transaction node which will be appended to the history
 */
node *get_node(int tno, int nresv) {
  node* new_book = (node*)malloc(sizeof(node));
  new_book->tno = tno;
  new_book->nresv = nresv;
  new_book->next = NULL;
  return new_book;
}

/*
 * add the particular booking in the booking history
 */
void add_booking(int tno, int nresv, BOOKINGS* table) {
  BOOKINGS new_node = get_node(tno, nresv);
  new_node->next = NULL;
  if((*table) == NULL) {
	*table = new_node;
	return;
  }
  BOOKINGS p = *table;
  while(p->next != NULL)
	p = p->next;
  p->next = new_node;
  return;
}

/*
 * delete the least recent booking
 */
node *del_booking(BOOKINGS *table) {
  if((*table) == NULL)
	return NULL;
  node *ret = *table;
  *table = (*table)->next;
  return ret;
}

/*
 * clear the booking history
 */
void clear_history(BOOKINGS *table) {
  BOOKINGS p = *table, q;
  while(p!=NULL) {
	q = p;
	p = p->next;
	free(q);
  }
  *table = NULL;
}

/*
 * a thread before entering the critical zone will look in the master table if it's allowed to do so
 * if it's allowed, it also updates the master table
 */
int grant_permission(int train_n, int type) {
  int i;

  // check if there is already an entry for that train
  for(i = 0; i < MAX; i++) 
	if(main_table[i].tno == train_n)
      break;

  // if yes then modify that entry if applicable    
  if(i < MAX) {
	if((main_table[i].state == type) && (type == READ)) {
      main_table[i].nthread++;
      return 1;
	}
	else {
      return 0;
	}
  }

  // if entry not found then get the minimum blank entry
  for(i = 0; i < MAX; i++)
	if(main_table[i].tno == -1)
      break;
  if(i < MAX) {
	main_table[i].tno = train_n;
	main_table[i].state = type;
	main_table[i].nthread = 1;
	return 1;
  }
  else
	return 0;    
}

/*
 * after the threads work is done in the critical zone, delete it's entry from the master table
 */
void done(int train_n, int type) {
  int i;
  for(i = 0; i < MAX; i++)
	if(main_table[i].tno == train_n) {
      break;
	}
  if((i < MAX) && (main_table[i].state == type)) {
	main_table[i].nthread--;
	if(main_table[i].nthread == 0)
      main_table[i].tno = -1;
  }
}

/*
 * the main function of the thread
 */
void *pthread_main(void *pthread_argv) {\
  int th_id = *((int*)pthread_argv);
  int choice = 0, s_time, ntckt, ntrain;
  srand((unsigned int)time(NULL) + (unsigned int)100*th_id);
  BOOKINGS my_history = NULL, curr_book;
  while(check) {
	// thread sleeps for some time
	s_time = rand()%SLEEP_T + 1;
	sleep(s_time);

	// select a random query, train and number of tickets (for type 2)
    choice = rand()%10;
	ntrain = rand()%t;
	ntckt = rand()%MAX_TCKT + 1;

	// if there are MAX queries in the query zone then wait
	pthread_mutex_lock(&pmutex);
	nqueries++;
	if(nqueries > MAX) {
      pthread_cond_wait(&query_cond, &pmutex);
      pthread_mutex_unlock(&pmutex);
	}
	else
      pthread_mutex_unlock(&pmutex);

	switch(choice) {
      // check reservations
	case 0:
	case 1:
	case 2:
      // grant permission requires access of global memory, thus, mutexing is required
      pthread_mutex_lock(&pmutex);
      if(grant_permission(ntrain, READ)) {
		pthread_mutex_unlock(&pmutex);

		// sleeps for a short time
		s_time = rand()%SLEEP_T + 1;
		sleep(s_time);

		printf("Thread %d:\ttype 1 query:\ttrain %d -> %d seats available\n", th_id, ntrain, train[ntrain]);

		// done requires access of global memory, thus, mutexing is required
		pthread_mutex_lock(&pmutex);
		done(ntrain, READ);
		pthread_mutex_unlock(&pmutex);
      }
      else {
		pthread_mutex_unlock(&pmutex);
		printf("Thread %d:\ttype 1 query:\tpermission denied to view the data for train %d\n", th_id, ntrain);
      }
      break;

      // book some tickets
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
      // grant permission requires access of global memory, thus, mutexing is required
      pthread_mutex_lock(&pmutex);
      if(grant_permission(ntrain, WRITE)) {
		pthread_mutex_unlock(&pmutex);

		// sleeps for a short time
		s_time = rand()%SLEEP_T + 1;
		sleep(s_time);

		if(ntckt > train[ntrain]) {
          printf("Thread %d:\ttype 2 query:\ttrain %d -> asked %d seats but only %d seats available\n", th_id, ntrain, ntckt, train[ntrain]);
          ntckt = train[ntrain];
		}
		train[ntrain] -= ntckt;
		printf("Thread %d:\ttype 2 query:\ttrain %d -> %d seats reserved\n", th_id, ntrain, ntckt);
		add_booking(ntrain, ntckt, &my_history);

		// done requires access of global memory, thus, mutexing is required
		pthread_mutex_lock(&pmutex);
		done(ntrain, WRITE);
		pthread_mutex_unlock(&pmutex);
      }
      else {
		pthread_mutex_unlock(&pmutex);
		printf("Thread %d:\ttype 2 query:\tpermission denied to book seats for train %d\n", th_id, ntrain);
      }
      break;

      // cancel a transaction
	case 8:
	case 9:
      if(my_history == NULL) {
		printf("Thread %d:\ttype 3 query:\tEMPTY BOOKING HISTORY!! NOTHING TO CANCEL\n", th_id);
		break;
      }
      ntrain = my_history->tno;

      // grant permission requires access of global memory, thus, mutexing is required
      pthread_mutex_lock(&pmutex);
      if(grant_permission(ntrain, WRITE)) {
		pthread_mutex_unlock(&pmutex);

		// sleeps for a short time
		s_time = rand()%SLEEP_T + 1;
		sleep(s_time);

		curr_book = del_booking(&my_history);
		train[ntrain] += curr_book->nresv;
		printf("Thread %d:\ttype 3 query:\ttrain %d -> %d tickets canceled\n", th_id, ntrain, curr_book->nresv);
		free(curr_book);

		// done requires access of global memory, thus, mutexing is required
		pthread_mutex_lock(&pmutex);
		done(ntrain, WRITE);
		pthread_mutex_unlock(&pmutex);
      }
      else {
		pthread_mutex_unlock(&pmutex);
		printf("Thread %d:\ttype 3 query:\tpermission denied to cancel tickets of train %d\n", th_id, ntrain);
      }
      break;

	default:
      printf("ERROR in choice...\n");
      break;
	}

	// query of this thread is complete, if some thread is waiting signal it
	pthread_mutex_lock(&pmutex);
	if(nqueries > MAX) {
      nqueries--;
      pthread_cond_signal(&query_cond);
	} else {
      nqueries--;
	}
	pthread_mutex_unlock(&pmutex);
  }

  //clear the booking history and exit
  clear_history(&my_history);
  printf("Thread %d:\texitting...\n",th_id);
}

/*
 * create the mutexes and test them
 */
void create_mutex() {
  // initialize mutexes here
  pthread_mutex_init(&pmutex, NULL);
    
  pthread_mutex_trylock(&pmutex);   /* Try to lock mutex (non-blocking) */
  pthread_mutex_unlock(&pmutex);    /* Now, unlock the mutex */

  /* Initialize condition variable */
  pthread_cond_init(&query_cond, NULL);

}

/*
 * signals the threads to exit and waits for them
 */
void killThreads(pthread_t *tid) {
  printf("\t\t\t\t\t\t\tMAIN THREAD:\tSignals all threads to complete their last query and exit\n");
  check = 0;
  int i;
  void** ptr;

  // call the join function one by one
  for(i = 0; i < s; i++) {
	if(pthread_join(tid[i], ptr) != 0)
      perror("\t\t\t\t\t\t\tMAIN THREAD:\tERROR IN JOINING THREADS\n");
	else
      printf("\t\t\t\t\t\t\tMAIN THREAD:\tTHREAD %d, id %lu joined\n",i+1,tid[i]);
  }
}

/*
 * final windup,
 * deletion of mutexes
 * freeing of allocated memory
 */
void wind_up(int** arg, pthread_t** tid) {
  // add mutex deletions here
  pthread_mutex_destroy(&pmutex);
  pthread_cond_destroy(&query_cond);

  free(train); train = NULL;
  free(main_table); main_table = NULL;
  free(*arg); *arg = NULL;
  free(*tid); *tid = NULL;
  //    free(thread_status); thread_status = NULL;
}

/*
 * the main function
 */
int main(int argc, char *argv[]) {
  int i;
  int *arg; // arguments sent to threads is just a id which we give to the thread
  pthread_attr_t attr;

  // allocation of memory
  pthread_t *tid = (pthread_t*)malloc(s*sizeof(pthread_t));
  train = (int*)malloc(t*sizeof(int));
  main_table = (STATUS*)malloc(MAX*sizeof(STATUS));
  arg = (int*)malloc(s*sizeof(int));
  //    thread_status = (int*)malloc(s*sizeof(int));

  // memory initialization
  for(i = 0; i < t; i++)
	train[i] = c;
  for(i = 0; i < MAX; i++)
	main_table[i].tno = -1;

  create_mutex(); // create the mutexes which will be used

  // initialize the attr attribute
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // create s threads
  for(i = 0; i < s; i++) {
	arg[i] = i+1;
	//	thread_status[i] = 1;
	if(pthread_create(tid + i, &attr, pthread_main, (void*)(arg+i))) {
      perror("\t\t\t\t\t\t\tMAIN THREAD:\tUnable to create thread\n");
      pthread_attr_destroy(&attr);
      exit(1);
	}
	printf("\t\t\t\t\t\t\tMAIN THREAD:\tthread %d, id %lu created\n",arg[i], tid[i]);
  }

  // main thread will rest for the time specified
  sleep(W_Time);

  // after waking up, first task is to kill all the threads
  killThreads(tid);

  // and then print the status of all trains
  printf("\n\n\tMAIN THREAD::\tSTATUS...\n");
  for(i = 0; i < t; i++)
	printf("Train number %d:\tseats left %d\n",i,train[i]);

  // finally wind up all the memories and mutexes
  wind_up(&arg, &tid);

  // and exit
  exit(0);
}

// main ends here
