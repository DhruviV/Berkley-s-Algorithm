

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define TEST_PORT 12345
#define GROUP "228.0.0.5"
//#define GROUP1 "228.0.0.4"
#define MSGBUFSIZE 256

int main(int argc, char *argv[])
{
     struct sockaddr_in addr;
     int fd, fd1, fd2, nbytes,addrlen, result=0, cnt, i=0,j=0,k=0;
     struct ip_mreq mreq;
     char msgbuf[MSGBUFSIZE];
     int send_clock=0;
     int clock=0;
     time_t t;
     u_int yes=1;


     srand((unsigned) time(&t));

     clock=rand()%50;
     printf(" Hello, This is my clock: %d\n", clock);


     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }
     if ((fd1=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
      perror("socket");
      exit(1);
     }
     if ((fd2=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
      perror("socket");
      exit(1);
     }


    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEPORT,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }

     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=htonl(INADDR_ANY);
     addr.sin_port=htons(TEST_PORT);
     
     if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
	  perror("bind");
	  exit(1);
     }
     
     mreq.imr_multiaddr.s_addr=inet_addr(GROUP);
     mreq.imr_interface.s_addr=htons(INADDR_ANY);
     if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
	  perror("setsockopt");
	  exit(1);
     }

      while(i<1)
    {
	  addrlen=sizeof(addr);
	  if ((nbytes=recvfrom(fd,msgbuf,MSGBUFSIZE,0,(struct sockaddr *) &addr,&addrlen)) < 0) 
      {
	       perror("recvfrom");
	       exit(1);
	  }
      i++;
	  puts(msgbuf);
      sleep(1);
     }
     if (setsockopt(fd,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
      perror("setsockopt");
      exit(1);
     }

     sleep(5); 

     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=inet_addr(GROUP);
     addr.sin_port=htons(TEST_PORT);

     while (j<1)
     {
        
        send_clock = htonl(clock);
      if (sendto(fd1,&send_clock,sizeof(send_clock),0,(struct sockaddr *) &addr,
             sizeof(addr)) < 0) {
           perror("sendto");
            
           exit(1);
      }
       j++;
       
      
     }

     
    if (setsockopt(fd2,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }
    if (setsockopt(fd2,SOL_SOCKET,SO_REUSEPORT,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }

     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=htonl(INADDR_ANY); 
     addr.sin_port=htons(TEST_PORT);
     

     if (bind(fd2,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
      perror("bind");
      exit(1);
     }
     

     mreq.imr_multiaddr.s_addr=inet_addr(GROUP);
     mreq.imr_interface.s_addr=htons(INADDR_ANY);
     if (setsockopt(fd2,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
      perror("setsockopt");
      exit(1);
     }
     sleep(2);

      while(k<1)
    {
      addrlen=sizeof(addr);
      if ((nbytes=recvfrom(fd2,&result,sizeof(result),0,(struct sockaddr *) &addr,&addrlen)) < 0) 
      {
           perror("recvfrom");
           exit(1);
      }
      k++;
      clock=ntohl(result);
      printf("the synchronised clock is %d\n", clock);

      sleep(1);
     }
     if (setsockopt(fd2,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
      perror("setsockopt");
      exit(1);
     }

    

     return 0;

}
