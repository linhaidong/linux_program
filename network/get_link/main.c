/*************************************************************************
    > File Name: main.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2017年04月25日 星期二 10时58分04秒
    > Abstract: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include "nl.h"


int main()
{
	struct nl_handler nlh;
	struct nlmsg *nlmsg = NULL, *answer = NULL;
	struct ifinfomsg *ifi;
    struct rtgenmsg *msg;
	int index, len, err;

	err = netlink_open(&nlh, NETLINK_ROUTE);
	if (err)
		return err;

	err = -EINVAL;
#if 0
	len = strlen(name);
	if (len == 1 || len >= IFNAMSIZ)
		goto out;
#endif
	err = -ENOMEM;
	nlmsg = nlmsg_alloc(NLMSG_GOOD_SIZE);
	if (!nlmsg)
		goto out;

	//answer = nlmsg_alloc_reserve(NLMSG_GOOD_SIZE);
	answer = nlmsg_alloc_reserve(4096);
	if (!answer)
		goto out;
#if 0
	err = -EINVAL;
	index = if_nametoindex(name);
	if (!index)
		goto out;
#endif
    /*set message type for message head*/
    nlmsg->nlmsghdr->nlmsg_pid = 0;
	nlmsg->nlmsghdr->nlmsg_flags = NLM_F_REQUEST | NLM_F_MATCH | NLM_F_ROOT;
	nlmsg->nlmsghdr->nlmsg_type = RTM_GETLINK;
	nlmsg->nlmsghdr->nlmsg_seq = nlh.seq;
    /*set message data for special*/
	msg = (struct rtgenmsg *)nlmsg_reserve(nlmsg, sizeof(struct rtgenmsg));
	msg->rtgen_family = AF_INET;
	
    
    err = netlink_transaction_getinfo(&nlh, nlmsg, answer);
out:
	netlink_close(&nlh);
	nlmsg_free(nlmsg);
	nlmsg_free(answer);
	return err;
}

