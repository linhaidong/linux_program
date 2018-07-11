/*************************************************************************
    > File Name: sig_process.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2017年04月11日 星期二 15时16分32秒
    > Abstract: 
 ************************************************************************/
#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <syslog.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/stat.h>  
#define LOCKFILE "/var/run/matrix.pid"  
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)  
int already_running(void);  
int lockfile(int );  
int main(int argc,char * argv[])  
{  
    int val = already_running();  
    if(val == 0)  
    {  
        printf("sart to running.../n");  
    }  
    else  
    {  
        printf("alredy running.../n");  
        exit(0);  
    }  
    while(1)  
    {  
        sleep(1);  
        printf("dsdssds../n");  
    }  
    return 0;  
}  
int already_running(void)  
{  
    int fd;  
    char buf[16];  
    fd = open(LOCKFILE,O_RDWR|O_CREAT, LOCKMODE);  
    if(fd < 0)  
    {  
        printf("can't open file\r\n");
        //syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));  
        exit(1);  
    }  
    if(lockfile(fd) < 0)  
    {  
        if (errno == EACCES || errno == EAGAIN)  
        {  
            close(fd);  
            return 1;  
        } 
        printf("can't lock file\r\n");
        //syslog(LOG_ERR,"can't lock %s: %s", LOCKFILE, strerror(errno));  
        exit(1);  
    }  
    ftruncate(fd,0);  
    sprintf(buf,"%ld",(long)getpid());  
    write(fd,buf,strlen(buf) + 1);  
    return 0;  
}  
int lockfile(int fd)  
{  
    struct flock fl;  
    fl.l_type = F_WRLCK;  
    fl.l_start = 0;  
    fl.l_whence = SEEK_SET;  
    fl.l_len = 0;  
    return(fcntl(fd, F_SETLK, &fl));  
}  

