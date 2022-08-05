#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stdbool.h>

#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>

#include "constants.h"
#include "structs.h"




//char buffer[1025]; //data buffer of 1K 


struct clientCred cred;
struct clientDetails clientDet;

struct train train;
struct bookingRequest breq;
struct clientRequest cr;

struct flock loginlock;



int main(int argc , char *argv[]) 
{ 

	int serv_sd , addrlen ,cliaddrlen, clisd , valread; 
	struct sockaddr_in serv_address,cliaddr; 
		
	//create a server socket 
	if( (serv_sd = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	 
	//type of socket created 
	serv_address.sin_family = AF_INET; 
	serv_address.sin_addr.s_addr = INADDR_ANY; 
	serv_address.sin_port = htons(PORT); 
		
	if (bind(serv_sd, (struct sockaddr *)&serv_address, sizeof(serv_address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

	if (listen(serv_sd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	//accept the incoming connection 
	addrlen = sizeof(serv_address);
	cliaddrlen = sizeof(cliaddr); 
	puts("Waiting for connections ..."); 
		
	while(TRUE) 
	{ 
			
		if ((clisd = accept(serv_sd, (struct sockaddr *)&cliaddr,(socklen_t*)&cliaddrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 

		printf("New connection - ip : %s , port : %d\n" , 
							inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

		if (!fork( )) {
          		 close(serv_sd);
         		 
				 clientops(clisd);
		

				//Client exiting
				printf("Host disconnected - ip : %s , port : %d\n" ,  
                          				inet_ntoa(cliaddr.sin_addr) , ntohs(cliaddr.sin_port));
           		 exit(1);
     		} else
                	close(clisd);

	
	}  
	
		
	return 0; 
} 


void clientops(int clisd)
{
	
	int userfd = open("account.txt",O_RDWR);
	
	loginlock.l_type=F_WRLCK;
	loginlock.l_whence=SEEK_SET;
	loginlock.l_start=0;
	loginlock.l_len=0;
	loginlock.l_pid=getpid();

	char choice[10];
	
	bool auth = false;
	read( clisd , &clientDet, sizeof(clientDet)); 
	
	switch(clientDet.clientType)
					{
					case  1:
						printf("Login request: Agent\n");
						//acquire
						auth = authenticatUser(clientDet.cred.userid,clientDet.cred.password ,1,userfd);
						if(auth)
						{
							printf("login success\n");
							send(clisd , "success" , 10 , 0 ); 
							
							AgentOperations(clisd);
							loginlock.l_type=F_UNLCK;
							fcntl(userfd,F_SETLK,&loginlock);
						}
						else
						{
							printf("Login failed\n");
							send(clisd , "Login failed" , 50 , 0 ); 
						}
						break;

					case 2:
						printf("Login request: Customer\n");
						//acquire
						auth = authenticatUser(clientDet.cred.userid,clientDet.cred.password ,2,userfd);
						if(auth)
						{
							printf("login success\n");
							send(clisd , "success" , 10 , 0 ); 
							
							//read
						
							CustomerOperations(clisd);
							loginlock.l_type=F_UNLCK;
							fcntl(userfd,F_SETLK,&loginlock);
		//realse write 
						}
						else
						{
							printf("Login failed\n");
							send(clisd , "Login failed" , 50 , 0 ); 
						}

						break;

					case 3:
						printf("Login request: Admin\n");

						auth = authenticatUser(clientDet.cred.userid,clientDet.cred.password ,3,userfd);
						if(auth)
						{
							printf("login success\n");
							send(clisd , "success" , 10 , 0 ); 
							
							AdminOperations(clisd);
							loginlock.l_type=F_UNLCK;
							fcntl(userfd,F_SETLK,&loginlock);
						}
						else
						{
							printf("Login failed\n");
							send(clisd , "failed" , 10 , 0 ); 
						}
						break;

					default :
						printf("Invalid choice\n");


						break;


					}
	

}



bool authenticatUser(int userid , int password, int accType,int userfd)
{

	//int userfd = open("account.txt",O_RDONLY);
	int read_ret =1;
	struct account user;
	int found =0;
	int index=0;

	while(read_ret!=0)
	{
		read_ret = read(userfd,&user,sizeof(struct account));
		index++;
		if(user.uid == userid)  
		{
			found =1 ;
			break;
		}		
	}
	//close(userfd);
	
	if(accType==1) //agent
	{
		loginlock.l_type=F_RDLCK;	
	}
	else
	{
		loginlock.l_type=F_WRLCK;
	}
	
	index--;
	loginlock.l_whence=SEEK_SET;
	loginlock.l_start=index*sizeof(struct account);
	loginlock.l_len=sizeof(struct account);
	loginlock.l_pid=getpid();
	
	
	if(found ==1)
	{
		if(userid == user.uid &&  password == user.pin && accType == user.accType)
		{
			if(fcntl(userfd,F_SETLKW,&loginlock) == -1)
			{
				perror("lock error:");
			}
			else
			{
				printf("Lock acquired:\n");
			}
			
			
			return true;
		}
		else
		{
			return false;
		}
	}
	else
		return false;
}
