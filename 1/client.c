#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr


char grid[9]={'_','_','_','_','_','_','_','_','_'};

int ServerToClientRead(int sockfd){
	char buff[MAX];
	bzero(buff, sizeof(buff));
	if(read(sockfd,buff,sizeof(buff))==0){
		printf("!!!!SERVER INTERRUPT STOPPING THE CLIENTS!!!!\n");
		close(sockfd);
		exit(0);
	}
	if(strncmp(buff,"exit",4)==0){
		printf("Sorry your partner got disconnected!!!\n");
		close(sockfd);
		exit(0);
	}
	char c=buff[0];
	return c-'0';
}

void readFromUser(int sockfd){
	char buff[MAX];
	int cond=0;
	while(!cond){
		printf("Enter (ROW, COL) for placing your mark: ");
		bzero(buff, MAX);
		int n=0;
		while ((buff[n++] = getchar()) != '\n');
		int i=buff[0]-'1';
		int j=buff[2]-'1';
		if (i>=0&&i<=2&&j>=0&&j<=2&&grid[3*i+j]=='_'){
			cond=1;
		}
		if(cond==0){
			printf("Illegal Entry !!! \n");
		}

	}
	write(sockfd, buff, sizeof(buff));

}

int  readFromServer(int sockfd){
	char buff[MAX];
	bzero(buff, sizeof(buff));
	if(read(sockfd,buff,sizeof(buff))==0){
		printf("!!!!SERVER INTERRUPT STOPPING THE CLIENTS!!!!\n");
		close(sockfd);
		exit(0);
	}
	if(strncmp(buff,"exit",4)==0){
		printf("Sorry your partner got disconnected!!!\n");
		close(sockfd);
		exit(0);
	}
	if(strncmp(buff,"5",1)==0){
		return -1;
	}
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			grid[i*3+j]=buff[i*3+j];
			printf("%c",buff[i*3+j]);
			if(j!=2){
				printf("|");
			}
		}
		printf("\n");
	}
	return 0;
}


int checkRes(int sockfd){
	char buff[MAX];
	bzero(buff, sizeof(buff));
	if(read(sockfd,buff,sizeof(buff))==0){
		printf("!!!!SERVER INTERRUPT STOPPING THE CLIENTS!!!!\n");
		close(sockfd);
		exit(0);
	}
	if(strncmp(buff,"exit",4)==0){
		printf("Sorry your partner got disconnected!!!\n");
		close(sockfd);
		exit(0);
	}
	if(buff[0]=='1'){
		printf("Player 1 Wins\n");
	}
	else if(buff[0]=='2'){
		printf("Player 2 Wins\n");
	}
	else if(buff[0]=='0'){
		printf("Draw\n");

	}
	return buff[0]-'0';
}


int play(int id,int sockfd,int itr){
	if(id==1){
		if(itr>=1){
			printf("Your Player ID is 1. Your symbol is ‘O'. \n");
		}
		printf("Starting the game …\n");
		printf("_|_|_\n");
		printf("_|_|_\n");
		printf("_|_|_\n");
		for(;;){
			readFromUser(sockfd);
			if(readFromServer(sockfd)<0){
				return -1;
			}
			int bit=checkRes(sockfd);
			if(bit==1||bit==2||bit==0){
				break;
			}
			readFromServer(sockfd);
			bit=checkRes(sockfd);
			if(bit==1||bit==2||bit==0){
				break;
			}
		}
		return 0;
	}
	else if(id==2){
		if(itr>=1){
			printf("Your Player ID is 2. Your symbol is ‘X'. \n");
		}
		printf("Starting the game …\n");
		printf("_|_|_\n");
		printf("_|_|_\n");
		printf("_|_|_\n");
		for(;;){
			if(readFromServer(sockfd)<0){
				return -1;
			}
			int bit=checkRes(sockfd);
			if(bit==1||bit==2||bit==0){
				break;
			}
			readFromUser(sockfd);
			readFromServer(sockfd);
			bit=checkRes(sockfd);
			if(bit==1||bit==2||bit==0){
				break;
			}
		}
		return 0;
	}
	return 1;
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		exit(0);
	}
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		exit(0);
	}
	// function for chat)
	int id=ServerToClientRead(sockfd);
	if(id==1){
		printf("Connected to the game server. Your player ID is 1. Waiting for a partner to join . . .\n");
		ServerToClientRead(sockfd);
		printf("Your partner's ID is 2. Your symbol is ‘O’.\n");
		int itr=0;
		for(;;){
			if(play(id,sockfd,itr)<0){
				printf("Request Time Out!!!\n");
			}
			printf("Do you want to play another Game??? Type YES!!!\n");
			char buff[MAX];
			bzero(buff, MAX);
			int n=0;
			while ((buff[n++] = getchar()) != '\n');
			write(sockfd,buff,sizeof(buff));
			bzero(buff, MAX);
			if(read(sockfd,buff,sizeof(buff))==0){
				printf("!!!!SERVER INTERRUPT STOPPING THE CLIENTS!!!!\n");
				close(sockfd);
				exit(0);		
			}
			if(strncmp(buff,"exit",4)==0){
				printf("Sorry your partner got disconnected!!!\n");
				close(sockfd);
				exit(0);
			}
			if(buff[0]=='0'){printf("The Game is being Disconnected\n");break;}
			for(int i=0;i<9;i++){
				grid[i]='_';
			}
			itr++;
		}
	
	}
	else if(id==2){
		printf("Connected to the game server. Your player ID is 2. Your partner’s ID is 1. Your symbol is ‘X’\n");
		int itr=0;
		for(;;){
			if(play(id,sockfd,itr)<0){
				printf("Request Timed Out!!!\n");
			}
			printf("Do you want to play another Game??? Type YES!!!\n");
			char buff[MAX];
			bzero(buff, MAX);
			int n=0;
			while ((buff[n++] = getchar()) != '\n');
			write(sockfd,buff,sizeof(buff));
			bzero(buff, MAX);
			if(read(sockfd,buff,sizeof(buff))==0){
				printf("!!!!SERVER INTERRUPT STOPPING THE CLIENTS!!!!\n");
				close(sockfd);
				exit(0);
			}
			if(strncmp(buff,"exit",4)==0){
				printf("Sorry your partner got disconnected!!!\n");
				exit(0);
			}
			if(buff[0]=='0'){printf("The Game is being Disconnected\n");break;}
			for(int i=0;i<9;i++){
				grid[i]='_';
			}
			itr++;
		}
		
	}
	
	// close the socket
	close(sockfd);
}