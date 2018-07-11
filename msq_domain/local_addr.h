/*************************************************************************
    > File Name: local_addr.h
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2016年12月23日 星期五 15时09分45秒
 ************************************************************************/
#ifndef YOUKU_CONTAINER_H
#define YOUKU_CONTAINER_H
#include <sys/socket.h>
#include <sys/un.h>
typedef struct local_addr
{
    int sockfd;
    unsigned int len;
    struct sockaddr_un addr;
}local_addr;

typedef struct local_data_msg
{
    int  type;
    int  len;
    int  pid;
    char data[0];
}local_data_msg;

typedef struct msg_data
{
    long int msg_type;
    int data;
}msg_data;
extern int local_addr_init(local_addr * addr);
extern int local_addr_send(local_addr * addr, char * buf, int len);
extern int local_server_init(local_addr *addr);
#endif

