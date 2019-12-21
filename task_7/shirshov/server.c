#include "includes.h"
#include "api.h"

int main(int argc, char** argv) {
    int cmd_fifo;

    CHECK(mkfifo(argv[1], 0666 | IPC_CREAT));
    CHECK(cmd_fifo = open(argv[1], O_RDWR | O_NONBLOCK));

    struct timeval tv;
    tv.tv_usec = 0;
    tv.tv_sec = 100;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(cmd_fifo, &fds);

    Vector sessions; // Vector<my_session>
    Vector_init(sessions);

    for(;;) {
        puts("poll");
        int r = select(FD_SETSIZE, &fds, NULL, NULL, &tv);
        CHECK(r);
        if(r == 0) {
            // puts("no");
            continue;
        }

        if(FD_ISSET(cmd_fifo, &fds)) {
            my_msg_t msg;

            CHECK(read(cmd_fifo, &msg, sizeof(my_msg_t)));

            size_t n = msg.size1 + msg.size2;
            ssize_t m;
            char* buf = (char*)malloc(n);
            char* ptr = buf;

            int flags = fcntl(cmd_fifo, F_GETFL, 0);
            CHECK(fcntl(cmd_fifo, F_SETFL, flags & ~O_NONBLOCK));
            while((m = read(cmd_fifo, ptr, n)) > 0 && m < n) {
                n -= m;
                ptr += m;
            }

            printf("CONNECT %s %s\n", buf, buf + msg.size1);

            int fd1, fd2;
            
            CHECK(fd1 = open(buf, O_RDWR | O_NONBLOCK));
            puts("opened fd1");

            char c = 1;

            CHECK(fd2 = open(buf + msg.size1, O_WRONLY));
            puts("opened fd2");
            
            close(cmd_fifo);
            CHECK(cmd_fifo = open(argv[1], O_RDWR | O_NONBLOCK));
            FD_SET(cmd_fifo, &fds);

            FD_SET(fd1, &fds);

            // CHECK(fcntl(cmd_fifo, F_SETFL, flags | O_NONBLOCK));
            
            my_session_t* s = Vector_back_new(my_session_t, sessions);
            s->tx = fd1;
            s->fx = fd2;
        }

        // for(int i = 0; i < FD_SETSIZE; ++i) {
        //     if(FD_ISSET(i, &fds)) {
        //         printf("%d ", i);
        //     }
        // }
        // puts("");

        Vector to_remove;
        Vector_init(to_remove);
        
        Vector_foreach(my_session_t, sessions, s) {
            if(FD_ISSET(s->tx, &fds)) {
                for(;;) {
                    char buf[1024];
                    ssize_t m = read(s->tx, buf, sizeof(buf));
                    if(m > 0) {
                        printf("REQUEST %s\n", buf);
                        int file = open(buf, O_RDONLY);
                        struct stat st;
                        CHECK(fstat(file, &st));
                        sendfile(s->fx, file, 0, st.st_size);
                        break;
                    }
                    else if(m == 0) { // close this client
                        puts("DETACH");
                        close(s->tx);
                        close(s->fx);
                        *Vector_back_new(int, to_remove) = s - Vector_data(my_session_t, sessions);
                        break;
                    }
                    else if(errno == EAGAIN)
                        continue;
                    else {
                        perror("read(i, buf, sizeof(buf)");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }

        Vector_foreach(int, to_remove, i) {
            puts("remove");
            my_session_t* s = Vector_data(my_session_t, sessions) + *i;
            *s = Vector_data(my_session_t, sessions)[sessions.size - 1];
        }
        Vector_resize(my_session_t, sessions, sessions.size - to_remove.size);
    }
    CHECK(close(cmd_fifo));

    return 0;
}
