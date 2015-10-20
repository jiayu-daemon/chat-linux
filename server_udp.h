#ifndef __SERVER_H
#define __SERVER_H

int udp_link(void);
void insert(struct servmsg **last,struct msg buf,struct sockaddr_in cli_addr);/*将收到的所有消息插入链表尾部*/
void add_user(struct user_info *head,struct servmsg *user);/*若为登录消息，则将其消息插入用户链表*/
void *send_register_info(struct sockaddr_in addr,const char *src_data);/*发送注册信息到客户端 @jiayu*/
void *send_log_info(struct sockaddr_in addr,const char *src_data);/*发送登录信息到客户端 @jiayu*/
void *send_all_online(struct sockaddr_in addr);/*发送在线用户列表至客户端*/
void  delete_user(char name[]);/*用户下线，则将其从链表中删除*/
void register_user();/*@jiayu*/
void check_user();/*@jiayu*/
void *pthread_func();/*解析消息线程*/
void myhandle(int signum);

#endif