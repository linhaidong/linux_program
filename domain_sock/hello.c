/*************************************************************************
    > File Name: hello.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2016年12月23日 星期五 15时30分31秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h> 
#include <errno.h>
#include "local_addr.h"

static local_addr server_addr;
int msg_init(int id)
{
    int msgid = -1;  
    msgid = msgget((key_t)id, 0666 | IPC_CREAT);  
    if(msgid == -1)  
    {  
        fprintf(stderr, "msgget failed with error: %d\n", errno);  
        return -1;
    }
    return msgid;
}


int msg_send(char * buf, int len)
{
    return local_addr_send(&server_addr, buf, len);
}

int msg_recv(int id, int msgtype, char *buf, int len)
{
    int ret;
    ret = msgrcv(id, (void*)buf, len, msgtype, 0);
    if(ret == -1)
    {  
        fprintf(stderr, "msgrcv failed with errno: %d\n", errno);  
        return -1;
    } 
    return ret;
}

int main()
{
    int ret, i;
    local_addr client_addr;
    local_data_msg msg;
    int pid = getpid();
    char buf[512];
    msg_data * pdata;
    memset(&server_addr , 0, sizeof(local_addr));
    memset(&msg , 0, sizeof(local_data_msg));
    local_addr_init(&server_addr);
    client_addr_init(&client_addr);


    msg.type = 1;
    msg.len = 2;
    msg.pid = pid;
    
    for(i = 0; i < 100; i++)
    {
        memset(buf, 0, sizeof(buf));
        local_addr_send(&server_addr, (char *)&msg, sizeof(local_data_msg));
       // sleep(1);
        ret = container_msg_recv(&client_addr,  buf, sizeof(buf));
        if(ret > 0)
        {
            pdata = (msg_data *)buf;
            printf("return type is %ld, data is %d\r\n", pdata->msg_type, pdata->data);
        }
    }
    return 0;
}
