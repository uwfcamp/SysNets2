/*
 * Server side C program that echos
 * messages back to the client program
 *
 * @AUTHOR	Curtis Duvall, Nathan Wilkins, Julian Thayer
 * @DATE	3/8/2019
 * @INFO	COP4635 Project 1
 */

#include "server_framework.h" 
 
int main(int argc, char const *argv[])
{
    int server_fd;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    client_list_t *clientList = (client_list_t *) malloc(sizeof(client_list_t));
    clientList->socket=0;
    clientList->connected=0;
    clientList->logged_in=0;
    clientList->last=NULL;
    clientList->next=NULL;
    clientList->username[0]='\0';
    clientList->password[0]='\0';
    memset(&address, '\0', sizeof address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Ensure there is a login registry
    FILE *logins = fopen("logins.txt", "a");
    fclose(logins);

    // Creating socket file descriptor
    printf("STATUS: creating socket file descriptor\n");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    printf("STATUS: binding socket to address\n");
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    printf("STATUS: setting server to listen for new connections\n");
    if (listen(server_fd, 20) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    // make it so that future calls to accept
    // new sockets do not block. This means
    // that the server will keep going if
    // there is no new client to connect to.
    int temp_socket;
    int flags = fcntl(server_fd, F_GETFL);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    while(1)
    {
	char buffer[BUFFER_SIZE];

	int mode;
	char body[BUFFER_SIZE];
	char username[CREDENTIAL_SIZE];
	char password[CREDENTIAL_SIZE];
	char destination[CREDENTIAL_SIZE];

        // Here we will check to see if there is
        // a new connection request, and if there
        // is, we will add it to the list.
	temp_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (temp_socket>0)
            new_connection(clientList, temp_socket);

	// loop through every connection, to
        // see if there is an incoming message.
        // This must be done for any type of
        // message; public, private, file, etc.
        client_list_t *current = clientList;
        while (current!=NULL){
                if (current->connected==1){
                    //clear buffer and path array for message acceptance
                    clear_string(buffer, BUFFER_SIZE);

                    // accept message from client
                    valread = recv( current->socket , buffer, BUFFER_SIZE, MSG_NOSIGNAL | MSG_DONTWAIT);

                    // handle recieved messages
                    if (valread>0){
                        printf("%s\n", buffer);
			parse_message(buffer, &mode, username, password, destination, body);
			printf("%s\n", buffer);
			switch (mode){
				case 0:
					register_user(username, password, current);
					break;
				case 1:
					login_user(username, password, current);
					break;
				case 2:
					// THIS CASE IS RESERVED FOR MESSAGES FROM THE
					// SERVER TO THE CLIENT, TO VERIFY LOGIN SUCCESS
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					if (current->logged_in==1){
						if (strcmp(destination, " ")==0){
							broadcast_message(clientList, current->socket, body, current->username);
							log_into_group(username,body);
						}
						else
							private_message(clientList, body, destination, current->username);
					}
					break;
				case 8:
					break;
				case 9:
					break;
				case 10:
					break;
				case 11:
					break;
				case 12:
					break;
			}
                    }
                }
            // move to the next element in the list
            current = current->next;
        }
    }
    close(server_fd);
    printf("STATUS: connection broken\n");
    return 0;
}

void new_connection(client_list_t *clientList, int socket){
    // there is at least 1 connection
    if (clientList->connected == 1){
        client_list_t *end = clientList;
        while(end->next != NULL)
            end = end->next;
        end->next = (client_list_t *)malloc(sizeof(client_list_t));
        end->next->next = NULL;
        end->next->last = end;
        end->next->socket = socket;
        end->next->connected = 1;
	end->next->logged_in=0;
	end->next->username[0]='\0';
	end->next->password[0]='\0';
    }
    else{ // there are no connections
        clientList->socket = socket;
        clientList->connected = 1;
	clientList->logged_in=0;
	clientList->username[0]='\0';
	clientList->password[0]='\0';
    }
    printf("STATUS: new connection added, socket:%d\n", socket);

}

void remove_connection(client_list_t **clientList, int target_socket){
    // special case, where the target being
    // removed is the first element in the
    // link-list. A double pointer is passed
    // because of the second part of this case.
    if ((*clientList)->socket == target_socket){
        if ((*clientList)->next==NULL){
            (*clientList)->socket = 0;
            (*clientList)->connected = 0;
            (*clientList)->logged_in = 0;
            (*clientList)->username[0]='\0';
            (*clientList)->password[0]='\0';
        }
        else{
            client_list_t *temp = (*clientList);
            (*clientList)=(*clientList)->next;
            (*clientList)->last = NULL;
            free(temp);
        }
        return;
    }

    // look for the correct element in the link list
    client_list_t *target = *clientList;
    while(target->socket != target_socket && target != NULL)
        target = target->next;

    // if the element exists, remove it
    if (target != NULL){
        target->last->next = target->next;
	if (target->next != NULL)
		target->next->last = target->last;
        free(target);
    }
}