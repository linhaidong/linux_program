/*************************************************************************
    > File Name: network.h
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年03月14日 星期二 14时12分45秒
 ************************************************************************/
#ifndef MATRIX_NETWORK_H
#define MATRIX_NETWORK_H

#include <arpa/inet.h>
typedef struct network_conf
{
    int proto;
    int port;
    struct network_conf * next;
}network_conf;

typedef struct mrx_network
{
    char * brname;
    char * r_eth;
    char * v_eth;
    int  prot;
    int   r_port;
    int   v_port;
    struct in_addr v_addr;
    struct in_addr b_addr;
}mrx_network;
extern int  br_init(void);
extern void br_shutdown(void);
extern int br_add_bridge(const char *brname);
extern int br_del_bridge(const char *brname);
extern int br_add_interface(const char *bridge, const char *dev);
extern int br_del_interface(const char *bridge, const char *dev);

extern int network_veth_create(const char *name1, const char *name2);


extern int  netdev_delete_by_name(const char *name);

extern  int ipv4_addr_add(const char * name, struct in_addr *addr,struct in_addr *bcast, int prefix);

extern int network_dev_up(const char *name);
extern int network_dev_down(const char *name);



extern int add_default_route(struct in_addr *addr);

extern int netdev_move_to_namespace(const char* ifname, pid_t pid);
#endif

