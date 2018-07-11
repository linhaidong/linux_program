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
#define UNIX_DOMAIN "@matrix_local_tcp_server"  
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
    srv_addr.sun_family=AF_UNIX;  
    strncpy(srv_addr.sun_path,UNIX_DOMAIN,sizeof(srv_addr.sun_path)-1);  
    srv_addr.sun_path[0] = 0;
    unlink(UNIX_DOMAIN);  
    //bind sockfd & addr  
    ret=bind(listen_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));  
    if(ret==-1)  
    {  
        perror("cannot bind server socket");  
        close(listen_fd);  
        unlink(UNIX_DOMAIN);  
        return 1;  
    }  
    //listen sockfd   
    ret=listen(listen_fd,1);  
    if(ret==-1)  
    {  
        perror("cannot listen the client connect request");  
        close(listen_fd);  
        unlink(UNIX_DOMAIN);  
        return 1;  
    }  
    //have connect request use accept  
    len=sizeof(clt_addr);  
    com_fd=accept(listen_fd,(struct sockaddr*)&clt_addr,&len);  
    if(com_fd<0)  
    {  
        perror("cannot accept client connect request");  
        close(listen_fd);  
        unlink(UNIX_DOMAIN);  
        return 1;  
    }  
    //read and printf sent client info  
    printf("/n=====info=====/n");  
    //for(i=0;i<4;i++)  
    while(1)
    {  
        memset(recv_buf,0,1024);  
        int num=read(com_fd,recv_buf,sizeof(recv_buf));  
        printf("Message from client (%d)) :%s\r\n",num,recv_buf);    
    }  
    close(com_fd);  
    close(listen_fd);  
    unlink(UNIX_DOMAIN);  
    return 0;  
}  
