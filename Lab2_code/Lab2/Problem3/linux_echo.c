/*
* @Author: gaduy
* @Date:   2025-10-23 14:32:19
* @Last Modified by:   gaduy
* @Last Modified time: 2025-10-23 15:02:38
*/
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h> //For open(), FLAGS
#include <unistd.h> //For write(), close()
#include <sys/stat.h>

int main(int argc, char *argv[]) {
	int fd; //File descriptor for output file
	const char *string_to_write;
	const char *filename;
	ssize_t len;

	//The program must contains three arguments:
	// argv[0]: name of the program: ./linux_echo
	// argv[1]: string to write
	// argv[2]: output file
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <string> <output_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	string_to_write = argv[1];
	filename = argv[2];

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);//rw-rw-r--
	if (fd < 0) {
		perror(filename);
		exit(EXIT_FAILURE);
	}

	len = strlen(string_to_write);
	ssize_t bytes_written = 0;
	while (bytes_written < len) {//SHORT COUNT HANDLING
		ssize_t write_result = write (fd,
			string_to_write + bytes_written,
			len - bytes_written);
		if (write_result < 0) {
			perror("Error occured.");
			close(fd);
			exit(EXIT_FAILURE);
		}

		bytes_written += write_result;
	}

	//Add the new line character at the end of the string
	if (write(fd, "\n", 1) != 1) {
		perror("Error when adding the new line character");
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (close(fd) < 0) {
		perror("Error when close file.");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
