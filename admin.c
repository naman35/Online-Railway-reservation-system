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


struct flock userlock;
struct flock trainlock;

void AdminOperations(int sockfd)
{
	
	userlock.l_type=F_WRLCK;
	userlock.l_whence=SEEK_SET;
	userlock.l_start=0;
	userlock.l_len=0;
	userlock.l_pid=getpid();
	
	trainlock.l_type=F_WRLCK;
	trainlock.l_whence=SEEK_SET;
	trainlock.l_start=0;
	trainlock.l_len=0;
	trainlock.l_pid=getpid();
	
	while(1) {
	struct clientRequest cr ;
	read(sockfd , &cr, sizeof(cr));
	
	struct clientRequest delcr;
	struct train train = cr.tr;
	struct bookingRequest breq = cr.bookReq;
	int command = cr.command;
	struct train ret;
	struct serverResponse response;
	
	struct account user;
	struct bookingDetails bookdets;
	
	struct account emptyUser;
	emptyUser.uid = -1;
	emptyUser.pin = -1;
	emptyUser.accType = -1;
	
	struct train emptyTrain;
	emptyTrain.totalBooked = -1;
	emptyTrain.trainNo = -1;
	emptyTrain.totalSeats = -1;
	
	
	printf("COMMAND %d\n",command);
	
	int found=0;
	int fd;
	int userfd;
	int trainfd;
	int read_ret=1;
	
	switch(command)
	{
		case SEARCH_USER:
			
			userfd = open("account.txt",O_RDONLY);
				
				read_ret =1;
				response.commandStatus =0;
				
				//lock user
						userlock.l_start=0;
						userlock.l_len=0;
						userlock.l_type = F_RDLCK;
						fcntl(userfd,F_SETLK,&userlock);
				
				while(read_ret!=0)
				{
					read_ret = read(userfd,&user,sizeof(struct account));
					
					if(user.uid == cr.uid)  
					{
						response.commandStatus = 1;
						//response.tr = ret;
						break;
					}
					
				}
				//unlock user
						userlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
				close(userfd);
			
				send(sockfd , &response ,sizeof(struct serverResponse),0);
		
		break;
		
		case SEARCH_TRAIN:
				printf("Searching...\n");
				fd=open("train.txt",O_RDONLY);
				
				response.commandStatus =0;
				
				//lock train
						trainlock.l_start=0;
						trainlock.l_len=0;	
						trainlock.l_type = F_RDLCK;
						fcntl(fd,F_SETLK,&trainlock) ;
				
				while(read_ret!=0)
				{
					
					read_ret = read(fd,&ret,sizeof(struct train));
					
					if(ret.trainNo == train.trainNo)
					{
						found=1;
						response.commandStatus = 1;
						response.tr = ret;
						break;
					}
					
				}
				
				//unlock train
						trainlock.l_type = F_UNLCK;
						fcntl(fd,F_SETLK,&trainlock);
						
				close(fd);
			
				send(sockfd , &response ,sizeof(struct serverResponse),0);
				
		break;
		
		
		case ADD_USER:
		
				userfd = open("account.txt",O_RDWR);
				
				int addusrptr =0;
				read_ret =1;
				response.commandStatus =1;
				response.newuserStatus=1;
				while(read_ret!=0)
				{
					read_ret = read(userfd,&user,sizeof(struct account));
					addusrptr++;
					if(user.uid == cr.uid)  
					{
						response.newuserStatus=0;
						break;
					}
					
				}
				
				if(response.newuserStatus ==1)
				{
					addusrptr--;
					//lock user
						userlock.l_start=addusrptr*sizeof(struct account);
						userlock.l_len=sizeof(struct account);	
						userlock.l_type = F_WRLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
					lseek(userfd,addusrptr*sizeof(struct account),SEEK_SET);
					if(write(userfd,&cr.newuser,sizeof(struct account))==-1)
					{
						response.commandStatus=0;
						perror("write error\n");
					}
					
					//unlock user
						userlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&userlock);
					
					close(userfd);
					
				}
				else
				{
					printf("User already exists!!!\n");
				}
				send(sockfd , &response ,sizeof(struct serverResponse),0);
		
		break;
		
		case DELETE_USER:
		
			userfd = open("account.txt",O_RDWR);
				
				int delusrptr =0;
				read_ret =1;
				response.commandStatus =1;
				response.newuserStatus=1;
				while(read_ret!=0)
				{
					read_ret = read(userfd,&user,sizeof(struct account));
					delusrptr++;
					if(user.uid == cr.uid)  
					{
						response.newuserStatus=0;
						break;
					}
					
				}
				
				if(response.newuserStatus ==0)
				{
					delusrptr--;
					//lock user
						userlock.l_start=delusrptr*sizeof(struct account);
						userlock.l_len=sizeof(struct account);	
						userlock.l_type = F_WRLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
					lseek(userfd,delusrptr*sizeof(struct account),SEEK_SET);
					if(write(userfd,&emptyUser,sizeof(struct account))==-1)
					{
						response.commandStatus=0;
						perror("write error\n");
					}
					
					//unlock user
						userlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
					close(userfd);
					
				}
				else
				{
					printf("User does not exists!!!\n");
				}
				send(sockfd , &response ,sizeof(struct serverResponse),0);
		
		
		
		break;
		
		case ADD_TRAIN:
			
				trainfd = open("train.txt",O_RDWR);
				
				int addtrainptr =0;
				read_ret =1;
				response.commandStatus =1;
				response.newtrainStatus=1;
				
				while(read_ret!=0)
				{
					read_ret = read(trainfd,&ret,sizeof(struct train));
					addtrainptr++;
					if(ret.trainNo == cr.trainNo)  
					{
						response.newtrainStatus=0;
						break;
					}
					
				}
				
				if(response.newtrainStatus ==1)
				{
					addtrainptr--;
					
					 //lock train
						trainlock.l_start=addtrainptr*sizeof(struct train);
						trainlock.l_len=sizeof(struct train);	
						trainlock.l_type = F_WRLCK;
						fcntl(trainfd,F_SETLK,&trainlock) ;
					
					lseek(trainfd,addtrainptr*sizeof(struct train),SEEK_SET);
					if(write(trainfd,&cr.tr,sizeof(struct train))==-1)
					{
						response.commandStatus=0;
						perror("write error\n");
					}
					
						//unlock train
						trainlock.l_type = F_UNLCK;
						fcntl(trainfd,F_SETLK,&trainlock);
					
					close(trainfd);
					
				}
				else
				{
					printf("train already exists!!!\n");
				}
				send(sockfd , &response ,sizeof(struct serverResponse),0);
		
		
		
		break;
		
		case DELETE_TRAIN:
		
		trainfd = open("train.txt",O_RDWR);
				
				int deltrainptr =0;
				read_ret =1;
				response.commandStatus =1;
				response.newtrainStatus=1;
				while(read_ret!=0)
				{
					read_ret = read(trainfd,&ret,sizeof(struct train));
					deltrainptr++;
					if(ret.trainNo == cr.trainNo)  
					{
						response.newtrainStatus=0;
						break;
					}
					
				}
				
				if(response.newuserStatus ==0)
				{
					deltrainptr--;
					 //lock train
						trainlock.l_start=deltrainptr*sizeof(struct train);
						trainlock.l_len=sizeof(struct train);	
						trainlock.l_type = F_WRLCK;
						fcntl(trainfd,F_SETLK,&trainlock);
						
					lseek(trainfd,deltrainptr*sizeof(struct train),SEEK_SET);
					if(write(trainfd,&emptyTrain,sizeof(struct train))==-1)
					{
						response.commandStatus=0;
						perror("write error\n");
					}
					
					//unlock train
						trainlock.l_type = F_UNLCK;
						fcntl(trainfd,F_SETLK,&trainlock);
					
					close(trainfd);
					
				}
				else
				{
					printf("Train does not exists!!!\n");
				}
				send(sockfd , &response ,sizeof(struct serverResponse),0);
		
		
		break;
		
		
		case MODIFY_USER:
		
			userfd = open("account.txt",O_RDWR);
				
				int modusrptr =0;
				read_ret =1;
				response.commandStatus =1;
				response.newuserStatus=1;
				while(read_ret!=0)
				{
					read_ret = read(userfd,&user,sizeof(struct account));
					modusrptr++;
					if(user.uid == cr.uid)  
					{
						response.newuserStatus=0;
						response.acc = user;
						break;
					}
					
				}
		
				send(sockfd , &response ,sizeof(struct serverResponse),0);
				
				if(response.newuserStatus ==0)
				{
					read(sockfd , &cr, sizeof(cr));
					modusrptr--;
					
					//lock user
						userlock.l_start=modusrptr*sizeof(struct account);
						userlock.l_len=sizeof(struct account);	
						userlock.l_type = F_WRLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
					
					lseek(userfd,modusrptr*sizeof(struct account),SEEK_SET);
					if(write(userfd,&cr.newuser,sizeof(struct account))==-1)
					{
						response.commandStatus=0;
						perror("write error\n");
					}
					
					//unlock user
						userlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
				}
				else
				{
					response.commandStatus=0;
				}
				
				send(sockfd , &response ,sizeof(struct serverResponse),0);
				
		break;
		
		case MODIFY_TRAIN:
			
			userfd = open("train.txt",O_RDWR);
				
				int modtrainptr =0;
				read_ret =1;
				response.commandStatus =1;
				response.newtrainStatus=1;
				while(read_ret!=0)
				{
					read_ret = read(userfd,&ret,sizeof(struct train));
					modtrainptr++;
					if(ret.trainNo == cr.trainNo)  
					{
						response.newtrainStatus=0;
						response.tr = ret;
						break;
					}
					
				}
		
				send(sockfd , &response ,sizeof(struct serverResponse),0);
				
				if(response.newtrainStatus ==0)
				{
					read(sockfd , &cr, sizeof(cr));
					modtrainptr--;
					
					//lock train
						trainlock.l_start=modtrainptr*sizeof(struct train);
						trainlock.l_len=sizeof(struct train);	
						trainlock.l_type = F_WRLCK;
						fcntl(userfd,F_SETLK,&trainlock);
					
					lseek(userfd,modtrainptr*sizeof(struct train),SEEK_SET);
					if(write(userfd,&cr.tr,sizeof(struct train))==-1)
					{
						response.commandStatus=0;
						perror("write error\n");
					}
						
						//unlock train
						trainlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&trainlock);
						
				}
				else
				{
					response.commandStatus=0;
				}
				
				send(sockfd , &response ,sizeof(struct serverResponse),0);
			
		
		break;
		
		case LOGOUT_ADMIN:
		
			return;
		break;
		
	}
	
	}
	
	
}