/*
	FICHIER : processus.c
	Auteur : Oussama EL-HAJJAM -- Meryem ALAOUI
	Dépendances : builtin.h processus.h
*/


#include "processus.h"
#include "builtin.h"


/*
	Fonction stradd 
	Paramètre a : chaine de characteres ... 
	Paramètre b : chaine de charactères à ajouter à la fin de la chaine b... 
	Retourne une chaine de charactères, c'est la concatènation de la cahaine b avec la chaine a...
*/

char* stradd(const char* a, const char* b){
    size_t len = strlen(a) + strlen(b);
    char *ret = (char*)malloc(len * sizeof(char) + 1);
    *ret = '\0';
    return strcat(strcat(ret, a) ,b);
}


/*
	Fonction processus_signal_handler 
	Paramètre sig : entier c'est le signale pour stopper l'execution (CTRL+C) ...
	La fonction permet de stopper l'execution d'un processus et appele la fonction processus signal handler pour afficher laprompt et puis retourner à laligne...
*/

void processus_signal_handler(int sig){
	if(sig == SIGINT){
		printf("\n");
		signal(SIGINT, processus_signal_handler);
	}
}

/*
	Fonction signal_handler 
	Paramètre sig : entier qui represente le signale pour stopper l'execution (CTRL+C) ...
	La fonction permet de retourner à la ligne dans leprogramme principale.
	par exemple si vous tapper une commande erroné et que vous voulez pas l'executer et retourner à une ligne vide()...
*/

void signal_handler(int sig){
	if(sig == SIGINT){
		printf("\n");
		signal(SIGINT, signal_handler);
		printf("%s",my_prompt());
	}
}


/*
	Fonction my_prompt 
	Paramètre: pas de parametre...
	La fonction et utiliser pour afficher la prompte du programme avec des couleur suivi du chemain courant à l'aide de la fonction stradd()...
*/

char * my_prompt(void){
	char	*cwd;
	char	buff[4096 + 1];
	char	rtrn;

	cwd = getcwd(buff, 4096);
	cwd = stradd("\033[1;32m",cwd);
	cwd = stradd(cwd, ": \033[0m");
	return stradd("\033[0;31m\nG2@Minishell:D\033[0m",cwd);//on peut remplacé tous simplement le G2 avec le nom d'utilisateur à partie du variabled'environement USER avec getenv
}

/*
	Fonction exec_process 
	Paramètre p: le processus à executer.
	La fonction et utiliser pour executer le processus passé en parametre.
	Remearque : nous avons eu des problèmes durant l'execution des commande avec execve, c'est pour cela nous avons utilisé execv().
*/

int exec_process(processus_t* p){
	
	if (strcmp(p->path, "cd") == 0) {
        builtin_cd(p->argv[1]);
        return 0;
    }else if (strcmp(p->path,"unset")==0){
		builtin_unset(p->argv[1]);
	}else if (strcmp(p->path, "pwd") == 0){
		builtin_pwd(p->path);
	}else if (strcmp(p->path, "exit") == 0){
		builtin_exit(1);
	}else if (strcmp(p->path, "export") == 0){
		builtin_export(p->argv[1],p->argv[2]);
	}
	else{
		p->pid = fork();
		if (p->pid == 0){
		    dup2(p->stdin, 0);
		    dup2(p->stdout, 1);
		    dup2(p->stderr, 2);
		    if (p->stdin != 0) close (p ->stdin);
		    if (p->stdout != 1) close (p ->stdout);
		    if (p->stderr!= 2) close (p ->stderr);
		    //execve(p->path, p->argv, p->envp);
		    signal(SIGINT, processus_signal_handler);
		    if(!execv(p->path, p->argv))
		    	printf("commande not found");
		    return -1;
		}
		if(p->background == 0){
			waitpid(p->pid, NULL, 0);
		}
	}
    return 0;
}

/*
	Fonction search_path 
	Paramètre p: le processus quand va chercher sans chemain d'execution.
	La fonction et utiliser pour recuperer le chamain de la commande, si elle est built_in on va pas chercher son chemain si non on cherche si le chemain ne trouve pas la commande est inconnue.
*/

int search_path(processus_t *p){
	if ((strcmp(p->argv[0], "cd")!=0) && (strcmp(p->argv[0],"unset") != 0) && (strcmp(p->argv[0], "pwd") != 0) && (strcmp(p->argv[0], "exit") != 0) && (strcmp(p->argv[0], "export") != 0) )  {
		char * env_path = getenv("PATH");
		char * tmp;
		if((tmp = strchr(p->argv[0],'/'))!=NULL){
		    if (tmp == p->argv[0]){
		        strcpy(p->path,p->argv[0]);
		    }
		    else{
		        char buffer[MAX_PATH_SIZE];
		        strcpy(p->path, getcwd(buffer,MAX_PATH_SIZE));
		        strcat(p->path,"/");
		        strcat(p->path, p->argv[0]);
		    }
		}
		else{
		    char path[MAX_PATH_SIZE];
		    strcpy(path, env_path);
		    tmp = strtok(path, ":");
		    while(tmp != NULL){
		        strcpy(p->path, tmp);
		        strcat(p->path, "/");
		        strcat(p->path, p->argv[0]);
		        FILE * f = fopen(p->path, "r");//lire seulement pour recuperer le chemain specifiue dela commande
		        if (f != NULL){
		            fclose(f);
		            //printf("%s\n",p->path);
		            return 0;
		        }
		        tmp = strtok(NULL, ":");
		    }
		    printf("\nCommande inconnue\n");
		}
		
	}
}

/*
	Fonction my_pipe 
	Paramètre cmd_array: tableau des processus à éxecuter.
	Paramètre size: int qui represente la taille du tableu des commandes (nombre des commande à executer comme avec les redirection pipe).
	La fonction permet de gérrer les redirection des resulats des commandes ç l'aides des deux tube.
*/

int my_pipe(processus_t *cmd_array, int size){
	int i;
    int p1[2];   // tube du processus père
    int p2[2];   // tube du processus fils
	
    if (pipe(p1))
        printf("minishell: erroor");

   
    execv(cmd_array[0].path, cmd_array[0].argv);
    printf("%d\n", size);
    close(p1[1]);

    for (i = 1; i < size - 1; i++) {
        //lire à partir du tube du processus père, ecrire pour le fils 
        // Fermer letube du quelle on lisent, et celui de l'écriture
        if (i % 2) 
        {
            if (pipe(p2))
                 printf("minishell: erroor");

            execv(cmd_array[i].path, cmd_array[i].argv);
            close(p1[0]);
            close(p2[1]);
        } 
        else 
        {
            if (pipe(p1))
                 printf("minishell: erroor");

            execv(cmd_array[i].path, cmd_array[i].argv);
            close(p2[0]);
            close(p1[1]);
        }
    }

    /* Terminer à stdout */
    if (i % 2) 
    {
        execv(cmd_array[i].path, cmd_array[i].argv);
        close(p1[0]);
    } 
    else 
    {
        execv(cmd_array[i].path, cmd_array[i].argv);
        close(p2[0]);
    }

	return 0;
}

/*
	Fonction exec_cmdline 
	Paramètre p: procesus à éxecuter.
	La fonction permet de gérrer et positionner next et les autres et executer les commande à l'aide de la fonction exec_process.
*/

int exec_cmdline(processus_t * p){

	while (p != NULL){
		exec_process(p);
		if (p->background == 0){// processus en arière plan
			waitpid(p->pid, NULL, 0);
			
		}
		if (p->next != NULL){// cmd 1 ; cmd 2
			p = p->next;
		}
		if (p->next_success != NULL){ //cmd1 && cmd2
			if(WEXITSTATUS(p->status) != 0 ) break;
			p = p->next_success;
			continue;
		}
		if (p->next_failure != NULL){ //cmd1 || cmd2
			if(WEXITSTATUS(p->status) != 0 ){
				break;
			}
			printf("\ncommande ; commande\n");
			p = p->next_failure;
			continue;
		}
		if (p->next != NULL){ //cmd1 | cmd2
			if(WEXITSTATUS(p->status) != 0 ){
				break;
			}
			printf("\n--------PIPE-----------\n");
			printf("\ncommande | commande\n");
			//my_pipe(p,p->next);
			p = p->next;
			continue;
		}
		p = p->next;
	}
    return 0;
}
