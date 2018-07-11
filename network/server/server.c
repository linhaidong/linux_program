/*************************************************************************
    > File Name: server.c
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2016年12月01日 星期四 15时49分46秒
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <errno.h>
#include <unistd.h>
#include <event2/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "queue.h"
typedef struct client{
    struct sockaddr_in addr;
    int fd;
    TAILQ_ENTRY(client) link;
}client;
static TAILQ_HEAD(queue, client) head;

void accept_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void *arg);
int tcp_server_init(int port, int listen_num);

void trim(char *src)
{
    char *begin = src;
    char *end   = src;

    while ( *end++ ) {
        ; // Statement is null  
    }

    if ( begin == end ) return; 

    while ( *begin == ' ' || *begin == '\t' )          
        ++begin;
    while ( (*end) == '\0' || *end == ' ' || *end == '\t' || *end == '\n' ) 
        --end;

    if ( begin > end ) {
        *src = '\0';  return; 
    } 
    //printf("begin=%1.1s; end=%1.1s\n", begin, end);
    while ( begin != end ) {
        *src++ = *begin++;
    }

    *src++ = *end;
    *src = '\0';

    return;
}
void client_free()
{
    client * pc;
    TAILQ_FOREACH(pc, &head, link)
    {
        TAILQ_REMOVE(&head, pc, link);
    }
}

struct event * file_event_add(struct event_base * base, event_callback_fn cb)
{
    struct event *ev;
    ev = event_new(base, 1, EV_READ| EV_PERSIST, cb, NULL);
    if(!ev)
        return NULL;
    event_add(ev, NULL);
    return ev;
}

int main(int argc, char** argv)
{
    TAILQ_INIT(&head);
	int listener = tcp_server_init(9999, 10);
	if( listener == -1 )
	{
		perror(" tcp_server_init error ");
		return -1;
	}
    printf("Init server .........\r\n");
	struct event_base* base = event_base_new();
    struct event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, base);
	event_add(ev_listen, NULL);
	event_base_dispatch(base);
    client_free();
	return 0;
}

void accept_cb(int fd, short events, void* arg)
{
    client *pc;
	struct event_base* base = (struct event_base*)arg;
	struct event *ev = NULL;
    evutil_socket_t sockfd;
	socklen_t len;
     
    pc = (client *)calloc(1, sizeof(client));
    if(!pc)
    {
        printf("create client error\r\n");
        return;
    }
	sockfd = accept(fd, (struct sockaddr*)&pc->addr, &len );
    if(sockfd == -1)
    {
        printf("accept error:%s", strerror(errno));
        free(pc);
        return;
    }
    printf("accept sock %d connnect\r\n", sockfd);
    evutil_make_socket_nonblocking(sockfd);
    pc->fd = sockfd;
    printf("%s:%d\r\n", __FUNCTION__, __LINE__);
    TAILQ_INSERT_HEAD(&head, pc, link);
    ev  = event_new(NULL, -1, 0, NULL, NULL);
	event_assign(ev, base, sockfd, EV_READ | EV_PERSIST, socket_read_cb, (void*)ev);
	event_add(ev, NULL);
    printf("%s:%d\r\n", __FUNCTION__, __LINE__);
}


void socket_read_cb(int fd, short events, void *arg)
{
	char msg[1024];
    client *pc;
	struct event *ev = (struct event*)arg;
    printf("have message\r\n");
    printf("%s:%d\r\n", __FUNCTION__, __LINE__);
	int len = read(fd, msg, sizeof(msg) - 1);
	if( len < 0 )
	{
		printf("some error happen when read, ret is %d\n", len);
        printf("errno is %d, %s\r\n", errno, strerror(errno));
		close(event_get_fd(ev));
        TAILQ_FOREACH(pc,&head, link)
        {
            if(pc->fd  == fd)
            {
                TAILQ_REMOVE(&head, pc, link);
            }
        }
		event_free(ev);
		return ;
	}
	
    msg[len] = '\0';
	printf("recv msg: %s\r\n", msg);
    if( strncmp("exit", msg, 4) == 0)
	{
		close(event_get_fd(ev));
        TAILQ_FOREACH(pc,&head, link)
        {
            if(pc->fd  == fd)
            {
                TAILQ_REMOVE(&head, pc, link);
            }
        }
		printf("child exit\n");
        exit(0);
		return ;
    }	

}


int tcp_server_init(int port, int listen_num)
{
	int errno_save;
	evutil_socket_t listener;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if( listener == -1 )
		return -1;

	evutil_make_listen_socket_reuseable(listener);

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);

	if(bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0 )
		goto error;

	if(listen(listener, listen_num) < 0)
		goto error;

	evutil_make_socket_nonblocking(listener);
	return listener;

	error:
		errno_save = errno;
		evutil_closesocket(listener);
		errno = errno_save;
		return -1;
}

