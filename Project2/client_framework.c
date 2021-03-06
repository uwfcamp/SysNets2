//Curtis Duvall, Nathan Wilkins, and Julian Thayer
//Systems and Networking II
//Project 2
//client_framework.c

#include "Definitions.h"
#include "parse.h"
#include "c_menu_funct.h"
#include "c_login_funct.h"
#include "c_admin_funct.h"
#include "client_framework.h"
#define IP_ADDRESS "127.0.0.1"


struct sockaddr_in serv_addr;
server_t *server;


int main(int argc, char const *argv[])
{
    server = build_server_structure();
    int quit = -1;
    //initialize mutex lock
    if(pthread_mutex_init(&server->lock, NULL) !=0)
	fprintf(stderr, "lock init failed\n");
    //initialize semaphore
    sem_init(&server->mutex, 0, 1);
    sem_wait(&server->mutex);
    if ((server->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr,"ERROR: socket creation error\n");
        server->connected=0;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, IP_ADDRESS, &serv_addr.sin_addr)<=0)
    {
        fprintf(stderr,"ERROR: invalid address and/or address not supported\n");
        server->connected=0;
    }

    if (connect(server->socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "ERROR: connection failed\n");
        server->connected=0;
    }

    pthread_t tid;

    if (pthread_create(&tid, NULL, server_communication, (void *)server)){
	fprintf(stderr,"ERROR: could not create thread\n");
	server->connected=0;
    }

    while(server->connected && quit != 0){
	if(!server->logged_in)
		quit = login_menu(server);
	else
		main_menu(server);
    }

    pthread_join(tid, NULL);
    disconnect(server);
    sem_destroy(&server->mutex);
    pthread_mutex_destroy(&server->lock);
    return 0;
}



// This function will check for a message that
// needs to be sent and a message that is
// being recieved.
void *server_communication(void *vargp){
	// send message to server
	struct timeval *current_time = (struct timeval *)malloc(sizeof(struct timeval));
	gettimeofday(current_time, NULL);
	server_t *server = (server_t *)vargp;
	char pingMessage[CREDENTIAL_SIZE];
	sprintf(pingMessage,"20%c %c %c %c ", (char)DELIMITER, (char)DELIMITER, (char)DELIMITER, (char)DELIMITER);

	while(server->connected){

		//send message to server
		if (server->send==1){
			if (send(server->socket , server->buffer_out, server->buffered_out_size , MSG_NOSIGNAL | MSG_DONTWAIT)<0)
				server->connected=0;
			clear_string(server->buffer_out, BUFFER_SIZE);
			server->buffered_out_size=0;
			server->send=0;
		}

		// recieve message from server
		if (server->recieve==0){
			server->buffered_in_size = recv(server->socket, server->buffer_in, server->buffer_size, MSG_NOSIGNAL | MSG_DONTWAIT);

			int err = errno;

			//check if any data was actually recieved
			if (server->buffered_in_size>0) {
				server->recieve=1;
				gettimeofday(server->last_reception, NULL);
			}
			else if (err == EAGAIN || err == EWOULDBLOCK)
				server->buffered_in_size=0;
			else if (server->buffered_in_size<0)
				server->connected=0;
		}
		// need to create function to handle
		// the different types of messages
		if (server->recieve==1){
			//mutex 1 lock to replace typing variable
			int mode;
			char body[BUFFER_SIZE];
			char username[CREDENTIAL_SIZE];
			char password[CREDENTIAL_SIZE];
			char destination[CREDENTIAL_SIZE];
			parse_message(server->buffer_in, &mode, username, password, destination, body);
			//printf("%s\n", server->buffer_in); // Remove Later
			/*
********************MY EDITS*******************************************
*****************selective rendering of group and private messages*****
*/
			// print out chat messages
			switch (mode) {
				case 6:
					pthread_mutex_lock(&server->lock);
					if (server->in_private_chat==1)
						printf("%s: %s", username, body);
					pthread_mutex_unlock(&server->lock);
					break;
				case 7:
					pthread_mutex_lock(&server->lock);
					if(server->in_group_chat==1)
						printf("%s: %s", username, body);
					pthread_mutex_unlock(&server->lock);
					break;
				case 5: case 8: case 9:
					pthread_mutex_lock(&server->lock);
					printf("\n%s\n", body);
					pthread_mutex_unlock(&server->lock);
					break;
				case 13: case 14:
					if (strcmp(body, "Y")==0)
						server->valid_destination=1;
					else
						server->valid_destination=0;
					break;
				case 4:
					strcpy(server->password, body);
					pthread_mutex_lock(&server->lock);
					printf("PASSWORD SUCCESSFULLY CHANGED\n");
					pthread_mutex_unlock(&server->lock);
					break;
				case 15:
					server->is_banned_or_kicked=atoi(body);
					break;
				case 16:
					server->is_admin = atoi(body);
					break;
				case 17: case 18:
					pthread_mutex_lock(&server->lock);
					printf("\n%s\n\n", body);
					pthread_mutex_unlock(&server->lock);
					break;
				case 11:
					if (atoi(body)==1) {
						server->is_banned_or_kicked = 1;
						printf("\nYOU WERE BANNED! PRESS ENTER TO CONTINUE. . .\n");
					}
					else
						printf("USER WAS BANNED\n");
					break;
				case 12:
					if (atoi(body)==1){
						server->is_banned_or_kicked = -1;
						printf("\nYOU WERE KICKED! PRESS ENTER TO CONTINUE!. . .\n");
					}
					else
						printf("USER WAS KICKED\n");
					break;
				case 19:
					printf("\nTHERE WAS AN ERROR RETRIEVING THE FILE\nTRY A DIFFERENT FILE\n\n");
					break;
				case 10:
					recieve_file(body, server);
					break;
				case 3:
					printf("YOU LOGGED OFF");
					break;
				case 20:
                                        send(server->socket , pingMessage , strlen(pingMessage), MSG_NOSIGNAL | MSG_DONTWAIT);
					break;
			}
			//mutex 1 unlock to replace typing variable

			// once the recieved message has been utalyzed,
			// the buffer must be cleared, except in instances
			// where the main thread must handle the response.
			switch(mode) {
				case 5: case 13: case 8: case 9: case 14: case 15: case 16: case 4: case 17: case 11: case 12: case 18: case 19: case 10: case 3:
					server->buffered_in_size=0;
					clear_string(server->buffer_in, BUFFER_SIZE);
					server->recieve=0;
					sem_post(&server->mutex);
					break;
				case 0: case 1: case 2:
					break;
				default:
					clear_string(server->buffer_in, BUFFER_SIZE);
					server->buffered_in_size=0;
					server->recieve=0;
					break;
			}
		}
		gettimeofday(current_time, NULL);
		if ((current_time->tv_sec - server->last_reception->tv_sec)>TIMEOUT_INTERVAL){
			printf("\n\nIT SEEMS THE SERVER IS NO LONGER RESPONDING\n\nTRY AGAIN LATER\n\n");
			server->connected=0;
		}
	}
	free(current_time);
	exit(0);
	return NULL;
}



// This function builds the stucture used for
// communication to and from the server
server_t *build_server_structure(void){
	server_t *server = (server_t *)malloc(sizeof(server_t));
	server->username[0]='\0';
	server->password[0]='\0';
	server->socket = 0;
	server->buffer_size=BUFFER_SIZE;
	server->buffer_in=(char *)malloc(BUFFER_SIZE);
	server->buffer_in[0]='\0';
	server->buffered_in_size=0;
	server->buffer_out=(char *)malloc(BUFFER_SIZE);
	server->buffer_out[0]='\0';
	server->buffered_out_size=0;
	server->send=0;
	server->is_banned_or_kicked=0;
	server->recieve=0;
	server->connected=1;
	server->logged_in=0;
	server->is_banned_or_kicked=0;
	server->in_group_chat=0;
	server->in_private_chat=0;
	server->username_private_chat[0]='\0';
	server->last_reception = (struct timeval *)malloc(sizeof(struct timeval));
	gettimeofday(server->last_reception, NULL);
	return server;
}



// This function disconnects from the server
// and frees up the memory allocated for
// communication. Should be called after
// communication is done.
void disconnect(server_t *server){
	close(server->socket);
	free(server->last_reception);
	free(server->buffer_in);
	free(server->buffer_out);
	free(server);
}


/* This function will prompt the user for the selection of a menu option,
 * denoted by a number. The function then checks to ensure a number was
 * given, and if so, converts it to an integer. If not, the functions asks
 * again.
 *
 * return
 *	- The integer given by the user.
 */
int menu_input(server_t *server){
	char input[CREDENTIAL_SIZE]={0};
	int valid = 0, i;
	do{
		do {
			printf("Enter an action: ");
			fgets(input, CREDENTIAL_SIZE, stdin);
		}while(strlen(input)<=1 && server->is_banned_or_kicked == 0);
		if (server->is_banned_or_kicked == 0) {
			input[strlen(input)-1]=0;
			for(i=0; input[i]!='\0'; i++){
				valid = 0;
				if (input[i]<'0' || input[i]>'9')
					break;
				valid = 1;
			}
		}
	}while(!valid && server->is_banned_or_kicked == 0);
	return atoi(input);
}



/* This function will print the entire menu to the screen, and
 * perform some action based on the user's input.
 *
 * return
 *	- The selection of the user.
 */
int main_menu(server_t *server){
	int selection = -1;
	if(server->is_banned_or_kicked==0) {
		// print the menu
		printf("\n-=| MAIN MENU |=-\n\n");
		printf("1. View current online number\n");
		printf("2. Enter the group chat\n");
		printf("3. Enter the private chat\n");
		printf("4. View chat history\n");
		printf("5. File transfer\n");
		printf("6. Change the password\n");
		printf("7. Logout\n");
		printf("8. Administrator\n");
		printf("\n");

		// get the selection

		do{
			selection = menu_input(server);
		}while((selection<0 || selection>8) && server->is_banned_or_kicked==0);
	}
	// perform selection
	fflush(stdout);
	if(is_banned_or_kicked(server)==0) {
		switch(selection){
			case 0:
				break;
			case 1: // case of show all online users
				request_users(server);
				break;
			case 2: // enter group chat
				group_chat(server);
				break;
			case 3: // enter private chat
				private_chat(server);
				break;
			case 4: //view chat history
				chat_history(server);
				break;
			case 5: //file transfer
				file_menu(server);
				break;
			case 6: //change password
				change_password(server);
				break;
			case 7: // logout case
				logout(server);
				server->logged_in=0;
				server->username[0]='\0';
				server->password[0]='\0';
				break;
			case 8: // admin login
				admin_login(server);
				break;
		}
	}// check if user is banned
	else if(is_banned_or_kicked(server)==1) {
		printf("YOU HAVE BEEN BANNED, GOODBYE!\n");
		logout(server);
		server->logged_in=0;
		server->username[0]='\0';
		server->password[0]='\0';
		selection = 7;
	}
	else { // then user has been kicked
		printf("YOU HAVE BEEN KICKED, GOODBYE!\n");
		logout(server);
		server->logged_in=0;
		server->username[0]='\0';
		server->password[0]='\0';
		selection = 7;
	}

	return selection;
}

/* WARNING, ONLY CALL THIS FUNCTION INSIDE THE SERVER COMMUNICATION THREAD
 * This function will accept the binary data of a file, and store
 * it in a file, denoted by the filename given in the body of the
 * last message.
 *
 * char *body
 *	- a string containing the filename and file size, seperated by an underscore
 *
 * char *destination
 *	- the user that the file is intended for
 *
 * client_list_t *current
 *	- the user that is sending the file
 *
 * return
 *	- N/A
 */
void recieve_file(char *body, server_t *server){
	char *filename;
	char *str_size;
	unsigned long size;
	int amount_read;

	// split the "body" at the last occuring underscore,
	// which will give us the file's name, and size.
	int i=0;
	while(body[i]!='\0')
		i++;
	while(body[i]!='_')
		i--;
	body[i] = '\0';
	filename = body;
	str_size = &body[i+1];

	// convert the string of filesize to an unsigned long
	size = atoul(str_size);

	// recieve the file chunks from the server, and write them to a file
	FILE *fp = fopen(filename, "w");
	while(size>0){
		amount_read = recv(server->socket , server->buffer_in, BUFFER_SIZE, MSG_NOSIGNAL);
		size-=amount_read;
		fwrite(server->buffer_in, 1, amount_read, fp);
	}
	fclose(fp);

	return;
}



/* This function will convert a string of numbers to an
 * unsigned long, and return this value to the user.
 *
 * char *value
 *	- pointer to a string of numbers
 *
 * return
 *	- unsigned long denoting the numerical value of the string
 */
unsigned long atoul(char *value){
	int i=0;
	unsigned long number = 0;
	while(value[i]!='\0'){
		number *= 10;
		number += value[i] - '0';
		i++;
	}
	return number;
}
