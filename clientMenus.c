#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
//#define PORT 8888 
#include <stdbool.h>
#include<stdlib.h>

#include "constants.h"
#include "structs.h"


void  CustomerMenu(int sockfd ,int userid)
{
	int options;
	int delindex;
	int updateindex;
	int updatedSeats;
	struct train train;
	struct bookingRequest breq;
	struct clientRequest cr;
	struct serverResponse response;
	
	int train_no;
	int no_of_seats;
	while(1)
	{
	printf("----------------------------------------------------------------------------------------------------------------------------------\n");
		printf("1.Search\n2.Book ticket\n3.Cancel Ticket\n4.View Previous Bookings\n5.Update Booking\n6.Log out");
        	printf("\nEnter choice:");
		scanf("%d",&options);		
		flushBuffer();
		
		//printf("%d",options);
		
		switch(options)
		{
				case SEARCH:
				
				printf("Enter train number:\n");
				scanf("%d",&train_no);
				//printf("Enter number of seats to be booked:\n");
				//scanf("%d",&no_of_seats);
				
				train.trainNo = train_no;
				breq.trainNo = train_no;
				//breq.number_of_seats = no_of_seats;
				
				//search train
				cr.command = SEARCH; //search command
				cr.tr = train;
				cr.bookReq = breq;
				send(sockfd , &cr , sizeof(cr),0);
				char buff[10];
				read(sockfd,&response ,sizeof(struct serverResponse));
				//printf("Search result: %d\n",response.commandStatus);
				if(response.commandStatus==1)
				{
					printf("Train found\n");
					printf("Train number %d\n",response.tr.trainNo);
					printf("Available seats %d\n",response.tr.totalSeats );
				}
				else
				{
					printf("Train not found\n");
				}
				break;
				
				case BOOK_TICKET:
				
				//=========SEARCH==========
				printf("Enter train number:\n");
				scanf("%d",&train_no);
				printf("Enter number of seats to be booked:\n");
				scanf("%d",&no_of_seats);
				
				train.trainNo = train_no;
				breq.trainNo = train_no;
				breq.number_of_seats = no_of_seats;
				
				//search train
				cr.command = BOOK_TICKET; //book command
				cr.tr = train;
				cr.bookReq = breq;
				cr.uid = userid;
				
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				if(response.commandStatus==1)
				{
					/*printf("Train found\n");
					printf("Train number %d\n",response.tr.trainNo);
					printf("Available seats %d\n",response.tr.totalSeats - response.tr.totalBooked );*/
					if(response.tr.totalSeats - response.tr.totalBooked >= no_of_seats)
					{
						if(response.bookingStatus == 1)
						{
							 printf("Booking successful!!!\n");
						}
						else
						{
							printf("Booking error\n");
						}
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
				//==========================
				
				
				break;
				
				case CANCEL_TICKET:
				
				cr.uid = userid;
				cr.command = CANCEL_TICKET;
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				printf("Bookings = %d \n",response.acc.totalBookings);
				int t = 0;
				int b=1;
				
				while(t<response.acc.totalBookings)
				{
					printf("===========Booking number %d===========\n",b);
					printf("Train no: %d\n",response.acc.booked[t].trainNo);
					printf("Number of seats: %d\n\n",response.acc.booked[t].number_of_seats);
					t++;
					b++;
				}
				
				printf("\nEnter the booking number to be deleted:");
				scanf("%d",&delindex);
				
				cr.deleteIndex = delindex;
				
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));

				if(response.commandStatus == 1) {	
					printf("Booking Deleted Successfully!!!\n");
				}
				else
				{
					printf("Booking Deletion Failed!!!\n");

				}
				
				break;
				
				case PREV_BOOKING:
				
				cr.uid = userid;
				cr.command = PREV_BOOKING;
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				printf("Total Bookings = %d \n",response.acc.totalBookings);
				 int tot = response.acc.totalBookings;
				 int bi=1;
				while(tot>0)
				{
					printf("===========Booking number %d===========\n",bi);	
					printf("Train no: %d\n",response.acc.booked[tot-1].trainNo);
					printf("Number of seats: %d\n\n",response.acc.booked[tot-1].number_of_seats);
					tot--;
					bi++;
				}
				
				break;
				
				case UPDATE_BOOKING:
				
				cr.uid = userid;
				cr.command = UPDATE_BOOKING;
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				printf("Bookings = %d \n",response.acc.totalBookings);
				int ti = 0;
				int booki=1;
				
				while(ti<response.acc.totalBookings)
				{
					printf("===========Booking number %d===========\n",booki);
					printf("Train no: %d\n",response.acc.booked[ti].trainNo);
					printf("Number of seats: %d\n\n",response.acc.booked[ti].number_of_seats);
					ti++;
					booki++;
				}
				
				printf("\nEnter the booking number to be updated:");
				scanf("%d",&updateindex);
				
				printf("\nEnter the new number of seats:");
				scanf("%d",&updatedSeats);
				
				
				cr.updateIndex = updateindex;
				cr.updatedSeats = updatedSeats;

				
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));

				if(response.commandStatus == 1) {	
					printf("Booking Updated Successfully!!!\n");
				}
				else
				{
					printf("Booking Updation Failed!!!\n");

				}
				
				break;
			 
			 case LOGOUT:
				
				cr.command = LOGOUT;
				send(sockfd , &cr , sizeof(cr),0);
			     exit(1);
			    break;
			
			
		}
	
	
	}
}




void  AgentMenu(int sockfd ,int agentid)
{
	int options;
	int delindex;
	int updateindex;
	int updatedSeats;
	struct train train;
	struct bookingRequest breq;
	struct clientRequest cr;
	struct serverResponse response;
	int UserId;
	int train_no;
	int no_of_seats;
	while(1)
	{
	printf("----------------------------------------------------------------------------------------------------------------------------------\n");
		printf("1.Search\n2.Book ticket\n3.Cancel Ticket\n4.View Previous Bookings\n5.Update Booking\n6.Log out");
        	printf("\nEnter choice:");
		scanf("%d",&options);		
		flushBuffer();
		
		//printf("%d",options);
		
		switch(options)
		{
				case SEARCH:
				
				printf("Enter train number:\n");
				scanf("%d",&train_no);
				//printf("Enter number of seats to be booked:\n");
				//scanf("%d",&no_of_seats);
				
				train.trainNo = train_no;
				breq.trainNo = train_no;
				//breq.number_of_seats = no_of_seats;
				
				//search train
				cr.command = SEARCH; //search command
				cr.tr = train;
				cr.bookReq = breq;
				send(sockfd , &cr , sizeof(cr),0);
				char buff[10];
				read(sockfd,&response ,sizeof(struct serverResponse));
				//printf("Search result: %d\n",response.commandStatus);
				if(response.commandStatus==1)
				{
					printf("Train found\n");
					printf("Train number %d\n",response.tr.trainNo);
					printf("Available seats %d\n",response.tr.totalSeats );
				}
				else
				{
					printf("Train not found\n");
				}
				break;
				
				case BOOK_TICKET:
				
				//=========SEARCH==========
				printf("Enter train number:\n");
				scanf("%d",&train_no);
				printf("Enter number of seats to be booked:\n");
				scanf("%d",&no_of_seats);
				printf("Enter the user id of customer:\n");
				scanf("%d",&UserId);
				
				train.trainNo = train_no;
				breq.trainNo = train_no;
				breq.number_of_seats = no_of_seats;
				
				//search train
				cr.command = BOOK_TICKET; //book command
				cr.tr = train;
				cr.bookReq = breq;
				cr.uid = UserId;
				
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				if(response.commandStatus==1)
				{
					/*printf("Train found\n");
					printf("Train number %d\n",response.tr.trainNo);
					printf("Available seats %d\n",response.tr.totalSeats - response.tr.totalBooked );*/
					if(response.tr.totalSeats - response.tr.totalBooked >= no_of_seats)
					{
						if(response.bookingStatus == 1)
						{
							 printf("Booking successful!!!\n");
						}
						else
						{
							printf("Booking error\n");
						}
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
				//==========================
				
				
				break;
				
				case CANCEL_TICKET:
				
				printf("Enter the user id of customer:\n");
				scanf("%d",&UserId);
				cr.uid = UserId;
				cr.command = CANCEL_TICKET;
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				printf("Bookings = %d \n",response.acc.totalBookings);
				int t = 0;
				int b=1;
				
				while(t<response.acc.totalBookings)
				{
					printf("===========Booking number %d===========\n",b);
					printf("Train no: %d\n",response.acc.booked[t].trainNo);
					printf("Number of seats: %d\n\n",response.acc.booked[t].number_of_seats);
					t++;
					b++;
				}
				
				printf("\nEnter the booking number to be deleted:");
				scanf("%d",&delindex);
				
				cr.deleteIndex = delindex;
				
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));

				if(response.commandStatus == 1) {	
					printf("Booking Deleted Successfully!!!\n");
				}
				else
				{
					printf("Booking Deletion Failed!!!\n");

				}
				
				break;
				
				case PREV_BOOKING:
				
				printf("Enter the user id of customer:\n");
				scanf("%d",&UserId);
				cr.uid = UserId;
				cr.command = PREV_BOOKING;
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				printf("Total Bookings = %d \n",response.acc.totalBookings);
				 int tot = response.acc.totalBookings;
				 int bi=1;
				while(tot>0)
				{
					printf("===========Booking number %d===========\n",bi);	
					printf("Train no: %d\n",response.acc.booked[tot-1].trainNo);
					printf("Number of seats: %d\n\n",response.acc.booked[tot-1].number_of_seats);
					tot--;
					bi++;
				}
				
				break;
				
				case UPDATE_BOOKING:
				
				printf("Enter the user id of customer:\n");
				scanf("%d",&UserId);
				cr.uid = UserId;
				cr.command = UPDATE_BOOKING;
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				printf("Bookings = %d \n",response.acc.totalBookings);
				int ti = 0;
				int booki=1;
				
				while(ti<response.acc.totalBookings)
				{
					printf("===========Booking number %d===========\n",booki);
					printf("Train no: %d\n",response.acc.booked[ti].trainNo);
					printf("Number of seats: %d\n\n",response.acc.booked[ti].number_of_seats);
					ti++;
					booki++;
				}
				
				printf("\nEnter the booking number to be updated:");
				scanf("%d",&updateindex);
				
				printf("\nEnter the new number of seats:");
				scanf("%d",&updatedSeats);
				
				
				cr.updateIndex = updateindex;
				cr.updatedSeats = updatedSeats;

				
				send(sockfd , &cr , sizeof(cr),0);
				read(sockfd,&response ,sizeof(struct serverResponse));

				if(response.commandStatus == 1) {	
					printf("Booking Updated Successfully!!!\n");
				}
				else
				{
					printf("Booking Updation Failed!!!\n");

				}
				
				break;
			 
			 case LOGOUT:
				
				cr.command = LOGOUT;
				send(sockfd , &cr , sizeof(cr),0);
			     exit(1);
			    break;
			
			
		}
	
	
	}
}


void AdminMenu(int sockfd, int adminid)
{
	int options;
	int UserId;
	int password;
	int accType;
	
	int trainNo;
	int num_of_seats;
	
	int newpass;
	int newaccType;
	int newtotalSeats;
	
	struct serverResponse response;
	int train_no;
	struct train train;
	struct bookingRequest breq;
	struct clientRequest cr;
	struct account newuser;
	struct account modUser;
	
	struct train modTrain;
	
	while(1)
	{
	printf("----------------------------------------------------------------------------------------------------------------------------------\n");
		printf("1.Search User\n2.Add User\n3.Delete User\n4.Modify User\n5.Search Train\n6.Add Train\n7.Delete Train\n8.Modify Train\n9.Logout");
        	printf("\nEnter choice:");
		scanf("%d",&options);		
		flushBuffer();
	
		switch(options)
		{
			case SEARCH_USER:
			
				printf("Enter User ID:\n");
				scanf("%d",&UserId);
				
				cr.uid = UserId;
				
				cr.command = SEARCH_USER; //search command
				
				send(sockfd , &cr , sizeof(cr),0);
				
				read(sockfd,&response ,sizeof(struct serverResponse));

				if(response.commandStatus==1)
				{
					printf("User Found\n");
				}
				else
				{
					printf("User not found\n");
				}
				break;
			
			
			case SEARCH_TRAIN:
			
				printf("Enter train number:\n");
				scanf("%d",&train_no);
				
				
				train.trainNo = train_no;
				//breq.trainNo = train_no;
				
				//search train
				cr.command = SEARCH_TRAIN; //search command
				cr.tr = train;
				//cr.bookReq = breq;
				send(sockfd , &cr , sizeof(cr),0);
				
				read(sockfd,&response ,sizeof(struct serverResponse));
				//printf("Search result: %d\n",response.commandStatus);
				if(response.commandStatus==1)
				{
					printf("Train found\n");
					printf("Train number %d\n",response.tr.trainNo);
					printf("Available seats %d\n",response.tr.totalSeats );
				}
				else
				{
					printf("Train not found\n");
				}
				break;
				
				case ADD_USER:
				
				printf("Enter User ID:\n");
				scanf("%d",&UserId); 
				printf("Enter Password:\n");
				scanf("%d",&password);
				printf("Enter account type(Agent (1)  Normal User(2) Admin(3) ):\n");
				scanf("%d",&accType);
				
				newuser.uid = UserId;
				newuser.pin = password;
				newuser.accType = accType;
				newuser.totalBookings=0;
				
				cr.uid = UserId;
				cr.newuser = newuser;
				cr.command = ADD_USER;
				
				send(sockfd , &cr , sizeof(cr),0);
				
				read(sockfd,&response ,sizeof(struct serverResponse));
				//printf("Search result: %d\n",response.commandStatus);
				if(response.newuserStatus ==1)
				{
					if(response.commandStatus == 1)
					{
						printf("User added successfully!!!\n");
					}
					else
					{
						printf("New user adding failed!!!\n");
					}
				}
				else
				{
					printf("User already exists!!!\n");
				}
				
				
				break;
				case DELETE_USER:
		
				printf("Enter User ID:\n");
				scanf("%d",&UserId); 
				
				cr.uid = UserId;
				cr.command = DELETE_USER;
		
				send(sockfd , &cr , sizeof(cr),0);
				
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				if(response.newuserStatus ==0)
				{
					if(response.commandStatus == 1)
					{
						printf("User deleted successfully!!!\n");
					}
					else
					{
						printf("User deletion failed!!!\n");
					}
				}
				else
				{
					printf("User does not exists!!!\n");
				}
		
				break;
				
				case ADD_TRAIN:
				
				printf("Enter Train number:\n");
				scanf("%d",&trainNo); 
				printf("Enter total number of seats:\n");
				scanf("%d",&num_of_seats);
				
				
				train.trainNo = trainNo;
				train.totalSeats = num_of_seats;
				train.totalBooked =0;
				
				cr.tr = train;
				cr.trainNo = trainNo;
				cr.command = ADD_TRAIN;
				
				send(sockfd , &cr , sizeof(cr),0);
				
				read(sockfd,&response ,sizeof(struct serverResponse));
				if(response.newtrainStatus ==1)
				{
					if(response.commandStatus == 1)
					{
						printf("Train added successfully!!!\n");
					}
					else
					{
						printf("New Train adding failed!!!\n");
					}
				}
				else
				{
					printf("Train already exists!!!\n");
				}
				
				
				break;
				
				case DELETE_TRAIN:
					
				printf("Enter Train Number:\n");
				scanf("%d",&trainNo); 
				
				cr.trainNo = trainNo;
				cr.command = DELETE_TRAIN;
		
				send(sockfd , &cr , sizeof(cr),0);
				
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				if(response.newtrainStatus ==0)
				{
					if(response.commandStatus == 1)
					{
						printf("Train deleted successfully!!!\n");
					}
					else
					{
						printf("Train deletion failed!!!\n");
					}
				}
				else
				{
					printf("Train does not exists!!!\n");
				}
		
				
				break;
				
				case MODIFY_USER:
					
				printf("Enter User ID:\n");
				scanf("%d",&UserId); 
				
				cr.uid = UserId;
				cr.command = MODIFY_USER;
		
				send(sockfd , &cr , sizeof(cr),0);
				
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				if(response.newuserStatus ==0)
				{
					printf("User Id: %d\n",response.acc.uid);
					printf("Password: %d\n",response.acc.pin);
					printf("Account type: %d\n",response.acc.accType);
					
					modUser = response.acc;
					printf("Enter the new data:\n");
					printf("Enter new password (Re enter old if you do not want to change!!!)\n");
					scanf("%d",&newpass);
					printf("Enter new account Type (Re enter old if you do not want to change!!!)\n");
					scanf("%d",&newaccType);
					
					modUser.pin = newpass;
					modUser.accType = newaccType;
					
					cr.newuser= modUser;
					send(sockfd , &cr , sizeof(cr),0);
					
					read(sockfd,&response ,sizeof(struct serverResponse));
					
					if(response.commandStatus == 1)
					{
						printf("User modified successfully\n");
					}
					else
					{
						printf("User modification failled\n");

					}
					
				}
				else
				{
					printf("User does not exists");
				}
				

				break;
				
				case MODIFY_TRAIN:
				
				printf("Enter Train number:\n");
				scanf("%d",&train_no); 
				
				cr.trainNo = train_no;
				cr.command = MODIFY_TRAIN;
		
				send(sockfd , &cr , sizeof(cr),0);
				
				read(sockfd,&response ,sizeof(struct serverResponse));
				
				if(response.newtrainStatus ==0)
				{
					printf("Train number: %d\n",response.tr.trainNo);
					printf("Total Seats: %d\n",response.tr.totalSeats);
					printf("Total booked: %d\n",response.tr.totalBooked);
					
					modTrain = response.tr;
					printf("Enter the new data:\n");
					printf("Enter new total seats (Re enter old if you do not want to change!!!)\n");
					scanf("%d",&newtotalSeats);
					
					modTrain.totalSeats = newtotalSeats;
					
					cr.tr= modTrain;
					send(sockfd , &cr , sizeof(cr),0);
					
					read(sockfd,&response ,sizeof(struct serverResponse));
					
					if(response.commandStatus == 1)
					{
						printf("Train modified successfully\n");
					}
					else
					{
						printf("Train modification failled\n");

					}
					
				}
				else
				{
					printf("Train does not exists");
				}
				
				
				break;
				
				case LOGOUT_ADMIN:
				
				cr.command = LOGOUT_ADMIN;
				send(sockfd , &cr , sizeof(cr),0);
			     exit(1);
			    break;
			
			
		}
	}
	
}