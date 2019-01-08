#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define MAX_PATH_SIZE 1024



int builtin_cd(const char* path);
int builtin_pwd(char path[MAX_PATH_SIZE]);
int builtin_export(char* name, const char* value);
int builtin_unset(const char* name);
int builtin_source(const char* path);
int builtin_exit(int value);
int strgetenv(char *str);
