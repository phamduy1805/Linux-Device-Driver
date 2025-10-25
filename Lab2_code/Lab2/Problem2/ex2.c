/*
* @Author: gaduy
* @Date:   2025-10-22 21:11:29
* @Last Modified by:   gaduy
* @Last Modified time: 2025-10-22 21:59:57
*/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void usage_error(const char *prog_name) {
	fprintf(stderr, "Usage: %s <file_name>\n", prog_name);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

	struct stat fileStat; // variable for storing metadata

	if (argc != 2) {
		usage_error(argv[0]);
	}

	// Obtain metadata of the file by stat() syscall
	if (stat(argv[1], &fileStat) < 0) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	//Print the file size using st_size
	printf("Size: %lld bytes\n", (long long)fileStat.st_size);

	//Print the permissions mode
	char perms[11]; //for print:-rwxrwxrwx or something like that

	//Define the file type
	if (S_ISDIR(fileStat.st_mode)) perms[0] = 'd';
	else if (S_ISLNK(fileStat.st_mode)) perms[0] = 'l';
	else perms[0] = '-';

	// Leftover is for print permissions
	//Check the mode using mask and AND bit operator
	perms[1] = (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
	perms[2] = (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (fileStat.st_mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (fileStat.st_mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (fileStat.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (fileStat.st_mode & S_IXOTH) ? 'x' : '-';

    perms[10] = '\0'; //NULL terminator

    printf("Permissions: %s\n", perms);

	return EXIT_SUCCESS;
}