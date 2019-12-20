#ifndef FILES_H
#define FILES_H

#include "includes.h"

int is_text_file(const char* path) {
    int pipefd[2];
    pid_t pid;

    CHECK(pipe(pipefd));
    CHECK(pid = fork());

    if(pid == 0) {
        CHECK(close(pipefd[0]));

        CHECK(dup2(pipefd[1], STDOUT_FILENO));
        CHECK(dup2(pipefd[1], STDERR_FILENO));

        CHECK(close(pipefd[1]));

        CHECK(execlp("file", "file", "-br", "--mime-type", path, NULL));
    }
    else {
        CHECK(close(pipefd[1]));

        char buffer[5];
        char* ptr = buffer;
        ssize_t n;
        size_t m = sizeof(buffer);
        while((n = read(pipefd[0], ptr, m) > 0) && n > m) {
            m -= n;
            ptr += n;
        }
        CHECK(n);
        CHECK(close(pipefd[0]));

        int status;
        waitpid(pid, &status, 0);

        buffer[4] = '\0';
        
        return strcmp(buffer, "text") == 0;
    }
    return 0;
}

// int check_for_tar(char* path) {
//     int pipefd[2];
//     pid_t pid;

//     CHECK(pipe(pipefd));
//     CHECK(pid = fork());

//     if(pid == 0) {
//         CHECK(close(pipefd[0]));

//         CHECK(dup2(pipefd[1], STDOUT_FILENO));
//         CHECK(dup2(pipefd[1], STDERR_FILENO));

//         CHECK(close(pipefd[1]));

//         CHECK(execlp("which", "which", "tar", NULL));
//     }
//     else {
//         CHECK(close(pipefd[1]));

//         char* ptr = path;
//         ssize_t n;
//         size_t m = PATH_MAX;
//         while((n = read(pipefd[0], ptr, m) > 0) && n > m) {
//             m -= n;
//             ptr += n;
//         }
//         CHECK(n);
//         CHECK(close(pipefd[0]));

//         ptr = strchr(path, ' ');
//         ptr = '\0';

//         int status;
//         waitpid(pid, &status, 0);

//         if(WIFEXITED(status))
//             return WEXITSTATUS(status);
//     }
//     return 0;
// }

int copy_file(const char* dest, const char* source) {
    int fd1, fd2;

    CHECK(fd1 = open(dest, O_CREAT | O_WRONLY));
    CHECK(fd2 = open(source, O_RDONLY));

    ssize_t n;
    while((n = sendfile(fd1, fd2, 0, 1024)) > 0);
    CHECK(n);

    CHECK(close(fd1));
    CHECK(close(fd2));

    return 0;
}

// int copy_files(const char* dirname, const char* backup_path) {
//     DIR** dirs = (DIR**)malloc(sizeof(DIR*) * MAX_DIRS);
//     path_t name, backup;

//     path_init(name, dirname);
//     path_init(backup, backup_path);

//     int fallback = 1;
//     int i = 0;
//     while(i < MAX_DIRS && i >= 0) {
//         if(fallback) {
//             CHECK_NULL(dirs[i] = opendir(name));
//             CHECK(mkdir(backup, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
//         }
//         fallback = 0;

//         struct dirent* e;
//         while((e = readdir(dirs[i])) != NULL) {
//             if(strcmp(e->d_name, ".") == 0
//                 || strcmp(e->d_name, "..") == 0)
//                 continue;

//             path_join(name, e->d_name);
//             path_join(backup, e->d_name);

//             struct stat s;
//             CHECK(stat(name, &s));
//             if(S_ISDIR(s.st_mode)) {
//                 ++i;
//                 fallback = 1;
//                 break;
//             }
//             else if(S_ISREG(s.st_mode) && is_text_file(name)) {
//                 CHECK(copy_file(name, backup));
//             }

//             path_go_up(name);
//             path_go_up(backup);
//         }

//         if(!fallback) {
//             CHECK(closedir(dirs[i]));
//             --i;
//             path_go_up(name);
//             path_go_up(backup);
//         }
//     }

//     return 0;
// }

int compress(const char* backup, const char* destination) {
    pid_t pid;
    CHECK(pid = fork());

    if(pid == 0) {
        CHECK(close(STDOUT_FILENO));
        CHECK(close(STDERR_FILENO));

        execlp("tar", "tar", "-czf", destination, "--remove-files", backup, NULL);
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return EXIT_FAILURE;
    }

    return 0;
}

int decompress(const char* backup, const char* destination) {
    pid_t pid;
    CHECK(pid = fork());

    if(pid == 0) {
        CHECK(close(STDOUT_FILENO));
        CHECK(close(STDERR_FILENO));

        execlp("tar", "tar", "-xzf", backup, "-C", destination, NULL);
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return EXIT_FAILURE;
    }

    return 0;
}

int read_all(int fd, void* buf, size_t sz) {
    size_t m = sz;
    ssize_t n;
    char* b = (char*)buf;
    while((n = read(fd, b, m)) > 0 && n < m) {
        m -= n;
        b += n;
    }
    return n;
}

int write_all(int fd, const void* buf, size_t sz) {
    size_t m = sz;
    ssize_t n;
    const char* b = (const char*)buf;
    while((n = write(fd, b, m)) > 0 && n < m) {
        m -= n;
        b += n;
    }
    return n;
}

#endif // !FILES_H
