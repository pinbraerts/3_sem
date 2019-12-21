#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <sys/signal.h>
#include <sys/select.h>
#include <pthread.h>

#include <linux/limits.h>

#include "vector.h"

#define CHECK(...) do { if((__VA_ARGS__) < 0) { printf(__FILE__ " (line %d): " #__VA_ARGS__ ": %s\n", __LINE__, strerror(errno)); return errno; } } while(0)
#define CHECK_NULL(...) do { if((__VA_ARGS__) == NULL) { printf(__FILE__ " (line %d): " #__VA_ARGS__ ": %s\n", __LINE__, strerror(errno)); return errno; } } while(0)

#endif // !INCLUDES_H
