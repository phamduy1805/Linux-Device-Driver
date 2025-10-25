/*
* @Author: gaduy
* @Date:   2025-10-24 22:35:01
* @Last Modified by:   gaduy
* @Last Modified time: 2025-10-25 11:24:23
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h> //for signal(), SIGINT
#include <unistd.h> // for sleep()
#include <dirent.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()
#include <string.h> //for strcmp(), snprintf()
#include <time.h> //for time, ctime()

/**
 * SIGNAL HANDLING FOR SIGINT
 * Execute when user press Ctrl + D
 */
void signal_handler(int sig) {

	printf("Program is terminated by user.\n");
	exit(EXIT_SUCCESS);
}

/***
 * Traverse and print current regular file
 * in the specific directory
 */
void list_files_in_dir(const char *dir_path) {
	DIR *dp; // Pointer represent for directory stream
	struct dirent *entry; //Struct stores information for an entry in this directory
	struct stat statbuf; //for storing metadata
	char full_path[1024]; //buffer for storing full path

	dp = opendir(dir_path);
	if (dp == NULL) {
		perror("opendir");
		return; //Skip this iteration, wait for next 1 minute
	}

	//Read every entry in this directory
	while ((entry = readdir(dp)) != NULL) {

		//We must neglect the current and parent directory (. and ..)
		// just list all the regular files in current directory
		// avoid infinite traverse between directry
		if (strcmp(entry->d_name, ".") == 0 || 
			strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		//Allocate the current entry into fullpath buffer
		//so we can use stat() with full - path
		snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

		//Take the metadata of this entry
		if (stat(full_path, &statbuf) == -1) {
			perror(full_path);
			continue;
		}

		//Neglect the directory and symbolic links
		if (S_ISREG(statbuf.st_mode)) {
			printf("- %s\n", entry->d_name);
		}
	}

	closedir(dp);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: ./%s <path_of_dir>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char *path_to_monitor = argv[1];

	//Register the SIGINT handler
	signal(SIGINT, signal_handler);

	printf("We will check this directory every 1 minute. Press Ctrl + C to terminated the program.\n");

	while (1) {
		time_t now = time(NULL);
		printf("Last check at: %s", ctime(&now));

		list_files_in_dir(path_to_monitor);

		sleep(60);
	}

	return EXIT_SUCCESS;
}

