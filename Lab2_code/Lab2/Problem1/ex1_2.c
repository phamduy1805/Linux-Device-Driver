/*
* @Author: gaduy
* @Date:   2025-10-22 14:38:33
* @Last Modified by:   gaduy
* @Last Modified time: 2025-10-22 14:51:56
*/

//C Standard Library Version

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

void write_from_keyboard_stdio(FILE *fp_out) {
	char buffer[BUFFER_SIZE];
	printf("Enter contents from ur keyboard. Press Ctrl + D to complete writing.\n");
	while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
		if (fputs(buffer, fp_out) == EOF) {
			perror("Error");
			fclose(fp_out);
			exit(EXIT_FAILURE);
		}
	}
}

void copy_from_file_stdio(FILE *fp_out, const char *source_filename) {
	FILE *fp_in;
	char buffer[BUFFER_SIZE];
	size_t bytes_read;
	fp_in = fopen(source_filename, "r");
	if (fp_in == NULL) {
		perror("Error");
		exit(EXIT_FAILURE);
	}
	while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp_in)) > 0) {
		if (fwrite(buffer, 1, bytes_read, fp_out) != bytes_read) {
			perror("ERROR");
			fclose(fp_in);
			fclose(fp_out);
			exit(EXIT_FAILURE);
		}
	}

	if (ferror(fp_in)) {
		perror("ERROR");
		fclose(fp_in);
		exit(EXIT_FAILURE);
	}

	fclose(fp_in);
}

// (Hàm usage_error giống hệt như phiên bản trước)
void usage_error(const char *prog_name) {
    fprintf(stderr, "Cách dùng:\n");
    fprintf(stderr, "  %s -k <output_file>\n", prog_name);
    fprintf(stderr, "     (Tùy chọn 1: Ghi nội dung từ bàn phím)\n\n");
    fprintf(stderr, "  %s -f <source_file> <output_file>\n", prog_name);
    fprintf(stderr, "     (Tùy chọn 2: Sao chép nội dung từ file)\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int opt;
    int mode = 0; // 0=chưa chọn, 1=keyboard, 2=file
    char *source_filename = NULL;
    char *output_filename = NULL;
    FILE *fp_out;

    // 1. Phân tích các tùy chọn (giống hệt phiên bản syscall)
    while ((opt = getopt(argc, argv, "kf:")) != -1) {
        switch (opt) {
            case 'k':
                if (mode != 0) usage_error(argv[0]);
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

    // 2. Kiểm tra các đối số còn lại (giống hệt phiên bản syscall)
    if (mode == 0) {
        fprintf(stderr, "Lỗi: Bạn phải chọn một chế độ (-k hoặc -f).\n");
        usage_error(argv[0]);
    }
    if (argc - optind != 1) {
        fprintf(stderr, "Lỗi: Sai số lượng đối số.\n");
        usage_error(argv[0]);
    }
    output_filename = argv[optind];

    // 3. Mở file output
    fp_out = fopen(output_filename, "w"); // "w" = write, tự động tạo/xóa file
    if (fp_out == NULL) {
        perror("Lỗi khi tạo file output");
        return EXIT_FAILURE;
    }

    // 4. Thực thi chế độ đã chọn
    if (mode == 1) {
        write_from_keyboard_stdio(fp_out);
    } else { // mode == 2
        copy_from_file_stdio(fp_out, source_filename);
    }

    // 5. Dọn dẹp
    fclose(fp_out);
    printf("File '%s' đã được tạo và ghi thành công.\n", output_filename);
    return EXIT_SUCCESS;
}