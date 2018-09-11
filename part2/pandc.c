#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>


// mutex lock 
pthread_mutex_t buffer_lock;

// semaphores 
sem_t full;
sem_t empty;

int Ptime;
int Ctime;


struct bounded_buffer {
    int size;
    int* buffer;
    int start;              // starting point of array 
    int end;                // ending point of array 
} main_buffer;


struct thread_info {
    pthread_t thread_id;    // pthreads id
    int thread_num;         // assigned id
    int totalthreads;        // total number of threads
    int items;        // items to produce and consume
};


  // Adds items to bounded buffer as empty spaces
 
void* produce(void * arg){
    int counter = 0;
    struct thread_info *info = arg;


    while((*info).items--) {
    	sleep(Ptime);
        sem_wait(&empty);
        pthread_mutex_lock(&buffer_lock);
        
        main_buffer.buffer[main_buffer.end] = counter * (*info).totalthreads + (*info).thread_num;
        printf("%i produced -> %d \n", main_buffer.buffer[main_buffer.end], (*info).thread_num);
        main_buffer.end = (main_buffer.end + 1) % main_buffer.size;
        
        pthread_mutex_unlock(&buffer_lock);
        sem_post(&full);
        
        counter++;
    }
    return 0;
}


// Consumes items from bounded buffer as full entries
void* consume(void* arg){
    int counter = 0;
    struct thread_info *info = arg;
    

    while((*info).items--) {
    	sleep(Ctime);
        sem_wait(&full);
        pthread_mutex_lock(&buffer_lock);
        
        if (main_buffer.buffer[main_buffer.start] != 0){
        	printf("%i consumed -> %d \n", main_buffer.buffer[main_buffer.start], arg);
        }        

        main_buffer.buffer[main_buffer.start] = 0;
        main_buffer.start = (main_buffer.start + 1) % main_buffer.size;
        
        pthread_mutex_unlock(&buffer_lock);
        sem_post(&empty);
        
        counter++;
    }
    return 0;
}


 // Spawns consumer and producer threads and assigns number of items 
 
void spawn_threads(int producer_count, int consumer_count, int items_per_producer){
    int num;
    struct thread_info *info;

    int items_per_consumer = (producer_count*items_per_producer)/consumer_count;
    
    info = calloc(producer_count + consumer_count, sizeof(struct thread_info));
    
    // create and set attributes of producer threads
    for (num = 1; num <= producer_count; num++) {
        info[num].thread_num = num;
        info[num].items = items_per_producer;
        info[num].totalthreads = producer_count;
        
        if(pthread_create(&info[num].thread_id, NULL, &produce, &info[num]))
        {
            fprintf(stderr,"Error in creating producer thread.\n");
            exit(2);
        }
    }
    
    // create and set attriutes of consumer threads
    for (;num <= producer_count + consumer_count; num++) {
        info[num].thread_num = num;
        info[num].items = items_per_consumer;
        
        
        if(num - producer_count <= (items_per_consumer))
            info[num].items++;
        
        if(pthread_create(&info[num].thread_id, NULL, &consume, &info[num])) {
            fprintf(stderr,"Error in creating consumer thread.\n");
            exit(2);
        }
    }
    num--;
    
    // wait for all threads to exit
    while(num >= 0) {
        pthread_join(info[num].thread_id, NULL);
        num--;
    }
    
    free(info);
}


 
int main(int argc, const char * argv[]) {
    
    if(argc != 7) {
        fprintf(stderr, "Invalid Input.\n Program must be called with command line arguments (NO ','):\n ./pandc buffer_size, num_producers, num_consumers, X, Ptime, Ctime\n");
        exit(1);
    }
    
    // start time
    time_t start; 
    start=time(NULL);
    printf("Current Time: %s\n",asctime( localtime(&start) ) );

    // grab command line arguments
    int buffer_size = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);
    int X = atoi(argv[4]);
    int Producetime = atoi(argv[5]);
    int Consumetime = atoi(argv[6]);

    // initialize bounded_buffer struct
    int buffer[buffer_size];

    main_buffer.size = buffer_size;
    main_buffer.buffer = buffer;
    main_buffer.start = 0;
    main_buffer.end = 0;
    Ptime = Producetime;
	Ctime = Consumetime;

    // initialize semaphores
    sem_init(&full, 0, 0) == NULL;
    sem_init(&empty, 0,main_buffer.size) == NULL ; 
    
    // initialize mutex lock
    pthread_mutex_init(&buffer_lock,NULL);
    
    // print variables and run simulation
    printf("Running producer/consumer with: \n %i element buffer, %i producers, %i consumers, %i X items, %i Ptime and %i Ctime.\n",buffer_size,num_producers,num_consumers,X,Producetime,Consumetime);
    spawn_threads(num_producers,num_consumers, X);

    // end time
    time_t end; 
    end=time(NULL); 
    printf("Current Time: %s\n",asctime( localtime(&end) ) );

    // Duration of the code
    double duration = difftime(end, start);

      printf("Time taken to execute in seconds : %f \n", duration);
}






