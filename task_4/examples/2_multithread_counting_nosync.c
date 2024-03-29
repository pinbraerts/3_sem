#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_LOOPS 1000000

long long sum = 0;

void *cnt_routine(void *args) {
	int i;
	int offset = *(int*) args;
	for (i = 0; i < NUM_LOOPS; i++) {
		sum +=offset;
	}
	pthread_exit(NULL);
}

int main() {
	pthread_t id1,id2;
	int offset1=1;
	int offset2=-1;
	pthread_create(&id1, NULL, cnt_routine, &offset1);
	pthread_create(&id2, NULL, cnt_routine, &offset2);
	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	printf("Result of counting: %lld\n",sum );
	return 0;
}
