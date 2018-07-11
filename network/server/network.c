/*************************************************************************
    > File Name: network.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年01月09日 星期一 16时31分32秒
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




int main()
{
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
    return 0;
}
