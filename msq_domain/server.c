/************************************************************************
    > File Name: container.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2016年12月23日 星期五 15时16分23秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <event2/event.h>
#include <event2/dns.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "local_addr.h"

void local_read_cb(int fd, short events, void *arg)
{
    int ret;
    int msg_fd;
    local_data_msg msg;
    msg_data c_msg;
    local_addr * addr = (local_addr *)arg;

    memset(&msg, 0, sizeof(local_data_msg));
    ret = recvfrom(addr->sockfd, (char *)&msg, sizeof(msg),
            0, (struct sockaddr *)&addr->addr, &addr->len);
    printf("recv mesage %d, %d, %d\r\n", msg.pid, msg.len, msg.pid);
    if(msg.type == 1)
    {
        if(msg.pid > 0)
        {
            //构造消息队列
            msg_fd = msgget(msg.pid, 0);
            if(msg_fd == -1)
            {
                printf("get process id failed\t\n");
                return;
            }
            memset(&c_msg, 0, sizeof(msg_data));
            c_msg.msg_type = msg.pid;
            c_msg.data = 100;
            //消息队列发送
            msgsnd(msg_fd, &c_msg, sizeof(msg_data), 0);
        }
    }
}
int main()
{
    struct event_base *base;
    struct event* ev_local;
    local_addr local_addr;
    /*create timer for calcluate the speed*/
    base = event_base_new();
    if(local_server_init(&local_addr) != 0)
    {
        printf("create local server addr error");
        return -1;
    }

    ev_local = event_new(base, local_addr.sockfd, EV_READ | EV_PERSIST, local_read_cb,
                         &local_addr); 
    event_add(ev_local, NULL);
    event_base_dispatch(base);
}   
