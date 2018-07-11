/*************************************************************************
    > File Name: loacl_addr.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2016年12月23日 星期五 15时03分51秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
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

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis   init domain socket address
 *
 * @Param addr
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
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

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  send msg 
 *
 * @Param addr
 * @Param buf
 * @Param len
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
int local_addr_send(local_addr * addr, char * buf, int len)
{
    return  sendto(addr->sockfd, buf, len, 0, (struct sockaddr*)&addr->addr,addr->len);
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
