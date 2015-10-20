#include <semaphore.h>
#include "include.h"
#include "client_udp.h"

#define SERV_USER_ETC "/home/lh/linux-c/QQLan/serv-etc/user_etc.dat\n"

enum user_current_status user_log_status,user_resgis_status;
static int cli_fd = -1; //主要套接字，用于接受服务器各种消息
static int sock_fd = -1; //组播套接字，用于群聊

static struct sockaddr_in serv_addr; //用于存储服务器IP和端口
static char myname[20]; //用于存储用户名

//sem_t sem1,sem2;//定义信号量,用于线程同步

/*创建数据报套接字函数*/
int udp_link(void)
{
    int sock_fd;
    sock_fd = socket(AF_INET,SOCK_DGRAM,0);
    if(sock_fd < 0)
    {
        perror("socket udp_link:");
    }
    return sock_fd;      
}

/*用于给服务器发送数据的函数*/
void send_sig(char myname[],char desname[],char data[],struct sockaddr_in serv_addr,char ch)
{
    struct msg mymsg;
    mymsg.type = ch;
    char temp[MAXSIZE];
    if(myname != NULL)
    {
      my_strcut(temp,myname);
      strcpy(mymsg.self_name,temp);
    }

    if(desname != NULL)
    {
      my_strcut(temp,desname);
      strcpy(mymsg.dst_name, temp);
    }   
    if(data != NULL)
    {
        strcpy(mymsg.data,data);
    }
       
    if(sendto(cli_fd,&mymsg,sizeof(struct msg),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
        perror("sendto");
        exit(1);
    }
}
/*打印登陆菜单*/
void log_menuconfig(void)
{   
    printf("************************************************************\n");
    printf("***********************欢迎使用*****************************\n");
    printf("********************登录或注册山寨飞秋**********************\n");
    printf("***************用户名和密码不要超过8个字节******************\n");
    printf("************************************************************\n");
    printf("《a:@用户注册》\t《b:@登录》\t《c:@离开》\n");
    printf("请输入：(例如:b)\n");
    printf("************************************************************\n");
}

/*去'\n'函数*/
char* my_strcut (char * dst, const char *src)
{
    char *ptr = dst;

    while(*src != '\n')
    {
        *dst = *src;
        dst++;
        src++;
    }
    *dst = '\0';

    return ptr;
}
void myhandle(int signum)
{
    send_sig(myname,NULL,NULL,serv_addr,OFFLINE);
    printf("\n谢谢使用！\n再见！\n");
    close(cli_fd);
    close(sock_fd);
    exit(1);
}
/*私聊函数*/
void *chat_private(void* arg)
{
    char peer_name[20];
    char temp[20];
    char chat_data[MAXSIZE];

    bzero(&chat_data,sizeof(chat_data));
    bzero(&peer_name,sizeof(peer_name));
    bzero(&temp,sizeof(temp));

    printf("请选择聊天对象：\n");
    read(0,peer_name,10);
    my_strcut(temp,peer_name);
    printf("-----------------正在与《%s》聊天-------------------\n",temp);
    
    while(1)
    {
        printf("请输入聊天内容（按回车键发送）：\n");
        printf("-----输入quit退回主界面-----\n");
        fgets(chat_data,sizeof(chat_data),stdin);
        fflush(stdin);
        //fwrite(chat_data,sizeof(chat_data),1,cli_fp);
        if(strncmp(chat_data,"quit",4) == 0)
            break;// 终止while循环
        send_sig(myname,peer_name,chat_data,serv_addr,CHAT_PRI);  
        printf("--------------------\n");
    }
}
/*线程：接收群聊消息（广播）*/
void *chat_toall_recv(void* arg)
{
  // int ret = -1;
  // int num = -1;

  // struct msg rcv_buf;
  // /*获取本机IP 并分配一个端口*/

  // struct hostent *h_info;
  // struct in_addr **p_addr;
  // h_info = gethostbyname("ubuntu");
  // p_addr = ((struct in_addr **)(h_info->h_addr_list));

  // struct sockaddr_in self_addr;
  // memset(&self_addr,0,sizeof(self_addr));
  // self_addr.sin_family = AF_INET;
  // self_addr.sin_port = htons(17891);// 
  // self_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // struct ip_mreq group;
  // bzero(&group,sizeof(group));
  // group.imr_multiaddr.s_addr = inet_addr(GROUP_IP);
  // group.imr_interface = *(*p_addr);//
  
  // ret = bind(sock_fd,(struct sockaddr *)&self_addr, sizeof(self_addr));
  // if(ret < 0){
  //   perror("bind");
  //   exit(1);
  // }

  // ret = setsockopt(sock_fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&group,sizeof(group));
  // if(ret < 0){
  //   perror("setsockopt to ADD_MEMBERSHIP");
  //   exit(1);
  //}
 
  // while(1){
  //   //printf("等待接收群聊消息\n");
  //   ret = recvfrom(sock_fd,&rcv_buf,sizeof(rcv_buf),0,NULL,NULL);
  //   //printf("新的群聊消息\n");
  //   if(ret < 0){
  //     perror("recvfrom");
  //     exit(1);
  //   }
  //   rcv_buf.data[ret - sizeof(rcv_buf.type)-sizeof(rcv_buf.self_name)-sizeof(rcv_buf.dst_name)] = '\0';//
  //   printf("=====《群聊》消息来自<%s>:\n",rcv_buf.self_name);
  //   printf("--------------------------\n");
  //   printf("消息内容：%s",rcv_buf.data);
    
  // }

  int ret = -1;
  int opt = 1;
  int nb = 0;
  struct msg rcv_buf;
  struct sockaddr_in addrto;

  bzero(&addrto, sizeof(struct sockaddr_in) );
  addrto.sin_family = AF_INET;
  addrto.sin_addr.s_addr = htonl(INADDR_ANY);
  addrto.sin_port = htons(6000);
  
  // 广播地址
  struct sockaddr_in from;
  bzero(&from, sizeof(struct sockaddr_in));
  from.sin_family = AF_INET;
  from.sin_addr.s_addr = htonl(INADDR_ANY);
  from.sin_port = htons(6000);  

   //设置该套接字为广播类型，
  nb = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
  if(nb == -1)
  {
    perror("SO_BROADCAST:");
  }
  // 绑定地址
  if(bind(sock_fd,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1) 
  {   
   perror("bind:");
  }
   while(1){
    //printf("等待接收群聊消息\n");
    ret = recvfrom(sock_fd,&rcv_buf,sizeof(rcv_buf),0,NULL,NULL);
    //printf("新的群聊消息\n");
    if(ret < 0){
      perror("recvfrom");
      exit(1);
    }
    rcv_buf.data[ret - sizeof(rcv_buf.type)-sizeof(rcv_buf.self_name)-sizeof(rcv_buf.dst_name)] = '\0';//
    printf("=====《群聊》消息来自<%s>:\n",rcv_buf.self_name);
    printf("--------------------------\n");
    printf("消息内容：%s",rcv_buf.data);
    
  }

}

/*发送群聊消息，其实跟饲料消息基本一样的，消息类型不一样而已*/
void chat_toall(void)
{
  char chat_data[MAXSIZE];  
  bzero(&chat_data,sizeof(chat_data));
  printf("--------------------------------------------------------\n");
  printf("---------------<正在与所有在线用户聊天>-----------------\n");
  while(getchar() != '\n'); 
  while(1){
    printf("请输入聊天(群聊)内容（按回车键发送）：\n");
    printf("-----输入quit退回主界面-----\n");
    fgets(chat_data,sizeof(chat_data),stdin);

    if(strncmp(chat_data,"quit",4) == 0)
        break;
    send_sig(myname,NULL,chat_data,serv_addr,CHAT_ALL);
  }
}

/*打印主菜单*/
void show_opt(void)
{
    printf("《a：@私聊模式》\t《b：@群聊模式》\n《c：@刷新列表》\t《d：@下线离开》\n");
    printf("请选择（如：A）\n");
}

/*线程1，接收消息函数，接收各种由服务器发来的数据，根据数据类型分别处理*/
void *recv_chat_func(void *arg)
{
    int ret = -1;
    int i = 0;
    struct msg rcv_buf;
   
    while(1)
    {
        bzero(&rcv_buf,sizeof(rcv_buf));
        ret = recvfrom(cli_fd,&rcv_buf,sizeof(rcv_buf),0,NULL,NULL);
        if(ret < 0){
            perror("recvfrom");
            exit(1);
        }
        //printf("接收到消息，消息类型：%d\n内容：%s\n",rcv_buf.type,rcv_buf.data);
        rcv_buf.data[ret - sizeof(rcv_buf.type)-sizeof(rcv_buf.self_name)-sizeof(rcv_buf.dst_name)] = '\0';
        if(rcv_buf.type == REGISTER){
            printf("%s\n",rcv_buf.data);
            //sem_wait(&sem1);
            if(strcmp(rcv_buf.data,"Register Success") == 0)
            {
                user_resgis_status = REGISTER_SUCESS;
                printf("注册成功，请重新启动客户端，登录帐号\n");
            }
            else
            {
                user_resgis_status = REGISTER_FAILED;
                 printf("Status:%d 注册失败，请重试\n",user_resgis_status);
            }
            //sem_post(&sem2);
        }
        /*登录消息*/
        else if(rcv_buf.type == LOG_IN ){

            printf("%s\n",rcv_buf.data);
            //printf("message type:%d message data:%s\n",rcv_buf.type,rcv_buf.data);
            if(strcmp(rcv_buf.data,"Log success") == 0)
            {
                user_log_status = LOG_IN_SUCCESS;
                //printf("enum OK\n");
                //printf("user_log_status:%d\n",user_log_status);
           }
            else
            {
                user_log_status = LOG_IN_FAILED;
                printf("Status:%d 登录失败\n",user_log_status);
            }
        }
        /*刷新消息*/
        else if(rcv_buf.type == REFRESH){
            printf("@%s\t",rcv_buf.data);
            fflush(NULL);
        }
        /*发送结束消息*/
        else if(rcv_buf.type == OVER){
            printf("\n");
        }
        /*用户名重复提示消息*/
        else if(rcv_buf.type == ERROR){
            printf("重复登录，登录失败！\n");
            printf("%s",rcv_buf.data);
            exit(1);
        }
        /*聊天消息（私聊）*/
        else {  
            printf("新消息！===消息来自《%s》:\n",rcv_buf.self_name);
            printf("--------------------------\n");
            printf("消息内容：%s",rcv_buf.data);
            printf("(若要与%s聊天，请退回主界面重新选择私聊对象)\n",rcv_buf.self_name);

        }
		bzero(&rcv_buf,sizeof(rcv_buf));
    }
}

int main(void)
{
    
    struct msg mymsg;
    char ch;      // 主菜单选择按键
    int on = 1;
    int ret = 0;
    int error;
    char chat_data[MAXSIZE];
    char passwd[20]; // 用户密码
    char temp_buffer[MAXSIZE];

    //ret += sem_init(&sem1,0,1); //初始化信号量0
    //ret += sem_init(&sem2,0,0);
    
    pthread_t tid1;// 接收消息函数，接收各种由服务器发来的数据
    pthread_t tid2;// receive chat to all message
    pthread_t tid3;// private chat
    
    bzero(temp_buffer,sizeof(temp_buffer));
    bzero(&chat_data,sizeof(chat_data));

    bzero(&serv_addr,sizeof(struct sockaddr));
    serv_addr.sin_family = AF_INET; // 
    serv_addr.sin_addr.s_addr = inet_addr(SEVER_IP);
    serv_addr.sin_port = htons(SERVER_PORT);

    cli_fd = udp_link();
    sock_fd = udp_link();//组播套接字

    signal(SIGINT,myhandle);// Ctrl C 结束
    /*允许重复使用本地地址与套接字进行绑定*/
    ret = setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&on,sizeof(on));
    if(ret < 0){
        perror("setsockopt to SO_REUSEADDR");
        exit(1);
    }

    error = pthread_create (&tid1,NULL,recv_chat_func,NULL);
    if( error < 0){
        //perror("pthread_create1");
        fprintf(stderr,"Create tid1:%s",strerror(error));
        exit(1);
    }
    pthread_detach(tid1);
    if(pthread_create(&tid2,NULL,chat_toall_recv,NULL) < 0){
    perror("pthread_create2");
    exit(1);
  }
  pthread_detach(tid2);

    system("clear");
    log_menuconfig();
    scanf("%c",&ch);
    switch (ch)
    {
        case 'a':{
                     strcpy(chat_data,SERV_USER_ETC);// server filename
                     printf("Please input your name:\n");
                     read(0,myname,9);
                     my_strcut(temp_buffer,myname);
                     strcat(chat_data,temp_buffer); // myname into
                     //fflush(stdin);

                     printf("Please input your password:\n");
                     read(0,passwd,9);
                     my_strcut(temp_buffer,passwd);
                     //fflush(stdin);
                     strcat(chat_data,",");
                     strcat(chat_data,temp_buffer);// passwd into
                     strcat(chat_data,"\n");

                     send_sig(myname,NULL,chat_data,serv_addr,REGISTER);
                 }
                 break;
        case 'b':{
                     strcpy(chat_data,SERV_USER_ETC);// server filename
                     printf("Please input your name:\n");
                     read(0,myname,9);
                     my_strcut(temp_buffer,myname);
                     strcat(chat_data,temp_buffer); // myname into
                     //fflush(stdin);

                     printf("Please input your password:\n");
                     read(0,passwd,9);
                     my_strcut(temp_buffer,passwd);
                     //fflush(stdin);
                     strcat(chat_data,",");
                     strcat(chat_data,temp_buffer);// passwd into
                     strcat(chat_data,"\n");

                     send_sig(myname,NULL,chat_data,serv_addr,LOG_IN);
                 }
                 break;
        case 'c':
                 {printf("Exit Program!\n");
                     exit(1);}
                     break;
    }
    
    sleep(5);// wait user status changed

    if(user_log_status == LOG_IN_SUCCESS)
    {
        
        printf("当前在线用户：\n");
        send_sig(myname,NULL,NULL,serv_addr,REFRESH);
        usleep(200000);
        printf("========================================================\n");
        /*列出所有功能*/
        usleep(200000);
option:printf("\n********************************************************\n");
        printf("********************用户主界面**********************\n");
        printf("********************************************************\n");
        show_opt();
        scanf(" %c",&ch);
        printf("--------------------------------------------------------\n");

        switch(ch)
        {
            /*Chat private*/
            case 'a':{
                         if(pthread_create(&tid3,NULL,chat_private,NULL) < 0){
                             perror("pthread_create3");
                             exit(1);
                         }
                     }break;
            /*Chat to all*/
            case 'b':chat_toall();break;// main 
            /*flush ok */
            case 'c': {printf("当前在线用户：\n");
                          send_sig(myname,NULL,NULL,serv_addr,REFRESH);usleep(200000);
                          goto option;}break;  //发送刷新消息
            /*offline ok*/
            case 'd': {send_sig(myname,NULL,NULL,serv_addr,OFFLINE);
                          printf("下线成功！谢谢使用！\n再见\n");
                          exit(1);}break;

            default :{
                         printf("没有这个选项，请重新选择：\n");
                         while(getchar() != '\n');
                         goto option;
                     }

        }// end switch
        pthread_join(tid3,NULL);// break return main window
        goto option;
    }// end if
     return 0;
}

