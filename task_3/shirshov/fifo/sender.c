#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>

#define NAME "/tmp/fifo.fifo"
#define SIZE 256

int main(int argc, char** argv) {
    int output_fd = open(NAME, O_WRONLY);
    if(output_fd == -1) {
        perror("fifo open() ");
        return -1;
    }

    int input_fd = open(argv[1], O_RDONLY);
    if(output_fd == -1) {
        close(output_fd);
        perror("open() ");
        return -1;
    }

    while(sendfile(output_fd, input_fd, 0, SIZE) > 0);

    close(input_fd);
    close(output_fd);

    return 0;
}
