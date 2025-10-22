/*
* @Author: gaduy
* @Date:   2025-10-22 11:40:25
* @Last Modified by:   gaduy
* @Last Modified time: 2025-10-22 13:57:49
*/
/*
* Code for example 2 lab 2
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

//write from keyboard
void write_from_keyboard(int fd_out) {
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read = 0;
	printf("Enter contents from your keyboard. Press Ctrl + D to complete writing.\n");
	while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
		
		//SHORT COUNT HANDLING
		ssize_t bytes_written = 0;
		ssize_t write_result = 0;

		while (bytes_written < bytes_read) {
			write_result = write (fd_out, 
				buffer + bytes_written, //Skip the written data
				bytes_read - bytes_written); //Only write the leftover data

			if (write_result < 0) {
				perror("Error occured when writing to file.\n");
				close(fd_out);
				exit(EXIT_FAILURE);
			}

			bytes_written += write_result;
		}
		//-------------------------------------

		if (bytes_read < 0) {
			perror("Error occured when read from ur keyboard.\n");
			exit(EXIT_FAILURE);
		} 
	}
}

void copy_from_file (int fd_out, const char *source_filename) {
	int fd_in;
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;

	fd_in = open(source_filename, O_RDONLY);
	if (fd_in < 0) {
		perror("Error when open source file.\n");
		exit(EXIT_FAILURE);
	}

	while ((bytes_read = read(fd_in, buffer, BUFFER_SIZE)) > 0) {
		//SHORT COUNT HANDLING
		ssize_t bytes_written = 0;
		ssize_t write_result = 0;

		while (bytes_written < bytes_read) {
			write_result = write (fd_out,
				buffer + bytes_written,
				bytes_read - bytes_written);

			if (write_result < 0) {
				perror("Error when write do the dest.\n");
				close(fd_in);
				close(fd_out);
				exit(EXIT_FAILURE);
			}
			bytes_written += write_result;
		}
		//--------------------------------------

		if (bytes_read < 0) {
			perror ("Error when read from source.");
			close(fd_in);
			exit(EXIT_FAILURE);
		}
	}
}

void usage_error (const char *prog_file) {
	fprintf(stderr, "Usage: \n");
	fprintf(stderr, "%s -k <output_file>\n", prog_file);
	fprintf(stderr, "%s -f <source_file> <destinate_file>\n", prog_file);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int opt;
	int mode = 0;
	char *source_filename = NULL;
	char *output_filename = NULL;

	int fd_out;

	while ((opt = getopt(argc, argv, "kf:")) != -1) {
		switch (opt) {
		case 'k':
			if (mode != 0) {
				usage_error(argv[0]);
			}
			mode = 1;
			break;

		case 'f':
			if (mode != 0) usage_error(argv[0]);
			mode = 2;
			source_filename = optarg;
			break;
		default:
			usage_error(argv[0]);
		}
	}

	if (mode == 0) {
		fprintf(stderr, "Wrong usage: -f or -k\n");
		usage_error(argv[0]);
	}

	if (argc - optind != 1) {
		fprintf(stderr, "Error: Wrong input arguments\n");
		usage_error(argv[0]);
	}

	output_filename = argv[optind];
	fd_out = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);//rw-rw-r--

	if (mode == 1) {
		write_from_keyboard(fd_out);
	} else {
		copy_from_file(fd_out, source_filename);
	}

	close (fd_out);

	return EXIT_SUCCESS;
}