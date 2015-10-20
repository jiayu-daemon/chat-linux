/*字符串*/
#include <stdio.h>
/*字符串比较大小*/
int my_strcmp(char *s1,char *s2)
{
  while (*s1 != '\0')
         {
           if (*s1 != *s2)
           {
             break;
            }
           s1++;
           s2++;
         }        
  int result = *s1 - *s2;
  if ( result > 0 )
    {
     return 1;
    }else if ( result < 0 )
    {
     return -1;
    }else
    {
     return 0;                                                              
    }
}
/*字符串的长度*/
int my_strlen (char *str)
{
    int i = 0;
    while(*str != '\0')
    {
        i++;
        str++;
    }
    return i;
}
/*字符串拼接*/
char * my_strcat (char * dest,char *src)
{
    char *tmp = dest;
    while (*dest != '\0')
    {
        dest++;
    }
    while (*src != '\0')
    {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = '\0';
    return tmp;
}
/*比较字符串的前n位大小*/
int my_strncmp (char *s1,char *s2,int value)
{
    int i=0;
    while ((*s1 != '\0') && (++i < value))
    {
        if (*s1 != *s2)
        {
            break;// if not equel,jump while ,and compare
        }
        s1++;
        s2++;
    }
    int result =*s1 - *s2;
    if(result > 0)
    {
        return 1;
    }else if (result < 0)
    {
        return -1;
    }else
    {
        return 0;
    }
}
/*To reserve a string*/
/*翻转字符串*/
char * reverse (char *str)
{
    int i;
    int tmp;
    int len = my_strlen(str);
    for (i = 0; i < len/2; i++)
    {
        tmp = str[i];
        str[i] = str[len - 1 -i];
        str[len -1 -i] = tmp;
    }
    return str;

}
/*int to string*/
/*将整数转换成字符串输出*/
char * itoa (char *dest,int n)
{
    int i = 0;
    int flag = 0;
    if (n < 0)
    {
        n =-n;
        flag = -1;
    }
    while (n > 0)
    {
        dest[i] = n % 10 + '0';
        i++;
        n = n / 10;
    }
    if(flag == -1)
    {
        dest[i] = '-';
        i++;
    }
    dest[i] = '\0';
    reverse (dest);
    return (dest);
}
/*比较两个字符串，将相同字符串部分之后全部输出*/
char * my_strstr(char * dest, char *src)
{
    int len = my_strlen(src);
    while (*dest != '\0')
    {
       int result = my_strncmp(dest,src,len);
        if (result == 0)
        {
            return dest;
        }
        dest++;// compare dest's next char
    }
    return NULL;
}
int main(void)
{
   char a[15] = "hello";
    printf("%s\n",my_strcat(a,"world"));
    printf("%d\n",my_strlen(a));
    printf ("%d\n",my_strcmp("abc","abc"));
    printf ("%d\n",my_strcmp("acd","abc"));
    printf("%d\n",my_strcmp("ab","abc"));
    printf("%d\n",my_strncmp("ab","abc",2));
    printf ("%d\n",my_strncmp("cdef","ag",2));
    printf ("%s\n",reverse(a));
    char b[5];
    printf ("%s\n",itoa(b,143));
    char * p=my_strstr("I am Chinese","ad");
    if (p != NULL)
    {
        printf ("%s\n",p);
    }else
    {
        printf("not fit\n");
    }
    return 0;
}
