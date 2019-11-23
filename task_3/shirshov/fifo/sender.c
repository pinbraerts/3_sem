#include <sys/sendfile.h>

#include "../utils.h"

int main(int argc, char** argv) {
    int output_fd;
    CHECK(output_fd = open(NAME, O_WRONLY));

    int input_fd;
    // CHECK(input_fd = open(argv[1], O_RDONLY));

    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("file = fopen(argv[1], \"r\")");
        return 1;
    }
    fseek(input, 0, SEEK_END);
    size_t n = ftell(input);
    ssize_t m;
    rewind(input);

    input_fd = input->_fileno;

    CHECK(m = write(output_fd, &n, sizeof(size_t)));

    while((m = sendfile(output_fd, input_fd, 0, n) > 0) && n > m)
        n -= m;

    CHECK(m);
    CHECK(fclose(input));
    CHECK(close(output_fd));

    return 0;
}
