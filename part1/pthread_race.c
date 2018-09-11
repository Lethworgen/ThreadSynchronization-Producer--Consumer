#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#define NUM_THREADS 10


/**	
 * Can use the following struct for
 * the nanosleep function.
 * the first value is sleep time in seconds
 * the second vaue is sleep time in nanoseconds.
 * 
 * struct timespec ts = {2, 0 };
 * 
 * you can call nano sleep as nanosleep(&ts, NULL);
*/


pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

// global variable with initial value zero
int global = 0;

void *ThreadFun(void *vargp){
	
    // Store the value argument passed to this thread
	int *id = (int *)vargp;
	
	fprintf(stderr, "*******Hello I am thread #%d \n", *id);

pthread_mutex_lock(&m);

    /*copy global to local*/
	int local = global;


	struct timespec ts = {2, 0 };
	nanosleep(&ts, NULL);

	fprintf(stderr, "//////Local Copy thread #%d - %d \n", *id, local);


   	//increments global by 10
	global = global + 10;

	
	
 	//increments local by 10
	local = local + 10;

	fprintf(stderr, "Final Local Copy thread #%d - %d \n", *id, local);

	nanosleep(&ts, NULL);


	/*copy local to global */
	global = local;

pthread_mutex_unlock(&m);

	return NULL;
}


int main() {


	pthread_t tid[NUM_THREADS];


	for (int i = 0; i < NUM_THREADS; i++) {
		int *arg = (int*)malloc(sizeof(int));
		*arg = i;
		pthread_create(&tid[i], NULL, ThreadFun, arg);
		
	}


	for ( int i = 0; i < 10; i++) {
		pthread_join( tid[i], NULL);

	}

	printf("!!!!Global variable %d \n",global);

	return 0;
}
