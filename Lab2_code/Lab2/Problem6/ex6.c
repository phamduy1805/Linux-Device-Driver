/*
* @Author: gaduy
* @Date:   2025-10-25 14:12:52
* @Last Modified by:   gaduy
* @Last Modified time: 2025-10-25 15:21:37
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_OF_THREADS 4 //NUMBER OF THREADS

pthread_mutex_t barrier_mutex = PTHREAD_MUTEX_INITIALIZER; //mutex for count synchronization
pthread_cond_t barrier_cond = PTHREAD_COND_INITIALIZER; //Variable condition for signal and wait
int counter = 0;
int num_threads = NUM_OF_THREADS; //Total thread for this program

/**
 * This function is for every thread
 * to check whether this thread has reached the barrier or not
 */
void *thread_procedure(void *arg) {
	long thread_id = (long)arg; //Take the thread's id

	printf("Thread %ld reached the barrier.\n", thread_id);

	/**
	 * This thread has reached the barried, now we should
	 * increase the counter
	 * check if this is final thread, then release all the threads
	 * if it not, wait there 
	 */

	//---------CRITICAL SECTION-------------

	pthread_mutex_lock(&barrier_mutex);

	counter++;

	if (counter == num_threads) {	//is this final thread?
		printf("Thread %ld is the last one.\n", thread_id);
		printf("All threads reached the barrier. Releasing them ...\n");

		//Wake all threads up
		pthread_cond_broadcast(&barrier_cond);
	} else {
		printf("Thread %ld is waiting at the barrier. Until now we have %d/%d threads is waiting\n", thread_id, counter, num_threads);

		/**
		 * Wait the signal from the final one
		 * pthread_cond_wait auto unlock mutex when waiting
		 * and auto lock mutex then released.
		 * (Obviously, thread still in critical section after released
		 * so this function must unlock it after the thread released.)
		 */
		pthread_cond_wait(&barrier_cond, &barrier_mutex);

		printf("Thread %ld was released from the barrier. \n", thread_id);
	}

	pthread_mutex_unlock(&barrier_mutex); //every thread should unlock mutex
	//--------------------------------------------------------------

	printf("Thread %ld passed the barrier.\n", thread_id);\
	return NULL;
}

int main() {
	pthread_t threads[NUM_OF_THREADS];

	printf("Begin creating threads ...\n");

	//Creates threads
	long i;
	for (i = 0; i < NUM_OF_THREADS; i++) {
		if (pthread_create(&threads[i], NULL, thread_procedure, (void *)i) != 0) {
			perror("Error occured when creating threads");
			return 1; //!= 0
		}
	}

	//Main thread wait for all other threads complete its execution
	printf("Waiting for threads to finish ....\n");
	for (i = 0; i < NUM_OF_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	printf("ALL THREADS FINISHED ITS WORK\n");
	return EXIT_SUCCESS;
}
