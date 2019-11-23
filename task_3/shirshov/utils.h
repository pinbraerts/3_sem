#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define CHECK(...) do { if(( __VA_ARGS__ ) == -1) { perror(__FILE__ " " #__VA_ARGS__ " "); return -1; } } while(0)
#define NAME "/tmp/fifo.fifo"
#define SIZE 16400
