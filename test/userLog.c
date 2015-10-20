#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void UserReg();
void Login();
char *my_strcut(char *dest,const char *src);
char *my_strcpy(char *dst, const char *src);

int main()
{
    char src[6] = "abcd\n";
    char dest[10];
    printf("before: %s",src);
    printf("after: %s",my_strcpy(&dest[0],src));
    printf("strcut:%s",my_strcut(&dest[0],src));

	int selectNum;
	printf("Registered and Login system\n\n");
	printf("1 UserReg\n");
	printf("2 Login\n");
	printf("3 Exit\n");
	scanf("%d",&selectNum);
	switch (selectNum)
	{
	case 1:UserReg();
		break;
	case 2:Login();
		break;
	case 3:printf("Exit Program!\n");
		exit(1);
		break;
	}

	return 0;
}

void UserReg()
{
	FILE *fp;
	char Linedata[50]={0},User[20],Pass[20];
	fp = fopen("data.dat","at");
	printf("Registered Account:\n");
	printf("Please input your name:\n");
	fflush(stdin);
	//gets(User);
	read(0,User,5);
	printf("Please input your password:\n");
	fflush(stdin);
	//gets(Pass);
	read(0,Pass,5);

	strcpy(Linedata,User);
	strcat(Linedata,",");
	strcat(Linedata,Pass);
	strcat(Linedata,"\n");
	fputs(Linedata,fp);
	fclose(fp);
	printf("Registered Success!\n");
}

void Login()
{
	FILE *fp;
	int find=0;
	char User[20],Pass[20],Userstrcat[50]={0};
	char Userdata[50]={0};
	fp = fopen("data.dat","r");
	printf("Login Account:\n");
	printf("Please input Username:\n");
	fflush(stdin);
	//gets(User);
	read(0,User,5);

	printf("Please input Password:\n");
	fflush(stdin);
	//gets(Pass);
	read(0,Pass,5);

	strcpy(Userstrcat,User);
	strcat(Userstrcat,",");
	strcat(Userstrcat,Pass);
	strcat(Userstrcat,"\n");
	while (!feof(fp))//feof(fp)有两个返回值:如果遇到文件结束，函数feof（fp）的值为非零值，否则为0。
	{
		fgets(Userdata,19,fp);
		if (strcmp(Userdata,Userstrcat)==0 )
		{
			printf("OK!Login Success!\n");
			printf("Welcome back,%s!\n",User);
			find=1;
			break;
		}
	}
	if (!find)
	{
		printf("Username or Password incorrect!\n");
	}
	fclose(fp);
}


char *my_strcpy (char *dest, const char *src)
{
    char *pb = dest;

    while(*src != '\0')

    {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';

    return pb;

}

char *my_strcut(char *dst, const char *src)
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





