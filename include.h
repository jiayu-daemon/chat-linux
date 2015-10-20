/*
Linux下局域网通讯软件
传输层协议：UDP
功能:注册，登陆，私聊，群聊，在线列表，下线，重复登陆提示
作者:@jiayu
运行环境：Linux(ubunt10.04下测试)
2014年11月8日20:16:10
 */
 
#include<stdio.h>
#include<sys/types.h>          /* See NOTES */
#include<sys/socket.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
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

#define MAXSIZE 512
#define SEVER_IP "10.0.0.150"
#define SERVER_PORT 8000 
#define GROUP_IP "230.10.10.10"
#define GROUP_PORT 17891
#define REGISTER 1 //
#define LOG_IN 2
#define REFRESH 3 //要求刷新用户在线用户（重新打印在线用户）
#define CHAT_PRI 4
#define CHAT_ALL 5
#define DOWNLOAD 6 //从服务器下载文件（下载之前先打印文件列表）
#define UPLOAD 7 //上传文件到服务器
#define OFFLINE 8 //下线通知
#define OVER 9 //服务器发送本次消息结束
#define ERROR 10 //重复登录
#define FILE_NAME 11 //发送文件列表
#define FILE_ERROR 12 //选择文件名失败

/*服务器接收消息后，创建的在线用户列表*/
struct user_info{
       char user_name[20];
       int  id;
       struct sockaddr_in cli_addr;
       struct user_info *next;
};
/*客户端给服务器发送的消息*/
struct msg {
       char type;
       char self_name[20];
       char dst_name[20];
       char data[MAXSIZE];
};
/*服务器总列表*/
struct servmsg
{
       struct msg recvmsg;
       struct sockaddr_in addr;
       struct servmsg *next;
};