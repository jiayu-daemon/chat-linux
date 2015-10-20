#ifndef __CLIENT_H
#define __CLIENT_H


enum user_current_status
{
    REGISTER_SUCESS = 1,
    REGISTER_FAILED,
    LOG_IN_SUCCESS,
    LOG_IN_FAILED,
};

int udp_link(void);
void send_sig(char myname[],char desname[],char data[],struct sockaddr_in serv_addr,char ch);
void myhandle(int signum);
void log_menuconfig(void);
char *my_strcut (char * dst, const char *src);
void *chat_private(void *arg);
void *recv_chat_func(void *arg);
void *chat_toall_recv(void *arg);
void show_opt(void);

#endif
