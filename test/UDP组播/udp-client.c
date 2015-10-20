#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 255
 
int main (int argc, char **argv) 
{
  
    struct sockaddr_in peeraddr, myaddr; 
    int sockfd;  
    char recmsg[BUFLEN + 1];
    unsigned int socklen;
    socklen = sizeof (struct sockaddr_in);  

    /* 创建 socket 用于UDP通讯 */ 
    sockfd = socket (AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {      
        printf ("socket creating error\n");     
        exit (1);   
    }  
  
    /* 设置对方的端口和IP信息 */ 
    memset (&peeraddr, 0, socklen); 
    peeraddr.sin_family = AF_INET;  
    peeraddr.sin_port = htons (7838);
    inet_pton (AF_INET, "224.0.1.2", &peeraddr.sin_addr);

  
/* 设置自己的端口和IP信息 */ 
    memset (&myaddr, 0, socklen); 
    myaddr.sin_family = AF_INET; 
    myaddr.sin_port = htons (23456); 
    myaddr.sin_addr.s_addr = INADDR_ANY;
  
    /* 绑定自己的端口和IP信息到socket上 */ 
    if (bind (sockfd, (struct sockaddr *) &myaddr,     sizeof (struct sockaddr_in)) == -1)
    {     
        printf ("Bind error\n");     
        exit (0);  
    }
  
    /* 循环接受用户输入的消息发送组播消息 */ 
    for (;;)
    {      
        /* 接受用户输入 */ 
        bzero (recmsg, BUFLEN + 1);     
        if (fgets (recmsg, BUFLEN, stdin) == (char *) EOF)    
            exit (0);
          
        /* 发送消息 */ 
        if (sendto(sockfd, recmsg, strlen (recmsg), 0, (struct sockaddr *) &peeraddr, sizeof (struct sockaddr_in)) < 0)
        {      
            printf ("sendto error!\n");      
            exit (3);    
        }
          
        printf ("'%s' send ok\n", recmsg);
    } 
}