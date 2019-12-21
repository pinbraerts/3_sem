#include "includes.h"
#include "api.h"

int main(int argc, char** argv) {
    CHECK(mkfifo(argv[1], 0666));

    printf("FIFO names: %s\n", argv[1]);

    int input_fd;
    CHECK(input_fd = open(argv[1], O_RDWR | O_NONBLOCK));

    struct timeval tv;
    tv.tv_usec = 0;
    tv.tv_sec = 5;

    fd_set fds;
    FD_ZERO(&fds);
    // FD_SET(input_fd, &fds);

    Vector sessions; // Vector<my_session>
    Vector_init(sessions);

    for(;;) {
        CHECK(select(FD_SETSIZE, &fds, NULL, NULL, &tv));

        if(FD_ISSET(input_fd, &fds)) {
            my_request_t req;
            CHECK(read(input_fd, &req, sizeof(req)));

            size_t sz = req.size1 + req.size2;

            char* buf = (char*)malloc(sz);
            char* ptr = buf;
            size_t n = sz;
            ssize_t m;
            while((m = read(input_fd, ptr, n)) != 0 && m < n) {
                if(errno == EAGAIN) {
                    continue;
                }
                n -= m;
                ptr += m;
            }

            printf("CONNECT %s %s\n", buf, buf + req.size1);

            int fd1, fd2;

            CHECK(fd2 = open(buf + req.size1, O_WRONLY));
            CHECK(fd1 = open(buf, O_RDWR | O_NONBLOCK));
            
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
            puts("polling");
            if(FD_ISSET(s->tx, &fds)) { // read is non-blocking
                char buf[1024];
                ssize_t m = read(s->tx, buf, sizeof(buf));
                if(m > 0) {
                    printf("REQUEST %s\n", buf);
                    int file = open(buf, O_RDONLY);
                    struct stat st;
                    CHECK(fstat(file, &st));
                    sendfile(s->fx, file, 0, st.st_size);
                }
                else if(m == 0) { // close this client
                    puts("DETACH");
                    close(s->tx);
                    close(s->fx);
                    *Vector_back_new(int, to_remove) = s - Vector_data(my_session_t, sessions);
                }
                else {
                    perror("read(i, buf, sizeof(buf)");
                    exit(EXIT_FAILURE);
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

    return 0;
}
