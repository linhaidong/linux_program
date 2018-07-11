/*************************************************************************
    > File Name: fd_lock.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2018年03月14日 10时11分30秒 CST
    > Abstract: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
int fd = -1;


void file_writ();

int open_file()
{
    fd =  open("./test.txt", O_RDWR|O_CREAT);
    if (fd == -1) {
        printf("open file error:%s\r\n", strerror(errno));
        exit(-1);
    }
    return fd;
}
int file_lock()
{
    int ret;
    ret  = flock(fd, LOCK_SH);
    //ret  = flock(fd, LOCK_EX|LOCK_NB);
    //ret  = flock(fd, LOCK_EX);
    if (ret != 0) {
        printf("lock failed: %s\r\n", strerror(errno));
    } else {
        printf("lock success\r\n");
    }
    //close(fd);
    return ret;
}

void file_unlock()
{
    int ret;
    if (fd > 0){
        ret = flock(fd, LOCK_UN);
        if (ret != 0) {
            printf("unlock failed\r\n");
        }
        printf("unlock success\r\n");
    }
}


void file_close(void)
{
    int ret;
    file_writ();
    printf("close file\r\n");
    if (fd > 0){
        ret = close(fd);
        if (ret != 0){
            printf("close failed\r\n");
        }
    }
}



void file_read()
{
    char buf[10];
    int ret;
    ret = read(fd, buf, 10);
    if (ret > 0) {
        printf("read byte:%s\r\n", buf);
    }else{
        printf("read byte failed:%d\r\n", ret);
    }
}

void file_writ()
{
    char buf[10] = "123456789\n";
    int ret;
    ret = write(fd, buf, 10);
    if (ret > 0) {
        printf("write byte:%d, %s\r\n", ret, buf);
    }else{
        printf("write byte failed:%d\r\n", ret);
    }
    return ;
}

int main()
{
    char buf[128];
    open_file();
    while(1) {
        scanf("%s", buf);
        //printf("get str %s\r\n", buf);
        if (strncmp(buf, "lock", 4) == 0){
            file_lock();
        } else if (strncmp(buf, "unlock", 6) == 0){
            file_unlock();
        } else if (strncmp(buf, "read", 4) == 0){
            file_read();
        } else if (strncmp(buf, "writ", 4) == 0){
            file_writ();
        } else if (strncmp(buf, "close", 4) == 0){
            file_close();
        } else if (strncmp(buf, "exit", 4) == 0){
            exit(0);
        }
    }
    atexit(file_close);

}
//进程退出会释放相应的锁
