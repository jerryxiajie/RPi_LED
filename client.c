#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc,char *argv[])
{
    int sockfd,numbytes;
    char buf[256];
    struct sockaddr_in their_addr;
    printf("break!");
    while((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1);
    printf("We get the sockfd~\n");
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(9527);
    their_addr.sin_addr.s_addr=inet_addr("192.168.2.88");
    bzero(&(their_addr.sin_zero), 8);
    
    while(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) == -1);
    printf("Get the Server~Cheers!\n");
//    numbytes = recv(sockfd, buf, 20,0);//接收服务器端信息  

//    bzero(buf,256);
//    printf("Entersome thing:");
//    scanf("%s",buf);
//    numbytes = send(sockfd, buf, strlen(buf), 0);

    bzero(buf,256);
    while((numbytes = recv(sockfd, buf, 255, 0)) > 0)
    {
	printf("\n numbytes = %d \n", numbytes);
	buf[numbytes]='\0';
	printf("%s\n", buf);
	
    	close(sockfd);
    }

    close(sockfd);
    return 0;
}
