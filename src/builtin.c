/*
	FICHIER : builtin.c
	Auteur : Oussama EL-HAJJAM -- Meryem ALAOUI
	Dépendances : builtin.h
*/
#include "builtin.h"
#define MAX_PATH_SIZE 1024



int builtin_cd(const char* path){//chdir meme fonction (aller au chemain ) gerrer les cas du home
	/*if((path == NULL) || (strcmp(path, "~") == 0) || (strcmp(path, "~/") == 0) || (strcmp(path, "#") == 0)){
		path = getenv("HOME");
	}
	return chdir(path);*/
	if ((path == NULL)|| (strcmp(path, "~") == 0) || (strcmp(path, "~/") == 0) || (strcmp(path, "#") == 0)) {
		chdir(getenv("HOME")); 
		return 1;
	}
	else{ 
		if (chdir(path) == -1) {
			printf(" %s: est pas un dossier\n", path);
            return -1;
		}
	}
	return 0;
}

int builtin_pwd(char path[MAX_PATH_SIZE]){
	if(getcwd(path, MAX_PATH_SIZE) == NULL){
		printf("On peut pas afficher le repertoire courant");		
		return 1;
	}
	printf("%s\n",path);
	return 0;
}



int builtin_export(char* name, const char* value){//set env
	/*printf("\n%s--%s\n",name, value);
	return setenv(name, value, 1);
	putenv(name);
	return 0;*/
	if(getenv(name) != NULL){
		printf("%s", "La variableest écrasé\n");
	}else{
		printf("%s", "la variable est creé\n");
	}
	// If we specify no value for the variable, we set it to ""
	if (value == NULL){
		return setenv(name, "", 1);
		// We set the variable to the given value
	}else{
		return setenv(name, value, 1);
	}
	return -1;
}

int builtin_unset(const char* name){//
	if(getenv(name) != NULL){
		unsetenv(name);
		printf("%s", "La variable est supprimé d\n");
		return 0;
	}else{
		printf("%s", "La variable nexiste pas\n");
	}
	return -1;
}

int builtin_source(const char* path){//lire les commande dans un fichier comme scanf
	return 0;
}

int builtin_exit(int value){//exit de bibliotheque standard dire au revoir, voir si un processus en cours d'execution
	printf("\n\033[1;34mGlad to see you again\033[0m\n");
	kill(0, SIGKILL);
	kill(0, SIGINT);
	exit(value);
	return 1;
}


int strgetenv(char *str){
	if (str[0] == '$'){
		char *val = getenv(str++);
		if (val == NULL)
			builtin_export(str, "");
		else
			builtin_export(str, val);
		return 0;		
	}
	else
		return 1;
}
