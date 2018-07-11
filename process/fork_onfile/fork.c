/*************************************************************************
    > File Name: fork.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2017年04月21日 星期五 08时49分30秒
    > Abstract: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char buf1[200] = "11111111111111111process1, this working proces1, sprocess1, this working process111111111111111111111111\r\n";
char buf2[200] = "22222222222222222process2, this working proces2, sprocess2, this working process222222222222222222222222\r\n";
int main()
{
    int i;
    pid_t pid;
    int fd;
    fd = open("./test.txt", O_RDWR|O_CREAT);
    if (fd < 0) {
        printf("open file error\r\n");
        return 0;
    }
    pid = fork();
    if(pid == 0) {
        for(i = 0;  i < 1000; i++) {
            write(fd, buf1, strlen(buf1));
        }
    }
    else if(pid > 0) {
        for(i = 0;  i < 1000; i++) {
            write(fd, buf2, strlen(buf2));
        }
    }
    return 0;
}
