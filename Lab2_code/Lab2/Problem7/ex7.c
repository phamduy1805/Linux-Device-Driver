/*
* @Author: gaduy
* @Date:   2025-10-25 17:47:18
* @Last Modified by:   gaduy
* @Last Modified time: 2025-10-25 21:26:48
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 3	//Size of buffer for P and C
#define NUM_OF_ITEMS 8	//Total amount of items will be produce
#define PRODUCER_SLEEP 0//for producer to sleep after produce one item
#define CONSUMER_SLEEP 0//for consumer to sleep after consume a item

int buffer[BUFFER_SIZE];
int counter = 0;		//for couting the number of items in buffer
int in_idx = 0; 		//index (the next position) for producer to write in buffer
int out_idx = 0;		//index (the next position) for consumer to read from buffer

/* MuTex to protect the buffer and global variables*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/* Consumer will wait if buffer is empty, depends on this variable*/
pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;
/* Producer will wait if buffer is full, depends on this variables*/
pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
	int item;
	for (item = 0; item < NUM_OF_ITEMS; item++) {
		//critical: add item to buffer
		pthread_mutex_lock(&mutex);

		//Synchronization: wait if buffer is full
		while (counter == BUFFER_SIZE) {
			printf("PRODUCER: buffer is full...\n");
			pthread_cond_wait(&cond_empty, &mutex);
		} 

		//add item into buffer
		buffer[in_idx] = item;
		printf("PRODUCER: I've just added an item %d at index %d\n", item, in_idx);
		in_idx = (in_idx + 1) % BUFFER_SIZE;
		counter++;

		//Signal to consumer that buffer is now not empty
		pthread_cond_signal(&cond_full);

		pthread_mutex_unlock(&mutex);
		//end of critical section

		if (PRODUCER_SLEEP > 0) {
			sleep(PRODUCER_SLEEP);
		}
	}
	printf("PRODUCER: I have produced %d items. Task completed.\n", NUM_OF_ITEMS);
	return NULL;
}

void *consumer(void *arg) {
	int item;
	int items_consumed = 0; //count the total consumed items
	while (items_consumed < NUM_OF_ITEMS) {
		//critical section: get item from buffer
		pthread_mutex_lock(&mutex);

		//must wait if the buffer is empty
		while (counter == 0) {
			printf("CONSUMER: buffer is empty. I should wait ...\n");
			pthread_cond_wait(&cond_full, &mutex);
		}

		//Get item from buffer
		item = buffer[out_idx];
		printf("CONSUMER: I've just consumed item %d at index %d.\n", item, out_idx);
		out_idx = (out_idx + 1) % BUFFER_SIZE;
		counter--;

		//Signal to the producer that the buffer has some empty indexs
		pthread_cond_signal(&cond_empty);

		pthread_mutex_unlock(&mutex);
		//end of critical

		items_consumed++;

		if (CONSUMER_SLEEP > 0) {
			sleep(CONSUMER_SLEEP);
		}
	}
	printf("CONSUMER: I've just consumed %d items. No need to consume anymore.\n", NUM_OF_ITEMS);
	return NULL;
}

int main() {

	pthread_t producer_thr;
	pthread_t consumer_thr;

	printf("We will begin the Producer - Consumer cycle right now. Please be focus.\n");

	if (pthread_create(&producer_thr, NULL, producer, NULL) != 0) {
		perror("FAILED to create producer thread.\n");
		return EXIT_FAILURE;
	}

	if (pthread_create(&consumer_thr, NULL, consumer, NULL) != 0) {
		perror("FAILED to create consumer thread.\n");
		return EXIT_FAILURE;
	}

	//We must wait until the cycle complete
	pthread_join(producer_thr, NULL);
	pthread_join(consumer_thr, NULL);
	return EXIT_SUCCESS;
}