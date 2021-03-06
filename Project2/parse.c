//Curtis Duvall, Nathan Wilkins, and Julian Thayer
//Systems and Networking II
//Project 2
//parse.c

#include "parse.h"
#include "Definitions.h"

void parse_message(char buffer[], int * mode, char username[], char password[], char destination[], char body[])
{
	clear_string(username, CREDENTIAL_SIZE);
	clear_string(password, CREDENTIAL_SIZE);
	clear_string(destination, CREDENTIAL_SIZE);
	clear_string(body, BUFFER_SIZE);

	char * search = malloc(2*sizeof(char));
	char * token;
	char temp[BUFFER_SIZE];
	clear_string(temp, BUFFER_SIZE);
	strcpy(temp, buffer);
	search[1]=0;
	search[0]=DELIMITER;
	token = strtok(temp, search);
	*mode = atoi(token);
	token = strtok(NULL, search);
	strcpy(username, token);
	token = strtok(NULL, search);
	strcpy(password, token);
	token = strtok(NULL, search);
	strcpy(destination, token);
	token = strtok(NULL, search);
	strcpy(body, token);
	free(search);
	return;
}

void clear_string(char * string, int max_Size) {
	int i;
	for (i=0;i<max_Size;i++)
		string[i]=0;
	return;
}
