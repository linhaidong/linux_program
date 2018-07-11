/*************************************************************************
    > File Name: nl.c
    > Author: linhaidong
    > Mail: linhaidong@alibaba-inc.com
    > Created Time: 2017年02月23日 09时36分46秒 CST
 ************************************************************************/

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include "nl.h"

#define NLMSG_TAIL(nmsg) \
        ((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

extern size_t nlmsg_len(const struct nlmsg *nlmsg)
{
	return nlmsg->nlmsghdr->nlmsg_len - NLMSG_HDRLEN;
}

extern void *nlmsg_data(struct nlmsg *nlmsg)
{
	char *data = ((char *)nlmsg) + NLMSG_HDRLEN;
	if (!nlmsg_len(nlmsg))
		return NULL;
	return data;
}

static int nla_put(struct nlmsg *nlmsg, int attr,
		   const void *data, size_t len)
{
	struct rtattr *rta;
	size_t rtalen = RTA_LENGTH(len);
	size_t tlen = NLMSG_ALIGN(nlmsg->nlmsghdr->nlmsg_len) + RTA_ALIGN(rtalen);
	
	if (tlen > nlmsg->cap)
		return -ENOMEM;

	rta = NLMSG_TAIL(nlmsg->nlmsghdr);
	rta->rta_type = attr;
	rta->rta_len = rtalen;
	memcpy(RTA_DATA(rta), data, len);
	nlmsg->nlmsghdr->nlmsg_len = tlen;
	return 0;
}

extern int nla_put_buffer(struct nlmsg *nlmsg, int attr,
			  const void *data, size_t size)
{
	return nla_put(nlmsg, attr, data, size);
}

extern int nla_put_string(struct nlmsg *nlmsg, int attr, const char *string)
{
	return nla_put(nlmsg, attr, string, strlen(string) + 1);
}

extern int nla_put_u32(struct nlmsg *nlmsg, int attr, int value)
{
	return nla_put(nlmsg, attr, &value, sizeof(value));
}

extern int nla_put_u16(struct nlmsg *nlmsg, int attr, unsigned short value)
{
	return nla_put(nlmsg, attr, &value, 2);
}

extern int nla_put_attr(struct nlmsg *nlmsg, int attr)
{
	return nla_put(nlmsg, attr, NULL, 0);
}

struct rtattr *nla_begin_nested(struct nlmsg *nlmsg, int attr)
{
	struct rtattr *rtattr = NLMSG_TAIL(nlmsg->nlmsghdr);

	if (nla_put_attr(nlmsg, attr))
		return NULL;

	return rtattr;
}

void nla_end_nested(struct nlmsg *nlmsg, struct rtattr *attr)
{
	attr->rta_len = (void *)NLMSG_TAIL(nlmsg->nlmsghdr) - (void *)attr;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  
 *
 * @Param size
 *
 * @Returns   
four word alignment:
#define NLMSG_ALIGNTO	4U
#define NLMSG_ALIGN(len) ( ((len)+NLMSG_ALIGNTO-1) & ~(NLMSG_ALIGNTO-1) )
#define NLMSG_HDRLEN	 ((int) NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_LENGTH(len) ((len) + NLMSG_HDRLEN
 */
/* ----------------------------------------------------------------------------*/
extern struct nlmsg *nlmsg_alloc(size_t size)
{
	struct nlmsg *nlmsg;
   
    /*字节对齐*/
	size_t len = NLMSG_HDRLEN + NLMSG_ALIGN(size);

	nlmsg = (struct nlmsg *)malloc(sizeof(struct nlmsg));
	if (!nlmsg)
		return NULL;

	nlmsg->nlmsghdr = (struct nlmsghdr *)malloc(len);
	if (!nlmsg->nlmsghdr)
		goto errout;

	memset(nlmsg->nlmsghdr, 0, len);
	nlmsg->cap = len;
	nlmsg->nlmsghdr->nlmsg_len = NLMSG_HDRLEN;

	return nlmsg;
errout:
	free(nlmsg);
	return NULL;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  get space for data message after msg head
 *
 * @Param nlmsg
 * @Param len
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
extern void *nlmsg_reserve(struct nlmsg *nlmsg, size_t len)
{
	void *buf;
	size_t nlmsg_len = nlmsg->nlmsghdr->nlmsg_len;
	size_t tlen = NLMSG_ALIGN(len);

    /*buf has no space to storage info*/
	if (nlmsg_len + tlen > nlmsg->cap)
		return NULL;

	buf = ((char *)(nlmsg->nlmsghdr)) + nlmsg_len;
    /*msg len is totle message len*/
	nlmsg->nlmsghdr->nlmsg_len += tlen;

    /*last word must be fill in zero*/
	if (tlen > len)
		memset(buf + len, 0, tlen - len);

	return buf;
}

extern struct nlmsg *nlmsg_alloc_reserve(size_t size)
{
	struct nlmsg *nlmsg;

	nlmsg = nlmsg_alloc(size);
	if (!nlmsg)
		return NULL;

	// just set message length to cap directly
	nlmsg->nlmsghdr->nlmsg_len = nlmsg->cap;
	return nlmsg;
}

extern void nlmsg_free(struct nlmsg *nlmsg)
{
	if (!nlmsg)
		return;

	free(nlmsg->nlmsghdr);
	free(nlmsg);
}

extern int netlink_rcv(struct nl_handler *handler, struct nlmsg *answer, int len)
//extern int netlink_rcv(struct nl_handler *handler, char * buf, int len)
{
	int ret;
	struct sockaddr_nl nladdr;
    //size_t len = answer->nlmsghdr->nlmsg_len;
    //iov 内容改变
	struct iovec iov = {
		.iov_base = answer->nlmsghdr,
		//.iov_len = answer->nlmsghdr->nlmsg_len,
		//.iov_base = buf,
		//.iov_len = 4096
        .iov_len = len,
	};
	
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	memset(&nladdr, 0, sizeof(nladdr));
#if 0	
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = 0;
	nladdr.nl_groups = 0;
#endif
    printf("add:0x%x, len is %d\r\n", (int)iov.iov_base, iov.iov_len);
again:
	ret = recvmsg(handler->fd, &msg, 0);
	printf("ret is %d\r\n", ret);
    if (ret < 0) {
		if (errno == EINTR)
			goto again;
		return -errno;
	}

#if 0
	if (!ret)
		return 0;
	if (msg.msg_flags & MSG_TRUNC &&
	    ret == answer->nlmsghdr->nlmsg_len)
		return -EMSGSIZE;
#endif
	return ret;
}

#if 0
static int nl_request(int family, int type, struct nlsock *nl)
{
    int ret;
    struct sockaddr_nl snl;

    struct {
        struct nlmsghdr nlh;
        struct rtgenmsg g;
    } req;

    if(nl->sock < 0 ) {
        EINFO("socket isn't active." );
        return -1;
    }

    memset(&snl, 0, sizeof(snl));
    snl.nl_family = AF_NETLINK;

    req.nlh.nlmsg_len = sizeof(req);
    req.nlh.nlmsg_type = type;
    req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
    req.nlh.nlmsg_pid = 0;
    req.nlh.nlmsg_seq = ++nl->seq;
    req.g.rtgen_family = family;

    ret = sendto(nl->sock, (void*)&req, sizeof req, 0, (struct sockaddr*)&snl, sizeof(snl));
    if (ret < 0){
        EINFO("sendto failed: %s",strerror ( errno ) );
        return -1;
    }
    return 0;
}
#endif

extern int netlink_send(struct nl_handler *handler, struct nlmsg *nlmsg)
{
	struct sockaddr_nl nladdr;
	struct iovec iov = {
		.iov_base = nlmsg->nlmsghdr,
		.iov_len = nlmsg->nlmsghdr->nlmsg_len,
	};
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	int ret;
	
	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = 0;
	nladdr.nl_groups = 0;

	ret = sendmsg(handler->fd, &msg, 0);
	if (ret < 0)
		return -errno;

	return ret;
}

#ifndef NLMSG_ERROR
#define NLMSG_ERROR                0x2
#endif

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis        send netlink msg
 *
 * @Param handler
 * @Param request
 * @Param answer
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
#if 0
extern int netlink_transaction(struct nl_handler *handler,
			       struct nlmsg *request, struct nlmsg *answer)
{
	int ret;

	ret = netlink_send(handler, request);
	if (ret < 0)
		return ret;

	ret = netlink_rcv(handler, answer);
	if (ret < 0)
		return ret;

	if (answer->nlmsghdr->nlmsg_type == NLMSG_ERROR) {
		struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(answer->nlmsghdr);
		return err->error;
	}

	return 0;
}
#endif
static int nl_get_link(struct nlmsghdr *h, void ** argv)
{
    int len;
    struct ifinfomsg *ifa;
    struct rtattr *tb[IFA_MAX + 1];
    char ifa_label[IFNAMSIZ + 1] = {0};
#if 0
    local_addr **pladdr = (local_addr **)argv;;
    local_addr *addr_tmp, *addr_next;
#endif

    ifa = (struct ifinfomsg *)NLMSG_DATA(h);
    if(if_indextoname(ifa->ifi_index, ifa_label) == NULL){
        printf("index is error\r\n");
        return -1;
    }
   // printf("type is 0X%x\r\n", ifa->ifi_type);
    printf("name is %s\r\n", ifa_label);
#if 0
    memset(tb, 0, sizeof(tb));
    nl_parse_rtattr(tb, IFA_MAX, IFA_RTA(ifa), len );

    /*name if interface*/
    if(tb[IFLA_IFNAME])
        strncpy(ifa_label, RTA_DATA(tb[IFLA_IFNAME]), IFNAMSIZ); 
    else
        return -1;
#endif
    return 0;
}

static int nl_request ( int family, int type, int sock )
{
    int ret;
    struct sockaddr_nl snl;

    struct {
        struct nlmsghdr nlh;
        struct rtgenmsg g;
    } req;


    /* Check netlink socket. */
    if ( sock < 0 ) {
        fprintf ( stderr, "socket isn't active.");
        return -1;
    }

    memset ( &snl, 0, sizeof snl );
    snl.nl_family = AF_NETLINK;

    req.nlh.nlmsg_len = sizeof req;
    req.nlh.nlmsg_type = type;
    req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
    req.nlh.nlmsg_pid = 0;
    req.nlh.nlmsg_seq = 1000;
    req.g.rtgen_family = family;

    ret = sendto ( sock, ( void* ) &req, sizeof req, 0,
            ( struct sockaddr* ) &snl, sizeof snl );
    if ( ret < 0 ) {
        fprintf ( stderr, " sendto failed: %s",  strerror ( errno ) );
        return -1;
    }
    return 0;
}
/* Receive message from netlink interface and pass those information
   to the given function. */
static int
nl_parse_info (int sock)
{
    int status;
    int ret = 0;
    int error;

    while ( 1 ) {
        char buf[4096];
        struct iovec iov = { buf, sizeof buf };
        struct sockaddr_nl snl;
        struct msghdr msg = { ( void* ) &snl, sizeof snl, &iov, 1, NULL, 0, 0};
        struct nlmsghdr *h;

        status = recvmsg (sock, &msg, 0 );
        printf("recv len is %d\r\n", status);
        if ( status < 0 ) {
            if ( errno == EINTR )
                continue;
            if ( errno == EWOULDBLOCK || errno == EAGAIN )
                break;
            fprintf ( stderr, "recvmsg overrun");
            continue;
        }

        if ( snl.nl_pid != 0 ) {
            fprintf ( stderr, "Ignoring non kernel message from pid %u",
                    snl.nl_pid );
            continue;
        }

        if ( status == 0 ) {
            fprintf ( stderr, "EOF" );
            return -1;
        }

        if ( msg.msg_namelen != sizeof snl ) {
            fprintf ( stderr, " sender address length error: length %d", msg.msg_namelen );
            return -1;
        }

        for ( h = ( struct nlmsghdr * ) buf; NLMSG_OK ( h, status );
                h = NLMSG_NEXT ( h, status ) ) {
            /* Finish of reading. */
            if ( h->nlmsg_type == NLMSG_DONE )
                return ret;

            /* Error handling. */
            if ( h->nlmsg_type == NLMSG_ERROR ) {
                struct nlmsgerr *err = ( struct nlmsgerr * ) NLMSG_DATA ( h );

                /* If the error field is zero, then this is an ACK */
                if ( err->error == 0 ) {
                    /* return if not a multipart message, otherwise continue */
                    if ( ! ( h->nlmsg_flags & NLM_F_MULTI ) ) {
                        return 0;
                    }
                    continue;
                }

                if ( h->nlmsg_len < NLMSG_LENGTH ( sizeof ( struct nlmsgerr ) ) ) {
                    fprintf ( stderr, "%s error: message truncated");
                    return -1;
                }
                fprintf ( stderr, " error: %s, type=%u, seq=%u, pid=%d", strerror ( -err->error ),
                        err->msg.nlmsg_type, err->msg.nlmsg_seq,
                        err->msg.nlmsg_pid );
                /*
                ret = -1;
                continue;
                */
                return -1;
            }
        }

#if 0
            /* skip unsolicited messages originating from command socket */
            if ( nl != &nl_cmd && h->nlmsg_pid == nl_cmd.snl.nl_pid ) {
                continue;
            }

            error = ( *filter ) ( &snl, h, arg );
            if ( error < 0 ) {
                fprintf ( stderr, "%s filter function error/n", nl->name );
                ret = error;
            }
        }

        /* After error care. */
        if ( msg.msg_flags & MSG_TRUNC ) {
            fprintf ( stderr, "%s error: message truncated", nl->name );
            continue;
        }
        if ( status ) {
            fprintf ( stderr, "%s error: data remnant size %d", nl->name,
                    status );
            return -1;
        }
#endif
    }
    return ret;
}

typedef int (* nlmsg_opertaion)(struct nlmsghdr *h, void *arg );

extern int netlink_transaction_getinfo(struct nl_handler *handler,struct nlmsg *request, 
                                    struct nlmsg *answer, nlmsg_operation operation, void *arg)
{
	int ret;
    struct nlmsghdr *h;
    int error;
    int status;
	ret = netlink_send(handler, request);
    if (ret < 0)
		return ret;
    while(1) {
        status = netlink_rcv(handler, answer, answer->nlmsghdr->nlmsg_len);
        if (status < 0) {
            printf("reveice message error\r\n");
            return ret;
        }
        if (answer->nlmsghdr->nlmsg_type == NLMSG_ERROR) {
            struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(answer->nlmsghdr);
            return err->error;
        }
        //for (h = (struct nlmsghdr * )answer->nlmsghdr; NLMSG_OK(h, status); h = NLMSG_NEXT(h, status)) {
        for (h = (struct nlmsghdr * )answer->nlmsghdr; NLMSG_OK(h, status); h = NLMSG_NEXT(h, status)) {
            /* Finish of reading. */
            if (h->nlmsg_type == NLMSG_DONE) {
                printf("netlink msg done\r\n");
                return 0;
            }
            /* Error handling. */
            if (h->nlmsg_type == NLMSG_ERROR) {
                struct  nlmsgerr *err = (struct nlmsgerr * )NLMSG_DATA(h);
                printf("message is error\r\n");
                /* If the error field is zero, then this is an ACK */
                if ( err->error == 0 ) {
                    /* return if not a m ultipart message, otherwise contin ue */
                    if (!(h->nlmsg_flags & NLM_F_MULTI)) {
                        return 0;
                    }
                    continue;
                }

                if ( h->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
                    printf( "error: message truncated");
                    return -1; 
                }
                printf( "error: %s, type=%u, seq=%u, pid=%d", strerror(-err->error),
                        err ->msg.nlmsg_type, err->msg.nlmsg_seq,
                        err->msg.nlmsg_pid ); 
                return -1; 
            }
            error = operation(h, arg);
            if (error != 0)  {
                printf( "filter f unction error\r\n");
                ret = error ; 
            }
        }
    }
	return 0;
}
extern int netlink_open(struct nl_handler *handler, int protocol)
{
	socklen_t socklen;
    int ret;
	int sndbuf = 32768;
	int rcvbuf = 32768;

	memset(handler, 0, sizeof(*handler));

	handler->fd = socket(AF_NETLINK, SOCK_RAW, protocol);
	if (handler->fd < 0)
		return -errno;
#if 0
	if (setsockopt(handler->fd, SOL_SOCKET, SO_SNDBUF,
		       &sndbuf, sizeof(sndbuf)) < 0)
		return -errno;

	if (setsockopt(handler->fd, SOL_SOCKET, SO_RCVBUF,
		       &rcvbuf,sizeof(rcvbuf)) < 0)
		return -errno;
#endif
    ret = fcntl(handler->fd, F_SETFL, O_NONBLOCK);
    if (ret < 0) {
        printf("Can't set socket flags: %s", strerror(errno));
        close (handler->fd);
        return -1;
    }
	memset(&handler->local, 0, sizeof(handler->local));
	handler->local.nl_family = AF_NETLINK;
	handler->local.nl_groups = 0;

	if (bind(handler->fd, (struct sockaddr*)&handler->local,
		 sizeof(handler->local)) < 0)
		return -errno;

	socklen = sizeof(handler->local);
	if (getsockname(handler->fd, (struct sockaddr*)&handler->local,
			&socklen) < 0)
		return -errno;

	if (socklen != sizeof(handler->local))
		return -EINVAL;

	if (handler->local.nl_family != AF_NETLINK)
		return -EINVAL;

	handler->seq = time(NULL);

	return 0;
}

extern int netlink_close(struct nl_handler *handler)
{
	close(handler->fd);
	handler->fd = -1;
	return 0;
}

