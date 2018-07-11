/*************************************************************************
    > File Name: network.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年03月14日 星期二 13时51分44秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_bridge.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/sockios.h>
#include <net/route.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <arpa/inet.h>
#include "nl.h"

#define mask_in_addr(x) (((struct sockaddr_in *)&((x).rt_genmask))->sin_addr.s_addr)
#define full_mask(x) (x)

#ifndef VETH_INFO_PEER
# define VETH_INFO_PEER 1
#endif

int br_socket_fd = -1;

int br_init(void)
{
	if ((br_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
		return errno;
	return 0;
}

void br_shutdown(void)
{
	close(br_socket_fd);
	br_socket_fd = -1;
}


int br_add_bridge(const char *brname)
{
	int ret;

#ifdef SIOCBRADDBR
	ret = ioctl(br_socket_fd, SIOCBRADDBR, brname);
	if (ret < 0)
#endif
	{
		char _br[IFNAMSIZ];
		unsigned long arg[3] = { BRCTL_ADD_BRIDGE, (unsigned long) _br };

		strncpy(_br, brname, IFNAMSIZ);
		ret = ioctl(br_socket_fd, SIOCSIFBR, arg);
	} 

	return ret < 0 ? errno : 0;
}

int br_del_bridge(const char *brname)
{
	int ret;

#ifdef SIOCBRDELBR	
	ret = ioctl(br_socket_fd, SIOCBRDELBR, brname);
	if (ret < 0)
#endif
	{
		char _br[IFNAMSIZ];
		unsigned long arg[3] = { BRCTL_DEL_BRIDGE, (unsigned long) _br };

		strncpy(_br, brname, IFNAMSIZ);
		ret = ioctl(br_socket_fd, SIOCSIFBR, arg);
	} 
	return  ret < 0 ? errno : 0;
}

int br_add_interface(const char *bridge, const char *dev)
{
	struct ifreq ifr;
	int err;
	int ifindex = if_nametoindex(dev);

	if (ifindex == 0) 
		return ENODEV;
	
	strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
#ifdef SIOCBRADDIF
	ifr.ifr_ifindex = ifindex;
	err = ioctl(br_socket_fd, SIOCBRADDIF, &ifr);
	if (err < 0)
#endif
	{
		unsigned long args[4] = { BRCTL_ADD_IF, ifindex, 0, 0 };
					  
		ifr.ifr_data = (char *) args;
		err = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
	}

	return err < 0 ? errno : 0;
}

int br_del_interface(const char *bridge, const char *dev)
{
	struct ifreq ifr;
	int err;
	int ifindex = if_nametoindex(dev);

	if (ifindex == 0) 
		return ENODEV;
	
	strncpy(ifr.ifr_name, bridge, IFNAMSIZ);
#ifdef SIOCBRDELIF
	ifr.ifr_ifindex = ifindex;
	err = ioctl(br_socket_fd, SIOCBRDELIF, &ifr);
	if (err < 0)
#endif		
	{
		unsigned long args[4] = { BRCTL_DEL_IF, ifindex, 0, 0 };
					  
		ifr.ifr_data = (char *) args;
		err = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
	}

	return err < 0 ? errno : 0;
}

int add_default_route(struct in_addr *addr)
{
    int ret;
    int skfd;
    struct sockaddr_in * rt_addr;
	char rt_buf[sizeof(struct rtentry)];
	struct rtentry *const rt = (struct rtentry *)rt_buf;
	memset(rt, 0, sizeof(*rt));
   

    rt_addr = (struct sockaddr_in *)&rt->rt_dst;

    rt_addr->sin_family = AF_INET;
    rt_addr->sin_port = 0;
	rt_addr->sin_addr.s_addr = INADDR_ANY;

    rt_addr = (struct sockaddr_in *)&rt->rt_gateway;

    rt_addr->sin_family = AF_INET;
    rt_addr->sin_port = 0;
    rt_addr->sin_addr = *addr;

    
    //rt->rt_flags =  RTF_HOST;
	rt->rt_flags |= RTF_GATEWAY;

	//mask_in_addr(*rt) = 0;
    rt_addr = (struct sockaddr_in *)&rt->rt_genmask;

   // r sockaddr_in *)&((x).rt_genmask))->sin_addr.s_addr)t_addr->sin_family = AF_INET;
   // rt_addr->sin_port = 0;
   // inet_aton("0.0.0.0", &rt_addr->sin_addr);

//    rt_addr->sin_addr.s_addr = 0;
//

	int prefix_len = 0;
    mask_in_addr(*rt) = htonl( ~(0xffffffffUL >> prefix_len));


//
	/* Create a socket to the INET kernel. */
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	ret = ioctl(skfd, SIOCADDRT, rt);
    if(ret != 0)
        printf("ser error:%s\r\n", strerror(errno));
    close(skfd);
}

int netdev_set_flag(const char *name, int flag)
{
	struct nl_handler nlh;
	struct nlmsg *nlmsg = NULL, *answer = NULL;
	struct ifinfomsg *ifi;
	int index, len, err;

	err = netlink_open(&nlh, NETLINK_ROUTE);
	if (err)
		return err;

	err = -EINVAL;
	len = strlen(name);
	if (len == 1 || len >= IFNAMSIZ)
		goto out;

	err = -ENOMEM;
	nlmsg = nlmsg_alloc(NLMSG_GOOD_SIZE);
	if (!nlmsg)
		goto out;

	answer = nlmsg_alloc_reserve(NLMSG_GOOD_SIZE);
	if (!answer)
		goto out;

	err = -EINVAL;
	index = if_nametoindex(name);
	if (!index)
		goto out;

	nlmsg->nlmsghdr->nlmsg_flags = NLM_F_REQUEST|NLM_F_ACK;
	nlmsg->nlmsghdr->nlmsg_type = RTM_NEWLINK;

	ifi = nlmsg_reserve(nlmsg, sizeof(struct ifinfomsg));
	ifi->ifi_family = AF_UNSPEC;
	ifi->ifi_index = index;
	ifi->ifi_change |= IFF_UP;
	ifi->ifi_flags |= flag;

	err = netlink_transaction(&nlh, nlmsg, answer);
out:
	netlink_close(&nlh);
	nlmsg_free(nlmsg);
	nlmsg_free(answer);
	return err;
}



int network_dev_up(const char *name)
{
	return netdev_set_flag(name, IFF_UP);
}

int network_dev_down(const char *name)
{
	return netdev_set_flag(name, 0);
}



static int netdev_delete_by_index(int ifindex)
{
	struct nl_handler nlh;
	struct nlmsg *nlmsg = NULL, *answer = NULL;
	struct ifinfomsg *ifi;
	int err;

	err = netlink_open(&nlh, NETLINK_ROUTE);
	if (err)
		return err;

	err = -ENOMEM;
	nlmsg = nlmsg_alloc(NLMSG_GOOD_SIZE);
	if (!nlmsg)
		goto out;

	answer = nlmsg_alloc_reserve(NLMSG_GOOD_SIZE);
	if (!answer)
		goto out;

	nlmsg->nlmsghdr->nlmsg_flags = NLM_F_ACK|NLM_F_REQUEST;
	nlmsg->nlmsghdr->nlmsg_type = RTM_DELLINK;

	ifi = nlmsg_reserve(nlmsg, sizeof(struct ifinfomsg));
	ifi->ifi_family = AF_UNSPEC;
	ifi->ifi_index = ifindex;

	err = netlink_transaction(&nlh, nlmsg, answer);
out:
	netlink_close(&nlh);
	nlmsg_free(answer);
	nlmsg_free(nlmsg);
	return err;
}

int netdev_delete_by_name(const char *name)
{
	int index;

	index = if_nametoindex(name);
	if (!index)
		return -EINVAL;

	return netdev_delete_by_index(index);
}


int netdev_move_to_namespace(const char* ifname, pid_t pid)
{
	struct nl_handler nlh;
	struct nlmsg *nlmsg = NULL;
	struct ifinfomsg *ifi;
	int err;

    if(!ifname)
        return -1;
    
    int ifindex;
    ifindex = if_nametoindex(ifname);
    if(ifindex == 0)
        return -1;
	
    err = netlink_open(&nlh, NETLINK_ROUTE);
	if (err)
		return err;

	err = -ENOMEM;
	nlmsg = nlmsg_alloc(NLMSG_GOOD_SIZE);
	if (!nlmsg)
		goto out;

	nlmsg->nlmsghdr->nlmsg_flags = NLM_F_REQUEST|NLM_F_ACK;
	nlmsg->nlmsghdr->nlmsg_type = RTM_NEWLINK;

	ifi = nlmsg_reserve(nlmsg, sizeof(struct ifinfomsg));
	ifi->ifi_family = AF_UNSPEC;
	ifi->ifi_index = ifindex;

	if (nla_put_u32(nlmsg, IFLA_NET_NS_PID, pid))
		goto out;

	if (ifname != NULL) {
		if (nla_put_string(nlmsg, IFLA_IFNAME, ifname))
			goto out;
	}

	err = netlink_transaction(&nlh, nlmsg, nlmsg);
out:
	netlink_close(&nlh);
	nlmsg_free(nlmsg);
	return err;
}



int network_veth_create(const char *name1, const char *name2)
{
	struct nl_handler nlh;
	struct nlmsg *nlmsg = NULL, *answer = NULL;
	struct ifinfomsg *ifi;
	struct rtattr *nest1, *nest2, *nest3;
	int len, err;

	err = netlink_open(&nlh, NETLINK_ROUTE);
	if (err)
		return err;

	err = -EINVAL;
	len = strlen(name1);
	if (len == 1 || len >= IFNAMSIZ)
		goto out;

	len = strlen(name2);
	if (len == 1 || len >= IFNAMSIZ)
		goto out;

	err = -ENOMEM;
	nlmsg = nlmsg_alloc(NLMSG_GOOD_SIZE);
	if (!nlmsg)
		goto out;

	answer = nlmsg_alloc_reserve(NLMSG_GOOD_SIZE);
	if (!answer)
		goto out;

	nlmsg->nlmsghdr->nlmsg_flags =
		NLM_F_REQUEST|NLM_F_CREATE|NLM_F_EXCL|NLM_F_ACK;
	nlmsg->nlmsghdr->nlmsg_type = RTM_NEWLINK;

	ifi = nlmsg_reserve(nlmsg, sizeof(struct ifinfomsg));
	ifi->ifi_family = AF_UNSPEC;

	err = -EINVAL;
	nest1 = nla_begin_nested(nlmsg, IFLA_LINKINFO);
	if (!nest1)
		goto out;

	if (nla_put_string(nlmsg, IFLA_INFO_KIND, "veth"))
		goto out;

	nest2 = nla_begin_nested(nlmsg, IFLA_INFO_DATA);
	if (!nest2)
		goto out;

	nest3 = nla_begin_nested(nlmsg, VETH_INFO_PEER);
	if (!nest3)
		goto out;

	ifi = nlmsg_reserve(nlmsg, sizeof(struct ifinfomsg));
	if (!ifi)
		goto out;

	if (nla_put_string(nlmsg, IFLA_IFNAME, name2))
		goto out;

	nla_end_nested(nlmsg, nest3);

	nla_end_nested(nlmsg, nest2);

	nla_end_nested(nlmsg, nest1);

	if (nla_put_string(nlmsg, IFLA_IFNAME, name1))
		goto out;

	err = netlink_transaction(&nlh, nlmsg, answer);
out:
	netlink_close(&nlh);
	nlmsg_free(answer);
	nlmsg_free(nlmsg);
	return err;
}



static int ip_addr_add(int family, int ifindex,
		       void *addr, void *bcast, void *acast, int prefix)
{
	struct nl_handler nlh;
	struct nlmsg *nlmsg = NULL, *answer = NULL;
	struct ifaddrmsg *ifa;
	int addrlen;
	int err;

	addrlen = family == AF_INET ? sizeof(struct in_addr) :
		sizeof(struct in6_addr);

	err = netlink_open(&nlh, NETLINK_ROUTE);
	if (err)
		return err;

	err = -ENOMEM;
	nlmsg = nlmsg_alloc(NLMSG_GOOD_SIZE);
	if (!nlmsg)
		goto out;

	answer = nlmsg_alloc_reserve(NLMSG_GOOD_SIZE);
	if (!answer)
		goto out;

	nlmsg->nlmsghdr->nlmsg_flags =
		NLM_F_ACK|NLM_F_REQUEST|NLM_F_CREATE|NLM_F_EXCL;
	nlmsg->nlmsghdr->nlmsg_type = RTM_NEWADDR;

	ifa = nlmsg_reserve(nlmsg, sizeof(struct ifaddrmsg));
	ifa->ifa_prefixlen = prefix;
	ifa->ifa_index = ifindex;
	ifa->ifa_family = family;
	ifa->ifa_scope = 0;
	
	err = -EINVAL;
	if (nla_put_buffer(nlmsg, IFA_LOCAL, addr, addrlen))
		goto out;

	if (nla_put_buffer(nlmsg, IFA_ADDRESS, addr, addrlen))
		goto out;

	if (nla_put_buffer(nlmsg, IFA_BROADCAST, bcast, addrlen))
		goto out;

	/* TODO : multicast, anycast with ipv6 */
	err = -EPROTONOSUPPORT;
	if (family == AF_INET6 &&
	    (memcmp(bcast, &in6addr_any, sizeof(in6addr_any)) ||
	     memcmp(acast, &in6addr_any, sizeof(in6addr_any))))
		goto out;

	err = netlink_transaction(&nlh, nlmsg, answer);
out:
	netlink_close(&nlh);
	nlmsg_free(answer);
	nlmsg_free(nlmsg);
	return err;
}


int ipv4_addr_add(const char * name, struct in_addr *addr, struct in_addr *bcast, int prefix)
{
    int ifindex;
    ifindex = if_nametoindex(name);
    if(ifindex == 0)
        return -1;
	return ip_addr_add(AF_INET, ifindex, addr, bcast, NULL, prefix);
}



