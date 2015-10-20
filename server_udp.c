#include "include.h"
#include "server_udp.h"


static struct sockaddr_in self_addr; //服务器自己的地址
static struct sockaddr_in cli_addr; //客户端地址

static int serv_fd;//主要套接字,用来接收客户端发来的各种消息
static int i=100; //用户id,从100开始（本程序里未使用）
static int find = 0;//标记用户是否成功登录，登录成功设置为1，失败设置为0 

static struct servmsg *H;    //服务器主线程，将接收的每个消息，建立成链表,H为链表头
static struct user_info *head_user;  //处理线程建立的用户链表头
static struct msg cli_msgbuf;       //用于存发来消息的结构体

/*建立UDP套接字*/
int udp_link(void)
{
    int sock_fd;
    sock_fd = socket(AF_INET,SOCK_DGRAM,0);// IPV4,UDP
    return sock_fd;     
}
/*将收到的所有消息插入链表尾部*/
void insert(struct servmsg **last,struct msg buf,struct sockaddr_in cli_addr)
{
    if((*last)->next == NULL )
    {
        (*last)->next = (struct servmsg *)malloc(sizeof(struct servmsg));// 
        (*last) = (*last)->next;
        (*last)->next = NULL;

        (*last)->recvmsg.type = buf.type;
        (*last)->addr = cli_addr;
        strcpy((*last)->recvmsg.self_name, buf.self_name);
        strcpy((*last)->recvmsg.dst_name,buf.dst_name);
        strncpy((*last)->recvmsg.data, buf.data, strlen(buf.data));
        printf("新信息接收完成\n");
    }
}

/*若为登录消息，则将其消息插入用户链表*/
void add_user(struct user_info *head,struct servmsg *user)
{
    char buf[] = "对不起，该用户已登录,不能重复登录!\n谢谢使用！\n";
    int ret = -1;
    struct msg repeat;
    bzero(&repeat,sizeof(struct msg));
    struct user_info *new_user;//接收到为登录消息，添加服务器列表节点
    new_user = (struct user_info *)malloc(sizeof(struct user_info));
    strcpy(new_user->user_name,user->recvmsg.self_name);
    new_user->id = i++;
    new_user->cli_addr = user->addr; //结构体整体赋值，客户端端口为5倍进程
    new_user->next = NULL;

    while( head->next != NULL)//遍历节点
    {
        head = head->next;
        printf("head->user_name:%s\n",head->user_name);
        if(strcmp(head->user_name,new_user->user_name) == 0)
        {
            repeat.type = ERROR;
            strcpy(repeat.data,buf);
            ret = sendto(serv_fd, &repeat,sizeof(repeat), 0, (struct sockaddr *)&(new_user->cli_addr), sizeof(struct sockaddr));   //发送消息
            if(ret < 0 )
            {
                perror("sendto");
                exit(1);
            }
        }
    }
    head->next = new_user;
    printf("新用户登录处理完成\n");
}
/*发送注册信息到客户端 @jiayu*/
void *send_register_info(struct sockaddr_in addr,const char *src_data)
{
   int ret = -1;
   struct msg buf;
   bzero(&buf,sizeof(buf));
   buf.type = REGISTER;//消息类型为REGISTER
   strcpy(buf.data,src_data);
   ret = sendto(serv_fd,&buf,sizeof(buf),0,(struct sockaddr *)&addr,sizeof(addr));

   if(ret < 0)
   {
      perror("send :");
   }
   printf("%d:%s\n",buf.type,buf.data);
}
/*发送登录信息到客户端 @jiayu*/
void *send_log_info(struct sockaddr_in addr,const char *src_data)
{
   int ret = -1;
   struct msg buf;
   bzero(&buf,sizeof(buf));
   buf.type = LOG_IN;//消息类型为LOG_IN
   strcpy(buf.data,src_data);
   ret = sendto(serv_fd,&buf,sizeof(buf),0,(struct sockaddr *)&addr,sizeof(addr));

   if(ret < 0)
   {
      perror("send :");
   }
   printf("%d:%s\n",buf.type,buf.data);
}
/*发送在线用户列表至客户端*/
void *send_all_online(struct sockaddr_in addr)
{
    char nobody_logo[]="对不起，暂时没有其他用户在线！";
    int num = -1;
    int len = -1;
    int i = 0;
    struct user_info *tmp;
    struct msg buf;
    bzero(&buf,sizeof(buf));
    buf.type = REFRESH;//消息类型为刷新

    tmp = head_user->next;
    if(tmp == NULL) {
        strcpy(buf.data,nobody_logo);
        printf("消息即将发送至addr   :%s(%d)\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
        num = sendto(serv_fd,&buf,sizeof(buf),0,(struct sockaddr *)&addr,sizeof(addr));
        if(num < 0){
            perror("sendto");
            exit(1);
        }
        printf("%d:%s\n",buf.type,buf.data);
    }
    else{
        while(tmp != NULL){
            bzero(&buf.data,sizeof(buf.data));
            buf.type = REFRESH;//消息类型为刷新
            strcpy(buf.data,tmp->user_name);
            printf("消息类型：%d数据：%s\n",buf.type,buf.data);

            printf("\n消息即将发送至addr   :%s(%d)\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
            num = sendto(serv_fd,&buf,sizeof(buf),0,(struct sockaddr *)&addr,sizeof(addr));
            if(num < 0){
                perror("sendto");
                exit(1);
            }
            tmp = tmp->next;
        }
    }
    bzero(&buf,sizeof(buf));
    buf.type = OVER;//结束发送
    num = sendto(serv_fd,&buf,sizeof(buf),0,(struct sockaddr *)&addr,sizeof(addr));
    if(num < 0){
        perror("sendto");
        exit(1);
    }
    printf("\n结束发送\n");
}
/*用户下线，则将其从链表中删除*/
void  delete_user(char name[])
{
    struct user_info *tmp;
    tmp = head_user->next;

    struct user_info *tmp2;
    tmp = head_user; //tmp2记住tmp前面的节点

    while(strcmp(name,tmp->user_name)){
        tmp2 = tmp;
        tmp = tmp->next;
    }
    tmp2->next = tmp->next;//删除tmp
    //free(tmp);
    //tmp = NULL;
}

/*@jiayu*/
void register_user()
{
    int i = 0;
    int j = 0;
    char file_buffer[40];
    char analyze_buffer[1024];//存放analyze数据
    const char regis_failed[] = "Register failed";
    const char regis_success[] = "Register Success";

    bzero(analyze_buffer,sizeof(analyze_buffer));
    bzero(file_buffer,sizeof(file_buffer));
    while(H->recvmsg.data[i] != '\n')
    {
        analyze_buffer[j] = H->recvmsg.data[i];
        i++;
        j++;
    }
    analyze_buffer[j] = '\0';
    FILE *fp = fopen(analyze_buffer,"at");
    if(fp == NULL)
    {
        send_register_info(H->addr,regis_failed);
        perror("Open data:");
    }
    i++;
    j = 0;
    bzero(analyze_buffer,sizeof(analyze_buffer));
    while(H->recvmsg.data[i] != '\n')
    {
        analyze_buffer[j] = H->recvmsg.data[i];
        i++;
        j++;
    }
    analyze_buffer[j] = '\n';
    fputs(analyze_buffer,fp);
    fclose(fp);
   send_register_info(H->addr,regis_success);
}
/*@jiayu*/
void check_user()
{
    int i = 0;
    int j = 0;
    char file_buffer[40];
    char analyze_buffer[1024];//存放analyze数据
    const char log_failed[] = "Log failed";
    const char log_success[] = "Log success";

    bzero(analyze_buffer,sizeof(analyze_buffer));
    bzero(file_buffer,sizeof(file_buffer));
    while(H->recvmsg.data[i] != '\n')
    {
        analyze_buffer[j] = H->recvmsg.data[i];
        i++;
        j++;
    }
    analyze_buffer[j] = '\0';
    FILE *fp = fopen(analyze_buffer,"r");
    if(fp == NULL)
    {
        perror("Open data:");
    }
    i++;
    j = 0;
    bzero(analyze_buffer,sizeof(analyze_buffer));
    while(H->recvmsg.data[i] != '\n')
    {
        analyze_buffer[j] = H->recvmsg.data[i];
        i++;
        j++;
    }
    analyze_buffer[j] = '\n';
    while (!feof(fp))//feof(fp)有两个返回值:如果遇到文件结束，函数feof（fp）的值为非零值，否则为0。
    {
        fgets(file_buffer,40,fp);//fgets 以字符串为单位
        if (strcmp(file_buffer,analyze_buffer) == 0 )
        {
            printf("%s登录消息",H->recvmsg.self_name);
            send_log_info(H->addr,log_success);
            //send_all_online(H->addr);
            find = 1;
            break;
            
        }
    }
    if (!find)
    {
            printf("Username or Password incorrect!\n");
            send_log_info(H->addr,log_failed);
    }
    fclose(fp);
}
/*私聊时，根据消息定位对方用户列表的位置，从而获取其接收套接字 IP和端口,*/
struct user_info *locat_des(char *name)
{
    printf("head_user->addr:%p\n",head_user->next);
    if (head_user->next == NULL) return NULL; //无其他用户
    struct user_info *tmp;
    tmp = head_user;
    
    while(tmp != NULL)
    {
        printf("username:%s\n",tmp->user_name);
        if(strcmp(tmp->user_name,name) == 0)
        {
                return tmp;
        }
    tmp = tmp->next;
    }
    return NULL;
}
/*发送消息至客户端*/
void sendmsg_to_cli(struct user_info *locat_tmp)
{
    int ret = -1;
    int msglen = -1;
    socklen_t addrlen = -1;
    msglen = sizeof(char)+20+20+strlen(H->recvmsg.data);
    addrlen = sizeof(struct sockaddr_in);
    ret = sendto(serv_fd, &(H->recvmsg),msglen, 0, (struct sockaddr *)&(locat_tmp->cli_addr), addrlen);   //发送消息
    if(ret < 0 ){
        perror("sendto");
        exit(1);
    }
    printf("将消息已发送至客户端\n");
}
/*私聊消息转发*/
void chat_private(void)
{
    char server[]="服务器";
    char off_logo[]="对不起，该用户不在线！";
    int sock_fd;
    sock_fd = serv_fd;
    struct user_info *locat_tmp;
    locat_tmp = locat_des(H->recvmsg.dst_name);         //定位，找出消息目标用户

    if(locat_tmp == NULL){
        printf("聊天对象不在线，进入处理。\n");
        locat_tmp = (struct user_info *)malloc(sizeof(struct user_info));
        strcpy(H->recvmsg.self_name,server);

        bzero(H->recvmsg.data,sizeof(H->recvmsg.data));
        strcpy(H->recvmsg.data,off_logo);
        H->recvmsg.data[strlen(off_logo)]='\n';

        locat_tmp->cli_addr = H->addr;
        printf("消息来自addr   :%s(%d)\n",inet_ntoa(H->addr.sin_addr),ntohs(H->addr.sin_port));
        sendmsg_to_cli(locat_tmp);//对方不在线，发送提示
        //free(locat_tmp);
        //locat_tmp = NULL;
        return;
    }
    sendmsg_to_cli(locat_tmp);
}

/*转发群聊消息至广播地址(客户端另开辟了一条接收群聊消息的线程)*/
void chat_toall( )
{
//     int msglen = -1;
//     socklen_t addrlen = -1;
//     int num = -1;
//     struct sockaddr_in msgbuf_tmp; //组播
//     printf("group_fd:%d\n",group_fd);
    
//     bzero(&msgbuf_tmp,sizeof(struct sockaddr_in));
//     msgbuf_tmp.sin_family = AF_INET;
//     msgbuf_tmp.sin_port = htons(GROUP_PORT);//
//     /* 设置发送组播消息的源主机的地址信息 */
//     msgbuf_tmp.sin_addr.s_addr = inet_addr(GROUP_IP);

//     struct msg buf;
//     buf = H->recvmsg;
    
//     msglen = sizeof(char)+20+20+strlen(buf.data);
//     addrlen = sizeof(msgbuf_tmp);
//     num = sendto(group_fd,&buf,msglen,0,(struct sockaddr *)&msg,buf_tmp,addrlen);
//     if(num < 0){
//         perror("sendto");
//         exit(1);
//     }
// //  H->recvmsg.data[num - sizeof(H->recvmsg.type)-sizeof(H->recvmsg.self_name)-sizeof(H->recvmsg.dst_name)] = '\0';
//     printf("num:%d\n",num);
//     printf("self_name:%s\n",buf.self_name);
//     printf("data:%s\n",buf.data);
//     printf("消息已发送至组播地址\n");
    int group_fd;     //广播套接字
    int msglen ;
    int num;
    int nb = 0;
    int opt = 1;
    struct msg buf;

    group_fd = udp_link();
    buf = H->recvmsg;
    //设置该套接字为广播类型，
    nb = setsockopt(group_fd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
    if(nb < 0)
    {
        perror("SO_BROADCAST:");
    }
    struct sockaddr_in addrto;
    bzero(&addrto, sizeof(struct sockaddr_in));
    addrto.sin_family=AF_INET;
    addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);
    addrto.sin_port=htons(6000);
    int nlen=sizeof(addrto);
    msglen = sizeof(char)+20+20+strlen(buf.data);

    num = sendto(group_fd,&buf,msglen,0,(struct sockaddr *)&addrto,nlen);
    if(num < 0){
        perror("sendto");
        exit(1);
    }
//  H->recvmsg.data[num - sizeof(H->recvmsg.type)-sizeof(H->recvmsg.self_name)-sizeof(H->recvmsg.dst_name)] = '\0';
    printf("num:%d\n",num);
    printf("self_name:%s\n",buf.self_name);
    printf("data:%s\n",buf.data);
    printf("消息已发送至广播地址\n");


}


/*解析消息线程*/
void *pthread_func()
{
    head_user = (struct user_info *)malloc(sizeof(struct user_info));
    head_user->next = NULL;

    struct user_info *tmp; // 循环变量，用于便利在线用户链表将其发送给第一次登录的或者有刷新请求的客户端
    struct  servmsg *free_tmp;

    int ret= -1;
    while(1)
    {
        while(H->next != NULL)
        {
            //                   free_tmp = H;
            H = H->next; //消息头，处理玩就往后移动
            //                   free(free_tmp);//将处理完的消息所占内存释放掉
            //                   free_tmp = NULL;
            printf("消息链表首地址H :%p\n",H);
            printf("准备解析消息---\n");
            printf("消息来自%s:\tIP:%s(%d)\n",H->recvmsg.self_name,inet_ntoa(H->addr.sin_addr),ntohs(H->addr.sin_port));
            if(H->recvmsg.type == REGISTER)
            {
                register_user();
                continue;
            }
            if(H->recvmsg.type == LOG_IN )
            {
                check_user(); //
                if(find)
                {
                    add_user(head_user,H);// add 
                }
                continue;
            }          
            if(H->recvmsg.type == REFRESH){
                printf("%s刷新消息\n",H->recvmsg.self_name);
                //usleep(50000);
                send_all_online(H->addr);
                continue;
            }
            if (H->recvmsg.type == OFFLINE){
                printf("%s下线\n",H->recvmsg.self_name);
                delete_user(H->recvmsg.self_name);
            }
            if (H->recvmsg.type == CHAT_PRI){
                printf("%s私聊消息\n",H->recvmsg.self_name);
                printf("目标：%s\n",H->recvmsg.dst_name);
                chat_private();
            }
            if(H->recvmsg.type == CHAT_ALL){
                printf("%s群聊消息\n",H->recvmsg.self_name);
                chat_toall();
            }

            printf("-----------------------------------\n");
        }//end while(H)
    }
}



int main(void)
{
    void myhandle(int signum);
    signal(SIGINT,myhandle);

    serv_fd = udp_link();

    H = (struct servmsg *)malloc(sizeof(struct servmsg));
    H->next = NULL;

    static struct servmsg *H_recv;
    H_recv = H;

    socklen_t addrlen= -1;
    int ret = -1;
    int on = 1;
    static pthread_t tid1 = -1; 
    bzero(&self_addr,sizeof(struct sockaddr));
    /*设置服务器IP地址和端口*/ 
    self_addr.sin_family = AF_INET;
    self_addr.sin_port = htons(SERVER_PORT);
    self_addr.sin_addr.s_addr = inet_addr(SEVER_IP);

    ret = setsockopt(serv_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&on,sizeof(on));
    if(ret < 0){
        perror("setsockopt to SO_REUSEADDR");
        exit(1);
    }

    if(bind(serv_fd,(struct sockaddr *)&self_addr,sizeof(struct sockaddr)) < 0){
        perror("bind");
        exit(1);
    }
    printf("*******欢迎使用********\n");
    printf("服务器正在运行……\n");
    /*创建解析数据包的线程*/
    if(pthread_create(&tid1, NULL, pthread_func,NULL ) < 0){
        perror("pthread_create");
        exit(1);
    }
    pthread_detach(tid1);

    addrlen = sizeof(struct sockaddr_in);

    while(1)
    {
        bzero(&cli_msgbuf,sizeof(struct msg));
        ret = recvfrom(serv_fd,&cli_msgbuf,sizeof(cli_msgbuf),0,(struct sockaddr *)&cli_addr,&addrlen);
        if(ret < 0)
        {
            perror("recvfrom");
            exit(1);
        }
        printf("有新消息发送到服务器\n");
        /*把收到的消息放入链表等待解析*/
        insert(&H_recv,cli_msgbuf,cli_addr);//H_recv每次改变，将消息每次插入至链表末尾
    }

    return 0;
}


void myhandle(int signum)
{
    printf("\n谢谢使用！\n再见！\n");
    close(serv_fd);
    exit(1);
}



