说明：
1 用户注册时，会在服务器端创建user_etc.dat文件
要更改文件位置，请修改宏
在client_udp.c中
#define SERV_USER_ETC "/home/lh/linux-c/QQLan/serv-etc/user_etc.dat\n"
末尾要加'\n'
2 在include.h中
#define SEVER_IP "10.0.0.150"
注意修改服务器IP地址