#include "includes.h"
#include "api.h"

int main(int argc, char** argv) {
    int fd1;
    CHECK(fd1 = open(argv[1], O_WRONLY | O_NONBLOCK));
    
    char* tx_name = argv[2];
    char* fx_name = argv[3];

    puts("mkfifo");
    CHECK(mkfifo(tx_name, 0666));
    CHECK(mkfifo(fx_name, 0666));
    
    my_request_t req;
    req.size1 = strlen(tx_name) + 1;
    req.size2 = strlen(fx_name) + 1;

    puts("request");
    CHECK(write(fd1, &req, sizeof(req)));
    CHECK(write(fd1, tx_name, req.size1));
    CHECK(write(fd1, fx_name, req.size2));
    CHECK(close(fd1));

    puts("open fx");
    int fx;
    CHECK(fx = open(fx_name, O_RDONLY));

    puts("open fifo");
    int tx;
    CHECK(tx = open(tx_name, O_WRONLY | O_NONBLOCK));

    for(int i = 4; i < argc; ++i) {
        puts("sending");
        CHECK(write(tx, argv[i], strlen(argv[i]) + 1));
        
        char buf[1024];
        ssize_t m;
        CHECK(m = read(fx, buf, sizeof(buf)));
        buf[m] = '\0';

        puts(buf);
    }

    CHECK(close(tx));
    CHECK(close(fx));

    CHECK(remove(tx_name));
    CHECK(remove(fx_name));

    return 0;
}
