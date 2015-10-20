#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>

#define SERVER_IP "10.0.0.139"
#define SERVER_PORT 8000
#define GROUP_IP "230.10.10.10"
#define GROUP_PORT 17891
#define MAXSIZE 1024

int sock_fd;

void* recv_func (void *arg)
{
    struct sockaddr_in cli_addr;
    int ret;
    char buffer[MAXSIZE];
    socklen_t addrlen = sizeof(struct sockaddr_in);
    while(1)
    {

        ret = recvfrom(sock_fd,buffer,sizeof(buffer),0,(struct sockaddr *)&cli_addr,&addrlen);
        if(ret < 0)
        {
            perror("recvfrom");
            exit(1);
        }
        printf("Recv Message from Client:%s\n",buffer);

    }

}

void *send_toall_func(void *arg)
{
    int msglen = -1;
    socklen_t addrlen = -1;
    int num = -1;
    struct sockaddr_in msgbuf_tmp; //组播

    char buffer[MAXSIZE];

    bzero(&msgbuf_tmp,sizeof(struct sockaddr_in));
    msgbuf_tmp.sin_family = AF_INET;
    msgbuf_tmp.sin_port = htons(GROUP_PORT);//
    /* 设置发送组播消息的源主机的地址信息 */
    msgbuf_tmp.sin_addr.s_addr = inet_addr(GROUP_IP);

    addrlen = sizeof(msgbuf_tmp);
    num = sendto(sock_fd,buffer,sizeof(buffer),0,(struct sockaddr *)&msgbuf_tmp,addrlen);
    if(num < 0){
        perror("sendto");
        exit(1);
    }

    printf("Send to all:%s\n",buffer);
}

int main()
{
    struct sockaddr_in self_addr; //服务器自己的地址
    int on = 1;
    int ret = -1;

    pthread_t tid1;
    pthread_t tid2;

    bzero(&self_addr,sizeof(struct sockaddr));
    /*设置服务器IP地址和端口*/ 
    self_addr.sin_family = AF_INET;
    self_addr.sin_port = htons(SERVER_PORT);
    self_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    sock_fd = socket(AF_INET,SOCK_DGRAM,0);

    ret = setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&on,sizeof(on));
    if(ret < 0){
        perror("setsockopt to SO_REUSEADDR");
        exit(1);
    }

    if(bind(sock_fd,(struct sockaddr *)&self_addr,sizeof(struct sockaddr)) < 0){
        perror("bind");
        exit(1);
    }
    printf("*******欢迎使用********\n");
    printf("服务器正在运行……\n");
    if(pthread_create(&tid1, NULL, recv_func,NULL ) < 0){
        perror("pthread1_create");
        exit(1);
    }
    if(pthread_create(&tid2, NULL, send_toall_func,NULL ) < 0){
        perror("pthread2_create");
        exit(1);
    }

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
}
