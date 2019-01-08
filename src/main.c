/*
	FICHIER : main.c
	Auteur : Oussama EL-HAJJAM -- Meryem ALAOUI
	Dépendances : builtin.h processus.h
*/

#include "processus.h"
#include "parser.h"
#define MAX_CMD_SIZE 255 // nombre maximum des commandes dans une ligne
#define MAX_STR_SIZE 1024

//Install if not installed !!
//sudo apt-get install libreadline6 libreadline6-dev


int main(int argc, char **argv){
	char *str;//[MAX_STR_SIZE]faut ajouter dans parser .h max str size
	processus_t cmd[MAX_CMD_SIZE];//defirir
	printf ("\n-----------------------G2 MINISHELL------------------------\n");
	printf ("\n----------Oussama EL-hajjam----Meryem Alaoui---------------\n");
	int not_print_prompt = 0;
	
	while(1){
		signal(SIGINT, signal_handler);// compturer le signal d'interruption
		
		//fgets(str, MAX_STR_SIZE, stdin);
		if(not_print_prompt == 0){
			str = readline(my_prompt()); // pour gerer l'autocomplete et le deplacement avec lecurseur à l'aide de la bibliotheque readline.h
		}
		not_print_prompt = 0;// la varible est utiliser pour ne pas afficher la prompte plus que deux fois
		if(strlen(str)>0) { add_history(str); }// ajouter lacommande dans l'historique
		//str[strlen(str)-1] = '\0';
		
		
		trim(str);
		clean(str);
		char *tokens[MAX_STR_SIZE/2+1];
		split_str(str, tokens, MAX_STR_SIZE/2+1);
		
		char **current = tokens;
		
		int n_cmd = 0;
		int next = 0;
		
		while(next != -1){
			separator_t sep;
			next = split_command(current, &sep);
			redirection_t redir;
			int path_idx = redirection(current, &redir);
			
			switch(redir){
				case OUTPUT_APPEND: //cmd >> file
					cmd[n_cmd].stdout = open(current[path_idx], O_CREAT | O_APPEND | O_WRONLY, 0660);
					cmd[n_cmd].stdin = 0;
					cmd[n_cmd].stderr = 2;
				break;
				
				case OUTPUT : //cmd > file
					cmd[n_cmd].stdout = open(current[path_idx], O_CREAT | O_WRONLY, 0660);
					cmd[n_cmd].stdin = 0;
					cmd[n_cmd].stderr = 2;
				break;
                case ERROR : //cmd $> file
					cmd[n_cmd].stdout = 1;
					cmd[n_cmd].stdin = 0;
					cmd[n_cmd].stderr = open(current[path_idx], O_CREAT | O_WRONLY, 0660);
				break;
				
				case ERROR_APPEND :	//cmd 2>> file
				    cmd[n_cmd].stdin = 0;
					cmd[n_cmd].stdout = 1;
					cmd[n_cmd].stderr = open(current[path_idx], O_CREAT | O_APPEND | O_WRONLY, 0660);
				break;
				
				default : 
					cmd[n_cmd].stdout = 1;
					cmd[n_cmd].stdin = 0;
					cmd[n_cmd].stderr = 2;
				break;
				/*case OUTPUT_....:// cmd > file
					
				break;*/
			}
			strcpy(cmd[n_cmd].path, *current);
			
			int i = 0;
			for (char **s = current; *s != NULL; ++s){
				cmd[n_cmd].argv[i++] = *s;
			}
			cmd[n_cmd].argv[i] = NULL;
			switch(sep) {
				case AND: //cmd && cmd2
					cmd[n_cmd].next = NULL;
					cmd[n_cmd].next_success = &cmd[n_cmd+1];
					cmd[n_cmd].next_failure = NULL;
					cmd[n_cmd].background = 0;
				break;

				case OR: //cmd || cmd2
					cmd[n_cmd].next = &cmd[n_cmd+1];
					cmd[n_cmd].next_success = NULL;
					cmd[n_cmd].next_failure = NULL;
					cmd[n_cmd].background = 0;
				break;
				
				case PIPE : //cmd | cmd2
					printf("\n -------- PIPE ****************\n");
					/*cmd[n_cmd].next = NULL;
					cmd[n_cmd].next_success = NULL;
					cmd[n_cmd].next_failure = NULL;
					cmd[n_cmd].next_pipe = &cmd[n_cmd+1];
					cmd[n_cmd].background = 0;*/
					my_pipe(cmd, argc+1);// appeler la fonction my_pipe en passons comme parametre le tableaux des commandes et le nombre des commandes à execter
					n_cmd++;
				break;
				
				case PARAL : 
					cmd[n_cmd].next =  &cmd[n_cmd+1];
					cmd[n_cmd].next_success =NULL;
					cmd[n_cmd].next_failure = NULL;
					cmd[n_cmd].background = 1;
				break;
				
				case END : 
					cmd[n_cmd].next =  NULL;
					cmd[n_cmd].next_success =NULL;
					cmd[n_cmd].next_failure = NULL;
					cmd[n_cmd].background = 0;
				break;
				
				case SEMICOL : //cmd ; cmd2
					cmd[n_cmd].next = NULL;
					cmd[n_cmd].next_success = NULL;
					cmd[n_cmd].next_failure = &cmd[n_cmd+1];
					cmd[n_cmd].background = 0;
				break;
			}
			
			search_path(&cmd[n_cmd]);
			current += next;
			++n_cmd;
		}
		
		exec_cmdline(cmd);
		memset(tokens, 0, sizeof(tokens));
		free(str);
		str = NULL;
		//printf("%s",str);
	}
	
	return 0;
}
