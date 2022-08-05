// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdbool.h>

#include "constants.h"
#include "structs.h"



struct clientCred cred;
struct clientDetails clientDet;

void flushBuffer(){
        while ((getchar()) != '\n'); /* Flush input buffer */
}

int main(int argc, char const *argv[]) 
{ 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	
	char buffer[1024] ; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}
	else
	{
		printf("Connected to Server\n");
	}	
	/////after connection/////



	int login_choice;
	int userid;
	int password;
	char loginStatus[50];
	char message1[50];
	
	
	printf("Login as:\n");
	printf("1.Agent\n2.Customer\n3.Admin\n");	
	

	scanf("%d",&login_choice);
	
	printf("Enter userid:\n");
	scanf("%d",&userid);
	
	printf("Enter Password:\n");
	
	scanf("%d",&password);
	
	
	cred.userid = userid;
	cred.password = password;
	
	clientDet.clientType  = login_choice;
	clientDet.cred = cred;
	
	
	send(sock , &clientDet , sizeof(clientDet) , 0 ); 
	
	recv(sock, &loginStatus , 50,0);
	
	
	if(strcmp(loginStatus,"success") == 0)
	{
		
		printf("Login successfull!!!!!\n");
		
		switch(login_choice)
		{
		case 1:

			AgentMenu(sock,userid);

			break;
			
		case 2:

			CustomerMenu(sock,userid);
			break;
			
		case 3:
		
			AdminMenu(sock,userid);
			break;
			
			
		}
		
		
		
	}
	else
	{
		printf("Login fail!!!!!\n");
	}
	
	
	
	return 0; 
} 

