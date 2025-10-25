/*
* @Author: gaduy
* @Date:   2025-10-24 20:37:41
* @Last Modified by:   gaduy
* @Last Modified time: 2025-10-24 21:45:46
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

static int idata = 1000; //Allocated in Data segment

int main() {
	int istack = 150; //Allocated in Stack segment
	int fd;
	pid_t childPid;
	char buffer[100]; //buffer for reading file

	fd = open("Hello.txt", O_RDWR | O_CREAT | O_TRUNC, 0664);
	if (fd < 0) {
		perror("Error inside child");
		exit(EXIT_FAILURE);
	}

	//Write some lines into source file
	char sample_data[] = "The quick brown fox jumps over the lazy dog";
	write(fd, sample_data, sizeof(sample_data) - 1); //Neglect the end of line character;

	//We should take the file pointer comeback to start point of soure file
	lseek(fd, 0, SEEK_SET);

	printf("Sample data inside Hello.txt: %s\n", sample_data);

	childPid = fork();

	if (childPid < 0) {
		perror("Error inside child");
		exit(EXIT_FAILURE);
	}

	if (childPid == 0) {
		//getpid() to get the real pid of this process
		printf("CHILD: PID: %d, Original values: idata = %d, istack = %d.\n", getpid(), idata, istack);

		//Changes idata and istack
		idata = 18;
		istack = 36;
		printf("CHILD: New data: idata = %d, istack = %d.\n", idata, istack);

		//Changes the file offset to 7th position
		off_t new_offset = lseek(fd, 7, SEEK_SET); //current pos: 7th byte

		if (close(fd) < 0) {//child close its fd
			perror("Error inside child");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	} else {

		//wait for child process to be terminated
		wait(NULL);
		printf("PARENT: PID: %d, Your child has been terminated.\n", getpid());

		//Prints the data inside parent process
		printf("PARENT: My values, after my child modified the same name data: idata = %d, istack = %d\n", idata, istack);

		//Read file
		ssize_t bytes_read = read(fd, buffer, 99);
		if (bytes_read < 0) {
			perror("Error inside parent");
			exit(EXIT_FAILURE);
		} else {
			buffer[bytes_read] = '\0'; //Due to the length difference between
										//the sample data and buffer
			printf("PARENT: Contents from Hello.txt: %s.\n", buffer);
		}

		if (close(fd) < 0) {
		//Parent close its fd
			perror("Error inside parent");
			exit(EXIT_FAILURE);
		}

	}

	return EXIT_SUCCESS;
}