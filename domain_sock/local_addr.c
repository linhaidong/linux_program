/*************************************************************************
    > File Name: loacl_addr.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2016年12月23日 星期五 15时03分51秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <netdb.h>
#include <stddef.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include "local_addr.h"
#define PATH "@local_addr"

int local_addr_init(local_addr * addr)
{
    addr->sockfd = socket(AF_UNIX,SOCK_DGRAM,0);
    if(addr->sockfd < 0)
    {
        perror("socket error");
        exit(-1);
    }
    bzero(&addr->addr,sizeof(struct sockaddr_un));
    addr->addr.sun_family = AF_UNIX;
    strcpy(addr->addr.sun_path,PATH);
    addr->addr.sun_path[0]=0;
    addr->len = strlen(PATH)+ offsetof(struct sockaddr_un, sun_path);
    return 0;
}

int local_addr_send(local_addr * addr, char * buf, int len)
{
    return  sendto(addr->sockfd, buf, len, 0, (struct sockaddr*)&addr->addr,addr->len);
}

int server_send(int fd, int pid, char *buf, int len)
{
    int addr_len = 0;
    char dst[20] = {0};
    struct sockaddr_un addr;
    sprintf(dst, "@%d", pid);
    bzero(&addr,sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, dst);
    addr.sun_path[0]=0;
    addr_len = strlen(dst) + offsetof(struct sockaddr_un, sun_path);
    return  sendto(fd, buf, len, 0, (struct sockaddr*)&addr, addr_len);

}
int client_addr_init(local_addr *addr)
{
    int pid;
    char dst[20] = {0};
    memset(addr, 0, sizeof(local_addr));
    addr->sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(addr->sockfd < 0 )
    {
        printf("socket error %s", strerror(errno));
        return 1;
    }
    pid = getpid();
    sprintf(dst, "@%d", pid);
    addr->addr.sun_family = AF_UNIX;
    strcpy(addr->addr.sun_path, dst);
    addr->addr.sun_path[0] = 0;
    addr->len = strlen(dst)+ offsetof(struct sockaddr_un, sun_path);
    if(bind(addr->sockfd,(struct sockaddr *)&addr->addr,addr->len) < 0)
    {
        printf("bind error %s", strerror(errno));
        close(addr->sockfd);
        return 1;
    }
    return 0;
}

int container_msg_recv(local_addr * addr, char *buf, int len)
{
    int ret;
    fd_set set;
    struct timeval time;
    FD_ZERO(&set);
    FD_SET(addr->sockfd, &set);
    time.tv_sec = 0;
    time.tv_usec = 5000;
    ret = select(addr->sockfd+1, &set, NULL, NULL, &time);
    if(ret == -1)
    {
        printf("time out\r\n");
        return -1;
    }
    if(FD_ISSET(addr->sockfd, &set))
    {
        return recvfrom(addr->sockfd, buf, len, 0, (struct sockaddr *)&addr->addr, &addr->len);
    }
    return 0;



}
int local_server_init(local_addr *addr)
{
    memset(addr, 0, sizeof(local_addr));
    addr->sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(addr->sockfd < 0 )
    {
        printf("socket error %s", strerror(errno));
        return 1;
    }
    addr->addr.sun_family = AF_UNIX;
    strcpy(addr->addr.sun_path, PATH);
    addr->addr.sun_path[0] = 0;
    addr->len = strlen(PATH)+ offsetof(struct sockaddr_un, sun_path);
    if(bind(addr->sockfd,(struct sockaddr *)&addr->addr,addr->len) < 0)
    {
        printf("bind error %s", strerror(errno));
        close(addr->sockfd);
        return 1;
    }
    return 0;
}
