//
// .
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#ifndef PROCESSUS_H
#define PROCESSUS_H


#define MAX_PATH_SIZE 1024
#define MAX_ARGS 128
#define MAX_ENVS 256

typedef struct processus_t{
	char	path[MAX_PATH_SIZE];
	char*	argv[MAX_ARGS];//tableau d'argument
	char*	envp[MAX_ENVS];//tableau de variables d'environement
	pid_t	pid;
	int	status;
	int	stdin, stdout, stderr;
	int	background;
	struct	processus_t *next;
	struct	processus_t *next_success;
	struct	processus_t *next_failure;
	struct	processus_t *next_pipe;
}processus_t;

int search_path(processus_t *p);
void signal_handler(int sign);
void processus_signal_handler(int sign);
int exec_process(processus_t* p);
int exec_cmdline(processus_t * process);
int my_pipe(processus_t *cmd_array, int size);
char *my_prompt(void);
#endif //PROCESSUS_H
