#include <stdio.h>
#include <stdlib.h>
#include<netdb.h>
#include<errno.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<signal.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<dirent.h>

#define SERVER_IP "10.0.0.139"
#define SERVER_PORT 8000
#define GROUP_IP "230.10.10.10"
#define GROUP_PORT 17891
#define MAXSIZE 1024

int sock_fd;
struct sockaddr_in serv_addr;

void *chat_toall_recv(void* arg)
{
    int ret = -1;
    int num = -1;

    char buffer[MAXSIZE];
    bzero(buffer,sizeof(buffer));

    /*获取本机IP 并分配一个端口*/
    struct hostent *h_info;
    struct in_addr **p_addr;
    h_info = gethostbyname("ubuntu");
    p_addr = ((struct in_addr **)(h_info->h_addr_list));

    struct sockaddr_in self_addr;
    memset(&self_addr,0,sizeof(self_addr));
    self_addr.sin_family = AF_INET;
    self_addr.sin_port = htons(17891);// 
    self_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    struct ip_mreq group;
    bzero(&group,sizeof(group));
    group.imr_multiaddr.s_addr = inet_addr(GROUP_IP);
    group.imr_interface = *(*p_addr);//

    ret = bind(sock_fd,(struct sockaddr *)&self_addr, sizeof(self_addr));
    if(ret < 0){
        perror("bind");
        exit(1);
    }

    ret = setsockopt(sock_fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&group,sizeof(group));
    if(ret < 0){
        perror("setsockopt to ADD_MEMBERSHIP");
        exit(1);
    }

    while(1)
    {
        ret = recvfrom(sock_fd,buffer,sizeof(buffer),0,NULL,NULL);
        if(ret <0)
        {
            perror("recvfrom:");
        }
        printf("Recv Message:%s\n",buffer);
    }
}
void *chat_toall(void *arg)
{
   
    char buffer[MAXSIZE];
    bzero(buffer,sizeof(buffer));

    while(1)
    {
        printf("--------quit----------\n");
        fgets(buffer,sizeof(buffer+1),stdin);

        if(strncmp (buffer,"quit",4) == 0)
        {
            break;
        }
        if(sendto(sock_fd,buffer,sizeof(buffer),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        {
            perror("Send:");
        }
    }
}

int main()
{
    int error;
    pthread_t tid1;
    pthread_t tid2;

    bzero(&serv_addr,sizeof(struct sockaddr));
    serv_addr.sin_family = AF_INET; // 
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_addr.sin_port = htons(SERVER_PORT);

    sock_fd = socket(AF_INET,SOCK_DGRAM,0);

    if(sock_fd < 0)
    {
        perror("socket:");
    }

    error = pthread_create (&tid1,NULL,chat_toall_recv,NULL);
    if( error < 0){
        perror("pthread_create1");
        fprintf(stderr,"Create tid1:%s",strerror(error));
        exit(1);
    }
    error = pthread_create (&tid2,NULL,chat_toall,NULL);
    if( error < 0){
        perror("pthread_create2");
        fprintf(stderr,"Create tid2:%s",strerror(error));
        exit(1);
    }

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    return 0;
}
