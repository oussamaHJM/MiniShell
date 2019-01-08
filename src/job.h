#include "processus.h"
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#ifndef JOB_H
#define JOB_H

/* Structutre du job.  */
typedef struct job
{
  struct job *next;           /* next  job */
  process *first_process;     /* list des processus dans le job on pointon sur le prmier processus (la tete) */
  pid_t pgid;                 /* pID du groupe de processus */
  int stdin, stdout, stderr;  /* les entrés sortie standars d'un job*/
} job;

/* retourn le job active à l'aide du pid du groupe.  */
job * find_job (pid_t pgid);

/* Return true si tous les processus du job sont arreter ou completer à l'aide de la liste */
int job_is_stopped (job *j);


/* Return true si tous les processus dans la liste du job sont completer.  */
int job_is_completed (job *j);



/* sauvegarder les attribut du shell.  */
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

/*lancé un  processus avec un pid du groupe*/

void launch_process (process *p, pid_t pgid,int infile, int outfile, int errfile,int foreground);

/* mettre un job dans l'arrière plan*/

void put_job_in_background (job *j, int cont);
