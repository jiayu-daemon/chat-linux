#include <stdio.h>
#include <string.h>

int main()
{
    int len;
    char buffer[1024];
    bzero(buffer,sizeof(buffer));

    printf("Please input words\n");
    //scanf("%s",buffer); // 阻塞，不读\n
    //gets(buffer);        // 阻塞，'\n' -> '\0'
    //read(stdin,buffer,10);//读终端，并没有阻塞（不一定）
    fgets(buffer,10,stdin);//阻塞终端，读到了'\n',加'\0'返回。读到9个字节也返回，第10个字节存放'\0'。读到文件末尾也返回。
    //fread(buffer,10,1,stdin);//阻塞终端，只有读到10个字节才返回,否则阻塞等待。把'\n'当作一个字节来读
    len = strlen(buffer);
    printf("%s\n",buffer);
    printf("Len:%d\n",len);
    printf("Test end\n");

    return 0;
}





