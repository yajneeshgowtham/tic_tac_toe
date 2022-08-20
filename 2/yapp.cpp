#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <ctime>
#include <iostream>
#include <chrono> 
#include<poll.h>
#include<sys/time.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define TIME 20
#define PING_PKT_S 64
#define TRUE 1
#define FALSE 0


using namespace std;


struct packet
{
	struct icmphdr hdr;
	char msg[PING_PKT_S-sizeof(struct icmphdr)];
};


class yapp
{
    private:
        struct packet p;
        struct sockaddr_in dst,src;
        struct pollfd PFD;
        string ipaddress;
        int sockfd;
        int sz;
    public:
        void init(){
            bzero(&p,sizeof(p));
            p.hdr.type=ICMP_ECHO;
            ipaddress="";
            sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
            sz=sizeof(src);
            PFD.events=POLLIN;
            dst.sin_family = AF_INET;
            dst.sin_port = htons(PORT);
        }
        int valsockfd(){
            PFD.fd=sockfd;
            return sockfd;
        }
        bool ip(char* str){
            struct in_addr address;
            if(inet_aton(str,(struct in_addr*)&dst.sin_addr.s_addr)==0){
                return FALSE;
            }
            ipaddress=inet_ntoa(address);
            return TRUE;
        }
        void sendping(){
            struct timeval start;
            struct timeval end;
	        gettimeofday(&start, NULL);
            if(sendto(sockfd,&p,sizeof(p),0,(struct sockaddr*)&dst,sizeof(dst))<0){
                exit(0);
            }
            if(poll((struct pollfd*)&PFD,1,20*1000)==0){
                cout<<"Request Timed Out or Host Unreacheable"<<endl;
                exit(0);
            }
            if(recvfrom(sockfd,&p,sizeof(p),0,(struct sockaddr*)&src,(socklen_t*)&sz)<0){
                exit(0);
            }
            gettimeofday(&end , NULL);
            double ms1 , ms2 , time;    
            ms1 = (double)start.tv_sec*1000000 + (double)start.tv_usec;
            ms2 = (double)end.tv_sec*1000000 + (double)end.tv_usec;
            time = (double)ms2 - (double)ms1;
            time=(double)time/(double)1000;
            cout<<"Round Trip Time is : "<<time<<" ms"<<endl;
        }

};


int main(int argc,char* argv[]){
    yapp y;
    y.init();
    if(argc!=2){
        printf("Please enter the IP address\n");
        exit(0);
    }
    if(y.valsockfd()<0){
        exit(0);
    }
    if(y.ip(argv[1])==FALSE){
        cout<<"Bad Host Name"<<endl;
        exit(0);
    }
    y.sendping();
    return 0;
}