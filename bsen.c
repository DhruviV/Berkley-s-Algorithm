

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define TEST_PORT 12345
#define GROUP "228.0.0.5"
#define MSGBUFSIZE 256



int berkley(int a, int b[], int y)
{
     int c=0;
     int i=0;

     c=c+a;

     for(i=0;i<(y-1);i++)
     {
          c=c+b[i];
     }

     c=c/y;
     return c;
}

int main(int argc, char *argv[])
{
     struct sockaddr_in addr;
     int fd, fd1, fd2, cnt, addrlen, nbytes, i=0,j=0,k=0;
     struct ip_mreq mreq;
     char *message="Hello";
     int x=3;
     int recieve_clock=0;
     int clock=0,store_clock[x], result_clock=4,result=0;
     time_t t;
     u_int yes=1;


     srand((unsigned) time(&t));

     clock=rand()%50;
     printf("This is my clock: %d\n", clock);
     sleep(1);


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
     
     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=inet_addr(GROUP);
     addr.sin_port=htons(TEST_PORT);
     
     while (i<(x-1)) {
	  if (sendto(fd,message,sizeof(message),0,(struct sockaddr *) &addr,
		     sizeof(addr)) < 0) {
	       perror("sendto");
            
	       exit(1);
	  }
       i++;
      
     }

    if (setsockopt(fd1,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }
    if (setsockopt(fd1,SOL_SOCKET,SO_REUSEPORT,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }

     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=htonl(INADDR_ANY); 
     addr.sin_port=htons(TEST_PORT);
     
    if (bind(fd1,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
       perror("bind");
       exit(1);
     }
     
     mreq.imr_multiaddr.s_addr=inet_addr(GROUP);
     mreq.imr_interface.s_addr=htons(INADDR_ANY);
     if (setsockopt(fd1,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
       perror("setsockopt");
       exit(1);
     }
     
      while(j<(x-1))
    {
       addrlen=sizeof(addr);

       if ((nbytes=recvfrom(fd1,&recieve_clock,sizeof(recieve_clock),0,(struct sockaddr *) &addr,&addrlen)) < 0) 
      {
            perror("recvfrom");
            exit(1);
       }
       store_clock[j]= ntohl(recieve_clock);
       
       printf("the clock is %d\n", store_clock[j]);
       j++;

      sleep(1);
     }
     if (setsockopt(fd1,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
      perror("setsockopt");
      exit(1);
     }
     

    result_clock=berkley(clock,store_clock,x);

    
     sleep(5);

     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=inet_addr(GROUP);
     addr.sin_port=htons(TEST_PORT);
    
     
     while (k<(x-1)) {
        result=htonl(result_clock);  
       if (sendto(fd2,&result,sizeof(result),0,(struct sockaddr *) &addr,
               sizeof(addr)) < 0) {
            perror("sendto");
            
            exit(1);
       }
       k++;
       
     }


     printf("clocks\n");
     for(i=0;i<(x-1);i++)
     {
          printf("%d\n", store_clock[i]);
     }
     printf("%d\n", clock);
     printf("synchronised clock: %d\n", result_clock);

     return 0;
}
