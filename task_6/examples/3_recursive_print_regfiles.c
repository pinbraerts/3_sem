/* Программа рекурсивно распечатывает полные пути к директориям и регулярным файлам, начиная с текущей директории. Данный пример немного отличается от рассмотренного на семинаре, однако смысл один и тот же */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

void SearchDirectory(const char *name) {
    DIR *dir = opendir(name);               //Assuming absolute pathname here.
    if(dir) {
        char Path[PATH_MAX], *EndPtr = Path;
        struct stat info;                //Helps us know about stuff
        struct dirent *e;
        strcpy(Path, name);                  //Copies the current path to the 'Path' variable.
        EndPtr += strlen(name);              //Moves the EndPtr to the ending position.
        while ((e = readdir(dir)) != NULL) {  //Iterates through the entire directory.
            strcpy(EndPtr, e -> d_name);       //Copies the current filename to the end of the path, overwriting it with each loop.
            if (!stat(Path, &info)) {         //stat returns zero on success.
                if (S_ISDIR(info.st_mode)) {  //Are we dealing with a directory?
                    //Make corresponding directory in the target folder here.
                    printf("directory: %s/\n", Path);
                    ;
                    SearchDirectory(Path);   //Calls this function AGAIN, this time with the sub-name.
                } else if (S_ISREG(info.st_mode)) { //Or did we find a regular file?
                    ;
                    //Run Copy routine
                    printf("reg_file: %s\n", Path);
                }
            }
        }
    }
    return;
}

int main() {
    printf("Hello, World!\n");
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        SearchDirectory(strcat(cwd, "/"));
    }
    return 0;
}
