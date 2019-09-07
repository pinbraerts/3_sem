#include <stdio.h>
#include "gossip.h"

void check(int x) {
    if(x == 0) return;

    fputs("Error!", stderr);
    exit(x);
}

int main(void) {
    Gossip g;
    check(gossip_init(&g));

    pid_t pid = fork();
    
    if(pid == 0) {
        int y;
        printf("Enter value: ");
        scanf("%d", &y);
        check(gossip_write(&g, &y, sizeof(y)));

        check(gossip_read(&g, &y, sizeof(y)));
        printf("Read value (in process %d): %d\n", pid, y);
    }
    else {
        int x;
        check(gossip_read(&g, &x, sizeof(x)));
        printf("Read value (in process %d): %d\n", pid, x);

        printf("Enter value: ");
        scanf("%d", &x);
        check(gossip_write(&g, &x, sizeof(x)));
    }

    check(gossip_free(&g));

    return 0;
}
