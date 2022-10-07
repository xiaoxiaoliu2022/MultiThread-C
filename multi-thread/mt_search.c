#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// ****************************
// This code is provided as an *optional* framework to use to 
// write your multi-threaded search code for Assignment A8. 
// 
// Feel free to change anything you want to in this. 
// ****************************


// ===================================================
// Global Variables
// ===================================================

#define MAX_NUM_RESULTS 100 
#define MAX_NUM_THREADS 32
int num_data_points = 0;
int section_size;

// Array of numbers to search
int* data = NULL;

// Number to be found
int target;

// Array of indices of target number 
int results[MAX_NUM_RESULTS];
// Number of results we have so far
int num_results = 0;
int num_lines;

// array holding the start idx of each thread
int start_idx[MAX_NUM_THREADS];

// Used for synchronization. 
// I encourage you to rename this something more meaningful. 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// All the threads (an array)
int num_threads;
pthread_t* threads;

// ===================================================
// Functions
// ===================================================

// The function that each thread will run 
// to look for the target value in the array. 
// The argument provided is a pointer to the array
// where this thread should start looking for the target. 
// (This can be either the index OR the address. Your choice.)
// Each thread will look at a max of section_size elements. 
void* DoSearch(void* start_address) {
	// wait until the file reading is completed at the main thread
	pthread_mutex_lock(&lock);
	pthread_mutex_unlock(&lock);

	int start_idx = *(int*)start_address;

	printf("Ready to work on [%d, %d)\n", start_idx, start_idx + section_size);
	for (int i = start_idx;i < (start_idx + section_size) && i < num_data_points && num_results < MAX_NUM_RESULTS;i++) {
		if (data[i] == target) {
			pthread_mutex_lock(&lock);
			if (num_results < MAX_NUM_RESULTS) {
				// add 1 to the result since line number is 1-indexed
				results[num_results] = i + 1;
				num_results += 1;
			}
			pthread_mutex_unlock(&lock);
		}
	}

	return NULL;
}

// Creates all the threads and starts them running. 
// Makes sure the thread IDs are stored in the global threads array. 
void MakeThreads(int num_threads) {
	threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
	if (threads == NULL) {
		fprintf(stderr, "Failed to allocate memory.");
		free(data);
		exit(1);
	}

	// section_size = ceil(num_lines / num_threads)
	section_size = (num_lines % num_threads == 0) ? num_lines / num_threads
		: num_lines / num_threads + 1;


	printf("Spawning threads\n");
	for (int i = 0; i < num_threads; i++) {
		printf("Starting thread %d\n", i);
		start_idx[i] = i * section_size;
		pthread_create(&threads[i], NULL, DoSearch, &start_idx[i]);
	}
}

// Opens the file called filename. 
// Reads line by line, adding each number to the 
// global data array. 
void ReadFile(char* filename) {
	FILE* f;
	int x;

	f = fopen(filename, "r");
	if (f == 0) {
		perror(filename);
		exit(1);
	}

	data = (int*)malloc(sizeof(int) * num_lines);
	if (data == NULL) {
		fprintf(stderr, "Failed to allocate memory.");
		exit(1);
	}
	while (fscanf(f, "%d", &x) == 1) {
		data[num_data_points] = x;
		num_data_points++;
	}

	fclose(f);
}

// Prints out the results, stored in the 
// global "results" array. 
void PrintResults() {
	printf("[");
	for (int i = 0;i < num_results;i++) {
		printf("%d ", results[i]);
	}
	printf("]\n");

}

int main(int num_args, char* args[]) {
	// START HERE
	// Read in the arguments and populate global variables. 
	if (num_args != 5) {
		fprintf(stderr, "Usage: ./mt_search <filename> <number to search for> <number of threads> <num lines>.\n");
		exit(3);
	}
	char* filename = args[1];

	int parsed = sscanf(args[2], "%d", &target);
	if (parsed != 1) {
		fprintf(stderr, "<number to search for> must be an integer.\n");
		exit(3);
	}

	parsed = sscanf(args[3], "%d", &num_threads);
	if (parsed != 1 || num_threads <= 0) {
		fprintf(stderr, "<number of threads> must be a positive integer.\n");
		exit(3);
	}

	parsed = sscanf(args[4], "%d", &num_lines);
	if (parsed != 1 || num_lines <= 0) {
		fprintf(stderr, "<num lines> must be a positive integer.\n");
		exit(3);
	}


	// Create the threads
	pthread_mutex_lock(&lock);
	MakeThreads(num_threads);

	// Read the file
	ReadFile(filename);
	// release lock after readfile
	pthread_mutex_unlock(&lock);

	// Wait for threads to finish doing their job
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	printf("Threads have joined\n");

	// Print out the results
	printf("%d target %d was found in positions (1-indexed):\n", num_results, target);
	PrintResults();

	// Clean up everything! 
	// Free everything you malloc'd. 
	// Don't forget to destroy the semaphore and mutex, which 
	// get malloc'd when you create them. 
	free(data);
	free(threads);
	return 0;
}
