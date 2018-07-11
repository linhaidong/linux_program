/*************************************************************************
    > File Name: server.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2017年07月17日 17时57分42秒 CST
    > Abstract: 
 ************************************************************************/
#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/un.h>   
#include <unistd.h>
//#define UNIX_DOMAIN "/tmp/UNIX.domain"  
#define UNIX_DOMAIN "@local_addr"  
int main(void)  
{  
    socklen_t clt_addr_len;  
    int listen_fd;  
    int com_fd;  
    int ret;  
    int i;  
    static char recv_buf[1024];   
    int len;  
    struct sockaddr_un clt_addr;  
    struct sockaddr_un srv_addr;  
    listen_fd=socket(PF_UNIX,SOCK_STREAM,0);  
    if(listen_fd<0)  
    {  
        perror("cannot create communication socket");  
        return 1;  
    }    
     
    memset(&srv_addr, 0, sizeof(srv_addr));
    //set server addr_param  
    srv_addr.sun_family = AF_UNIX;  
    strncpy(srv_addr.sun_path, UNIX_DOMAIN,sizeof(srv_addr.sun_path)-1);  
    srv_addr.sun_path[0] = 0;
    ret=connect(listen_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));  
    if(ret==-1)  
    {  
        perror("cannot connect server socket");  
        close(listen_fd);  
        return 1;  
    }  
    for(i=0;i<4;i++)  
    {  
        memset(recv_buf,0,1024);  
        int num = write(listen_fd,"hello world",11);  
    }  
    close(listen_fd);  
    return 0;  
}  
