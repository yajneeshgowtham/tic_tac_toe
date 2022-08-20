#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include<time.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.


char grid[9];

int check(){
	for(int i=0;i<9;i+=3){
		if((grid[i]==grid[i+1])&&(grid[i+1]==grid[i+2])){
			if(grid[i]=='O'){
				return 1;
			}
			else if(grid[i]=='X'){
				return 2;
			}
		}
	}
	for(int i=0;i<3;i+=1){
		if((grid[i]==grid[i+3])&&(grid[i+3]==grid[i+6])){
			if(grid[i]=='O'){
				return 1;
			}
			else if(grid[i]=='X'){
				return 2;
			}
		}
	}
	if((grid[0]==grid[4])&&(grid[4]==grid[8])){
		if(grid[0]=='O'){
			return 1;
		}
		else if(grid[0]=='X'){
			return 2;
		}
	}
	if((grid[2]==grid[4])&&(grid[4]==grid[6])){
		if(grid[2]=='O'){
			return 1;
		}
		else if(grid[2]=='X'){
			return 2;
		}
	}
	for(int i=0;i<9;i++){
		if(grid[i]=='_'){
			return 3;
		}
	}
	return 0;
}

void serverToClientWrite(int connfd,int state){
	char buff[MAX];
	bzero(buff,MAX);
	buff[0]='0'+state;
	write(connfd,buff,sizeof(buff));
}

void init(){
	for(int i=0;i<9;i++){
		grid[i]='_';
	}
}

int tictactoe(int connfd1,int connfd2,int sockfd,FILE* fptr){
	init();
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			fprintf(fptr,"_");
			fflush(fptr);
			if(j!=2){
				fprintf(fptr,"|");
				fflush(fptr);
			}
		}
		fprintf(fptr,"\n");
		fflush(fptr);
	}
	for(;;){
		char buff[MAX];
		bzero(buff,MAX);
		time_t t1,t2;
		time(&t1);
		if(read(connfd1,buff,sizeof(buff))==0){
			fprintf(fptr,"Player 1 got disconnected \n");
			fflush(fptr);
			write(connfd2,"exit",4);
			// close(sockfd);
			// exit(0);
			return 2;
		}
		time(&t2);
		double diff_t = difftime(t2, t1);
		if((int)diff_t>=15){
			fprintf(fptr,"Request Timed Out for player 1\n");
			fflush(fptr);
			fprintf(fptr,"Sending Request to Restart the Game\n");
			fflush(fptr);
			return -1;
		}
		fprintf(fptr,"Player 1 move is (ROW,COL):%c %c\n",buff[0],buff[2]);
		fflush(fptr);
		grid[(buff[0]-'1')*3+(buff[2]-'1')]='O';
		bzero(buff,MAX);
		for(int i=0;i<9;i++){buff[i]=grid[i];}
		fprintf(fptr,"The updated Game is :\n");
		fflush(fptr);
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				fprintf(fptr,"%c",grid[i*3+j]);
				fflush(fptr);
				if(j!=2){
					fprintf(fptr,"|");
					fflush(fptr);
				}
			}
			fprintf(fptr,"\n");
			fflush(fptr);
		}
		write(connfd1,buff,sizeof(buff));
		write(connfd2,buff,sizeof(buff));
		int bit=check();
		if(bit==1||bit==2){
			fprintf(fptr,"Player %d WON\n",bit);
			fflush(fptr);
		}
		if(bit==0){
			fprintf(fptr,"DRAW\n");
			fflush(fptr);
		}
		bzero(buff,MAX);
		buff[0]=bit+'0';
		write(connfd1,buff,sizeof(buff));
		write(connfd2,buff,sizeof(buff));
		if((bit==1)||(bit==2)||(bit==0)){
			break;
		}
		bzero(buff,MAX);
		time(&t1);
		if(read(connfd2,buff,sizeof(buff))==0){
			fprintf(fptr,"Player 2 got disconnected \n");
			fflush(fptr);
			write(connfd1,"exit",4);
			return 2;
			// close(sockfd);
			// exit(0);
		}
		time(&t2);
		diff_t = difftime(t2, t1);
		if((int)diff_t>=15){
			fprintf(fptr,"Request Timed Out for player 1\n");
			fflush(fptr);
			fprintf(fptr,"Sending Request to Restart the Game\n");
			fflush(fptr);
			return -1;
		}
		fprintf(fptr,"Player 2 move is (ROW,COL):%c %c\n",buff[0],buff[2]);
		fflush(fptr);
		grid[(buff[0]-'1')*3+(buff[2]-'1')]='X';
		bzero(buff,MAX);
		for(int i=0;i<9;i++){buff[i]=grid[i];}
		fprintf(fptr,"The updated Game is :\n");
		fflush(fptr);
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				fprintf(fptr,"%c",grid[i*3+j]);
				fflush(fptr);
				if(j!=2){
					fprintf(fptr,"|");
					fflush(fptr);
				}
			}
			fprintf(fptr,"\n");
			fflush(fptr);
		}
		write(connfd1,buff,sizeof(buff));
		write(connfd2,buff,sizeof(buff));
		bit=check();
		if(bit==1||bit==2){
			fprintf(fptr,"Player %d WON\n",bit);
			fflush(fptr);
		}
		if(bit==0){
			fprintf(fptr,"DRAW\n");
			fflush(fptr);
		}
		bzero(buff,MAX);
		buff[0]=bit+'0';
		write(connfd1,buff,sizeof(buff));
		write(connfd2,buff,sizeof(buff));
		if((bit==1)||(bit==2)||(bit==0)){
			break;
		}
	}
	return 0;
}

// Driver function
int main()
{
	int sockfd, connfd1, len1,connfd2,len2;
	struct sockaddr_in servaddr, cli1,cli2;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		exit(0);
	}
	bzero(&servaddr, sizeof(servaddr));
	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		exit(0);
	}
	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		exit(0);
	}
	FILE* fptr1=fopen("statistics.log","w");
	while(1){
		FILE* fptr=fopen("statistics.log","a+");
		fprintf(fptr,"=========================================================================\n");
		fflush(fptr);
		printf("Game server started. Waiting for players.\n");
		fprintf(fptr,"Game server started. Waiting for players.\n");
		fflush(fptr);
		// printf("wnwhewhfidhfo\n");
		len1 = sizeof(cli1);
		len2 = sizeof(cli2);
		// Accept the data packet from client and verification
		time_t t,start_t,end_t,t1,t2;
		connfd1 = accept(sockfd, (SA*)&cli1, &len1);
		// not a primitive datatype
		time(&t);
		fprintf(fptr,"Player 1 got Connected at time : %s\n",ctime(&t));
		fflush(fptr);
		if (connfd1 < 0) {
			exit(0);
		}
		serverToClientWrite(connfd1,1);
		connfd2=accept(sockfd,(SA*)&cli2,&len2);
		time(&t);
		fprintf(fptr,"Player 2 got Connected at time : %s\n",ctime(&t));
		fflush(fptr);
		if(connfd2<0){
			exit(0);
		}
		serverToClientWrite(connfd1,1);
		serverToClientWrite(connfd2,2);
		int itr=1;
		for(;;){
			fprintf(fptr,"!!!!ROUND %d STATISTICS!!!!\n",itr);
			fflush(fptr);
			fprintf(fptr,"!!!ROUND %d STARTS FIGHT!!!\n",itr);
			fflush(fptr);
			itr+=1;
			time(&start_t);
			fprintf(fptr,"Game started at : %s\n",ctime(&start_t));
			fflush(fptr);
			int result=tictactoe(connfd1,connfd2,sockfd,fptr);
			if(result<0){
				write(connfd1,"5",1);
				write(connfd2,"5",1);
			}
			else if(result>0){
				break;
			}
			time(&end_t);
			fprintf(fptr,"Game ended at : %s\n",ctime(&end_t));
			fflush(fptr);
			double diff_t = difftime(end_t, start_t);
			fprintf(fptr,"Game time is %f seconds\n",diff_t);
			fflush(fptr);
			char buff1[MAX];
			bzero(buff1,MAX);
			if((read(connfd1,buff1,sizeof(buff1)))==0){
				write(connfd2,"exit",4);
				fprintf(fptr,"Player 1 got disconnected\n");
				fflush(fptr);
				fprintf(fptr,"Stopping the Game...\n");
				fflush(fptr);
				break;
			}
			char buff2[MAX];
			bzero(buff2,MAX);
			if((read(connfd2,buff2,sizeof(buff2)))==0){
				write(connfd1,"exit",4);
				fprintf(fptr,"Player 2 got disconnected\n");
				fflush(fptr);
				fprintf(fptr,"Stopping the Game...\n");
				fflush(fptr);
				break;
			}
			int res1=strncmp(buff1,"YES",3);
			int res2=strncmp(buff2,"YES",3);
			int res;
			if(res1==0&&res2==0){res=1;}
			else{res=0;}
			fprintf(fptr,"Player 1 response for Rematch : %s\n",buff1);
			fflush(fptr);
			fprintf(fptr,"Player 2 response for Rematch : %s\n",buff2);
			fflush(fptr);
			char buff[MAX];
			bzero(buff,MAX);
			buff[0]=res+'0';
			write(connfd1,buff,sizeof(buff));
			write(connfd2,buff,sizeof(buff));
			if(res==0){
				fprintf(fptr,"Disconnecting the Game\n");
				fflush(fptr);
				break;
			}
			fprintf(fptr,"!!!ROUND %d ENDS!!!\n",itr-1);
			fflush(fptr);
		}
		fprintf(fptr,"=========================================================================\n");
		fflush(fptr);
		fclose(fptr);
   }
	close(sockfd);

}