#include "includes.h"

int main(void) {
    printf("My pid: %d\n", getpid());

    char buffer[256];
    int ret;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGTERM);
    sigprocmask(SIG_BLOCK, &set, NULL);

    siginfo_t info;

    while((ret = sigwaitinfo(&set, &info) >= 0)) {
        if(info.si_signo == SIGUSR1) {
            int r = info.si_value.sival_int;
            int m = r / 256;
            printf("%d %d %c\n", r, m, (char)r);
            buffer[m] = (char)r;
        }
        else if (info.si_signo == SIGTERM) {
            puts(buffer);
            break;
        }
    }

    return 0;
}
