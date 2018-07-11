/*************************************************************************
    > File Name: fork_free.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2017年04月12日 星期三 20时53分04秒
    > Abstract: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main()
{
    pid_t pid;
    char * str;
    str = (char *)calloc(10, sizeof(char));
    if(!str) {
        printf("calloc failed\r\n");
        return 0;
    }
    strncpy(str, "1234567890", 10);
   pid = fork();
   if(pid == 0)
   {
       sleep(1);
       printf("str is %s\r\n", str);
       printf("%s:%d\r\n", __FUNCTION__, __LINE__);
       free(str);
       printf("%s:%d\r\n", __FUNCTION__, __LINE__);
       exit(0);
   }
   else
   {
       printf("str is %s\r\n", str);
       free(str);
       printf("%s:%d\r\n", __FUNCTION__, __LINE__);
       printf("%s:%d\r\n", __FUNCTION__, __LINE__);
   }
   return 0;
}
