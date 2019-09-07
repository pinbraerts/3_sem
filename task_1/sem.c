#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <wait.h>
#include <unistd.h>

int main(void) {
	size_t size;
	char* buf = 0;
	int res = getline(&buf, &size, stdin);
	if(res <= 0) {
		puts("Error!");
		return res;
	}

	char* argv[10];

	unsigned n = 0, m = 0;
	argv[0] = buf;
	++m;
	for (; m < 10; ++m) {
		for (; !isspace(buf[n]) && buf[n] != '\0' && n < size; ++n); // skip word
		if(!isspace(buf[n])) break;
		buf[n] = '\0'; // add delimiter for args
		++n;
		for (; isspace(buf[n]) && n < size; ++n); // skip trailing spaces
		if(buf[n] == '\0' || n >= size) break;
		argv[m] = buf + n;
	}
	buf[n] = '\0';
	argv[m] = 0;


	unsigned i;
	for(i = 0; i < m; ++i)
		printf("'%s'\n", argv[i]);


	int pipefd[2];
	pid_t pid;

	if(pipe(pipefd) == -1) {
		puts("Error!");
		return 1;
	}

	pid = fork();

	if(pid == -1) {
		puts("Error");
		return 2;
	}

	if(pid == 0) { // child process
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
		execvp(buf, argv);
	} else {
		close(pipefd[1]);
		wait(0);
		char output[256];
		int n;
		while((n = read(pipefd[0], output, sizeof(output) - 1)) > 0) {
			output[n] = '\0';
			printf("%s", output);
		}
		puts("");
	}

	return res;
}
