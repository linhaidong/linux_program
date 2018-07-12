/*************************************************************************
    > File Name: server.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2018年07月12日 16时34分15秒 CST
    > Abstract: 
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include<string.h>
/*
int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0 )
    {
        printf("socke crate failed\r\n");
    }
}
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFLEN 100

const char* IP = "127.0.0.1";
const unsigned int SERV_PORT = 7777;
void Chat(int sockfd);

int main(int argc, char *argv[])
{
    int listenfd, connectfd;
    struct sockaddr_in s_addr, c_addr;
    char buf[BUFLEN];
    unsigned int port, listnum;
    pid_t childpid;
    socklen_t len;

    /*建立socket*/
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(errno);
    }
    /*设置服务器端口*/    
    port = SERV_PORT;

    /*设置侦听队列长度*/
    listnum = 5;

    /*设置服务器ip*/
    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
//  s_addr.sin_addr.s_addr = inet_aton(IP, &s_addr.sin_addr);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*把地址和端口帮定到套接字上*/
    if((bind(listenfd, (struct sockaddr*) &s_addr,sizeof(struct sockaddr))) == -1){
        perror("bind");
        exit(errno);
    }
    /*侦听本地端口*/
    if(listen(listenfd, listnum) == -1){
        perror("listen");
        exit(errno);    
    }
    while(1){
        printf("*****************server start***************\n");
        len = sizeof(struct sockaddr);
        if((connectfd = accept(listenfd, (struct sockaddr*) &c_addr, &len)) == -1){
            perror("accept");        
            exit(errno);
        }
        else
        {
            printf("connected with client, IP is: %s, PORT is: %d\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
        }

        //创建子进程
        if((childpid = fork()) == 0)
        {
            Chat(connectfd);

#if 0
            /*是否退出服务器*/
            printf("exit?：y->yes；n->no ");
            bzero(buf, BUFLEN);
            fgets(buf,BUFLEN, stdin);
            if(!strncasecmp(buf,"y",1)){
                printf("server stop\n");
                break;
            }
#endif
            //退出子进程
            exit(0);
        }
        else if(childpid > 0 )
        {
            close(connectfd);
        }

    }
    /*关闭监听的套接字*/
    close(listenfd);
    return 0;
}

void Chat(int sockfd)
{
    socklen_t len;
    char buf[BUFLEN];

    while(1)
    {
        _retry:
            /******发送消息*******/
            bzero(buf,BUFLEN);
            printf("enter your words:");
            /*fgets函数：从流中读取BUFLEN-1个字符*/
            fgets(buf,BUFLEN,stdin);
            /*打印发送的消息*/
            //fputs(buf,stdout);
            if(!strncasecmp(buf,"quit",4))
            {
                printf("server stop\n");
                break;
            }

            /*如果输入的字符串只有"\n"，即回车，那么请重新输入*/
            if(!strncmp(buf,"\n",1))
            {
                goto _retry;
            }    

            /*如果buf中含有'\n'，那么要用strlen(buf)-1，去掉'\n'*/            
            if(strchr(buf,'\n'))
            {
                len = send(sockfd, buf,strlen(buf)-1,0);
            }
            /*如果buf中没有'\n'，则用buf的真正长度strlen(buf)*/    
            else
            {
                len = send(sockfd,buf,strlen(buf),0);
            }

            if(len > 0)
                printf("send successful\n");            
            else{
                printf("send failed\n");
                break;            
            }
            /******接收消息*******/
            bzero(buf,BUFLEN);
            len = recv(sockfd,buf,BUFLEN,0);
            if(len > 0)
            {
                printf("receive massage:%s\n",buf);
                /*关闭已连接套接字*/
                close(sockfd);
                sleep(5);
                len = recv(sockfd,buf,BUFLEN,0);
                printf("first recv, return %d, err:%d, info:%s\r\n", len,errno, strerror(errno));
                break;
            }
            else  if(len < -1 )
            {
                printf("receive failed\n");
                break;        
            }
            else if (len == 0)
            {
                printf("client stop!!!!!!!!!!!!\n");
                len = recv(sockfd,buf,BUFLEN,0);
                printf("second recv, return %d, err:%d, info:%s\r\n", len,errno, strerror(errno));
                break;
            }
    }
}
