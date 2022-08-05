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

void AgentOperations(int sockfd)
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
	
	
	
	printf("COMMAND %d\n",command);
	
	int found=0;
	int fd;
	int userfd;
	int read_ret=1;
	
	switch(command)
	{
		case SEARCH:
				printf("Searching...\n");
				fd=open("train.txt",O_RDONLY);
						
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
		
		case BOOK_TICKET:
				
				//==========SEARCH=========
				printf("Booking ticket..\n");
				fd=open("train.txt",O_RDWR);
				int index =0;
				while(read_ret!=0)
				{
					read_ret = read(fd,&ret,sizeof(struct train));
					index++;
					if(ret.trainNo == train.trainNo)
					{
						found=1;	
						response.commandStatus = 1;
						response.tr = ret;
						break;
					}
					
				}
				
				userfd = open("account.txt",O_RDWR,0774);
				int uindex=0;
				read_ret =1;
				
				while(read_ret!=0)
				{
					read_ret = read(userfd,&user,sizeof(struct account));
					uindex++;
					if(user.uid == cr.uid)  
					{
						//response.commandStatus = 1;
						//response.tr = ret;
						break;
					}
					
				}
				
				
				
				if(response.commandStatus==1)
				{
					printf("Train found\n");
					printf("Train number %d\n",response.tr.trainNo);
					printf("Available seats %d\n",response.tr.totalSeats - response.tr.totalBooked );
					if(response.tr.totalSeats - response.tr.totalBooked >= breq.number_of_seats)
					{
						
						//updating train details
						ret.totalBooked += breq.number_of_seats;
						ret.totalSeats -= breq.number_of_seats;
						
						index--;
					//lock train
						trainlock.l_start=index*sizeof(struct train);
						trainlock.l_len=sizeof(struct train);	
						trainlock.l_type = F_WRLCK;
						fcntl(fd,F_SETLK,&trainlock) ;
						
						lseek(fd,index*sizeof(struct train),SEEK_SET);
						if(write(fd,&ret,sizeof(struct train))==-1)
						{
								perror("write error\n");
						}
						else{
						response.bookingStatus =1;
						}
						
						//unlock train
						trainlock.l_type = F_UNLCK;
						fcntl(fd,F_SETLK,&trainlock);
						
						//updating user details
						bookdets.trainNo = ret.trainNo;
						bookdets.number_of_seats = breq.number_of_seats;
					
						
						int totalBookings = user.totalBookings;	
						user.booked[totalBookings]=bookdets;
						
						user.totalBookings++;
						uindex--;
		//lock user
						userlock.l_start=uindex*sizeof(struct account);
						userlock.l_len=sizeof(struct account);	
						userlock.l_type = F_WRLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
						lseek(userfd,uindex*sizeof(struct account),SEEK_SET);
						if(write(userfd,&user,sizeof(struct account))==-1)
						{
								perror("write error\n");
						}
						else{
								printf("User data updated\n");
						}
						
						//unlock user
						userlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&userlock);
					}
					else
					{
						printf("No more available seats\n");
					}
					
				}
				else
				{
					printf("Train not found\n");
				}
				
				
				close(fd);
				
				send(sockfd , &response ,sizeof(struct serverResponse),0);
				//=====================
				
				
			
		break;
		
		case CANCEL_TICKET:
			
			userfd = open("account.txt",O_RDWR);
				
				read_ret =1;
				int usrindex=0;
				while(read_ret!=0)
				{
					read_ret = read(userfd,&user,sizeof(struct account));
					usrindex++;
					if(user.uid == cr.uid)  
					{
						
						response.acc = user;
						break;
					}
					
				}
				
				
			
			send(sockfd , &response ,sizeof(struct serverResponse),0);


			read(sockfd , &delcr, sizeof(delcr));
			int delindex = delcr.deleteIndex;
			printf("DELINDEX %d\n",delindex);
			delindex--;
			
			int trainNumber = user.booked[delindex].trainNo;
			int numofseats = user.booked[delindex].number_of_seats;
			
			
			printf("train Number %d",trainNumber);
			
			//updating train db
			
			fd=open("train.txt",O_RDWR);
				int rindex =0;
				while(read_ret!=0)
				{
					read_ret = read(fd,&ret,sizeof(struct train));
					rindex++;
					if(ret.trainNo == trainNumber)
					{
						response.commandStatus = 1;
						break;
					}
					
				}
			
			if(response.commandStatus == 1) {	
			ret.totalBooked -= numofseats;
			ret.totalSeats += numofseats;
			
			rindex--;
			 //lock train
						trainlock.l_start=rindex*sizeof(struct train);
						trainlock.l_len=sizeof(struct train);	
						trainlock.l_type = F_WRLCK;
						fcntl(fd,F_SETLK,&trainlock) ;
						
			lseek(fd,rindex*sizeof(struct train),SEEK_SET);
			if(write(fd,&ret,sizeof(struct train))==-1)
			{
					perror("write error\n");
			}
			//unlock train
						trainlock.l_type = F_UNLCK;
						fcntl(fd,F_SETLK,&trainlock);
						
			close(fd);
			//updating user db
			
			
			for(int i=delindex ; i<user.totalBookings-1 ;i++)
			{
				user.booked[i] = user.booked[i+1];
				
			}
			user.totalBookings--;
			
			usrindex--;
			//lock user
						userlock.l_start=usrindex*sizeof(struct account);
						userlock.l_len=sizeof(struct account);	
						userlock.l_type = F_WRLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
						
			lseek(userfd,usrindex*sizeof(struct account),SEEK_SET);
			if(write(userfd,&user,sizeof(struct account))==-1)
			{
					perror("write error\n");
			}
			
			//unlock user
						userlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&userlock);
			
			close(userfd);
			}
			
			send(sockfd , &response ,sizeof(struct serverResponse),0);
		
		break;
		
		case PREV_BOOKING:
				
			userfd = open("account.txt",O_RDONLY);
				
				read_ret =1;
				
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
						
						response.acc = user;
						break;
					}
					
				}
				
				//unlock user
						userlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&userlock);
				
				close(userfd);
			
			send(sockfd , &response ,sizeof(struct serverResponse),0);

			
		break;
		
		case UPDATE_BOOKING:
		
		userfd = open("account.txt",O_RDWR);
				
				read_ret =1;
				int usrIndex=0;
				while(read_ret!=0)
				{
					read_ret = read(userfd,&user,sizeof(struct account));
					usrIndex++;
					if(user.uid == cr.uid)  
					{
						
						response.acc = user;
						break;
					}
					
				}
			
			
			send(sockfd , &response ,sizeof(struct serverResponse),0);
			
				read(sockfd , &delcr, sizeof(delcr));
			int updateindex = delcr.updateIndex;
			printf("UPDATE INDEX %d\n",updateindex);
			updateindex--;
			
			int TrainNumber = user.booked[updateindex].trainNo;
			int Numofseats = user.booked[updateindex].number_of_seats;
			int updatedSeats = delcr.updatedSeats;
			
			printf("train Number %d",TrainNumber);
			
				//updating train db
			response.commandStatus = 0;
			fd=open("train.txt",O_RDWR);
				int Rindex =0;
				while(read_ret!=0)
				{
					read_ret = read(fd,&ret,sizeof(struct train));
					Rindex++;
					if(ret.trainNo == TrainNumber)
					{
						response.commandStatus = 1;
						break;
					}
					
				}
				
			if(response.commandStatus == 1) {	
			
			//reversing old booking
			ret.totalBooked -= Numofseats;
			ret.totalSeats += Numofseats;
			
			//adding new booking
			ret.totalBooked += updatedSeats;
			ret.totalSeats -= updatedSeats;
			
			Rindex--;
			//lock train
						trainlock.l_start=Rindex*sizeof(struct train);
						trainlock.l_len=sizeof(struct train);	
						trainlock.l_type = F_WRLCK;
						fcntl(fd,F_SETLK,&trainlock) ;
						
			lseek(fd,Rindex*sizeof(struct train),SEEK_SET);
			if(write(fd,&ret,sizeof(struct train))==-1)
			{
					perror("write error\n");
			}
			
			//unlock train
						trainlock.l_type = F_UNLCK;
						fcntl(fd,F_SETLK,&trainlock);
						
			
			close(fd);
			//updating user db
			
			
			user.booked[updateindex].number_of_seats = updatedSeats;
			printf("SEATS\n: %d",updatedSeats);
			usrIndex--;
			//lock user
						userlock.l_start=usrIndex*sizeof(struct account);
						userlock.l_len=sizeof(struct account);	
						userlock.l_type = F_WRLCK;
						fcntl(userfd,F_SETLK,&userlock);
						
			lseek(userfd,usrIndex*sizeof(struct account),SEEK_SET);
			if(write(userfd,&user,sizeof(struct account))==-1)
			{
					perror("write error\n");
			}
			
			//unlock user
						userlock.l_type = F_UNLCK;
						fcntl(userfd,F_SETLK,&userlock);
			
			close(userfd);
			}
			
			send(sockfd , &response ,sizeof(struct serverResponse),0);
				
			
		break;
		
		case LOGOUT:
		
		return;
		
		break;
	}
	}//while end
} 