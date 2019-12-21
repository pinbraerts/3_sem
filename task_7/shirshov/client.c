#include "includes.h"
#include "api.h"

int main(int argc, char** argv) {
    int cmd_fifo;
    CHECK(cmd_fifo = open(argv[1], O_WRONLY));

    char* tx_name = argv[2];
    char* fx_name = argv[3];

    puts("mkfifo");
    CHECK(mkfifo(tx_name, 0666));
    CHECK(mkfifo(fx_name, 0666));
    
    my_msg_t msg;
    msg.size1 = strlen(tx_name) + 1;
    msg.size2 = strlen(fx_name) + 1;

    CHECK(write(cmd_fifo, &msg, sizeof(my_msg_t)));
    CHECK(write(cmd_fifo, tx_name, msg.size1));
    CHECK(write(cmd_fifo, fx_name, msg.size2));
    puts("sent request");

    int tx;
    CHECK(tx = open(tx_name, O_WRONLY));
    puts("opened tx");

    char c;
    int fx;
    CHECK(fx = open(fx_name, O_RDONLY));
    puts("opened fx");
    CHECK(close(cmd_fifo));


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
