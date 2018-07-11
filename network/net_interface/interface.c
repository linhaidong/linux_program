/*************************************************************************
    > File Name: interface.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年01月09日 星期一 18时46分49秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>          
#include <sys/socket.h>

#include <netinet/in.h>
#include "interface.h"
#define DEFAULT_CMD_SIZE  128

int netspace_create(const char * name)
{
    int ret = 0;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!name)
        return -1;
    sprintf(cmd, "ip netns add %s", name);
    ret = system(cmd);
    return ret;
}
int netspace_del(const char * name)
{
    int ret = 0;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!name)
        return -1;
    sprintf(cmd, "ip netns delete %s", name);
    ret = system(cmd);
    return ret;
}
#if 0
int netspace_get(const char * name)
{
    return 0;
}
#endif
/* --------------------------------------------------------------------------*/
/**
 * @cmd:      ip link add veth0 type veth peer name vveth0
 * @Synopsis  
 */
/* ----------------------------------------------------------------------------*/
int virtual_interface_create(const char * if_e, const char *if_v)
{
    int ret = 0;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!if_e || !if_v)
        return -1;
    sprintf(cmd, "ip link add %s type veth peer name %s", if_e, if_v);
    ret = system(cmd);
    return ret;
}
 /* --------------------------------------------------------------------------*/
/**
 *
 *
 *
 * @cmd:     ip link delete v
 * @Synopsis  
 *
 * @Param if_e
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
int virtual_interface_del(const char * if_e)
{
    int ret = 0;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!if_e)
        return -1;
    sprintf(cmd, "ip link delete %s", if_e);
    ret = system(cmd);
    return ret;
}
/* --------------------------------------------------------------------------*/
/**i
 *
 * cmd:      ip netns exec test ip address add 192.168.8.20/24 dev vveth0
 * @Synopsis  
 *
 * @Param name
 * @Param addr
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
int interface_set_addr(const char * interface, struct in_addr addr)
{
    int ret;
    char cmd[DEFAULT_CMD_SIZE]={0};
    char * address = NULL;
    address = inet_ntoa(addr);
    sprintf(cmd, "ip address add %s/24 dev %s", address, interface);
    ret = system(cmd);
    return 0;
}
/* --------------------------------------------------------------------------*/
/**
 * @cmd        ip address flush dev vveth0
 * @Synopsis  
 *
 * @Param interface
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
int interface_del_addr(const char * interface)
{
    int ret;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!interface)
        return -1;
    sprintf(cmd, "ip address flush  dev %s", interface);
    ret = system(cmd);
    return ret;
}

/* --------------------------------------------------------------------------*/
/**i
 * @cmd:        ip link set dev vveth0 netns test
 * @Synopsis  
 *
 * @Param name_space
 * @Param interface
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
int interface_to_namespace(const char * name_space, const char * interface)
{
    int ret = 0;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!name_space || !interface)
        return -1;
    sprintf(cmd, "ip link set dev %s netns %s", interface, name_space);
    ret = system(cmd);
    return ret;


}
/* --------------------------------------------------------------------------*/
/**
 * @cmd:     ip link  set  veth0 up 
 * @Synopsis  
 *
 * @Param name
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
int interface_set_up(const char * interface)
{
    int ret;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!interface)
        return -1;
    sprintf(cmd, "ip link set %s up", interface);
    ret = system(cmd);
    return ret;
}


/* --------------------------------------------------------------------------*/
/**
 * @cmd:    ip netns exec test ip route add  default via 192.168.8.1
 * @Synopsis  
 *
 * @Param addr
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
int def_router_set(struct in_addr addr)
{
    int ret;
    char cmd[DEFAULT_CMD_SIZE]={0};
    char * address = NULL;
    address = inet_ntoa(addr);
    if(!address)
        return -1;
    sprintf(cmd, "ip route add default via %s", address);
    ret = system(cmd);
    return 0;
}

/* --------------------------------------------------------------------------*/
/**i
 * @cmd       iptables -t nat -A PREROUTING -p tcp --dport 80 -i eth0  -j DNAT --to 5.6.7.8:8080
 * @Synopsis  change distination of packet in interface  dev_name, this is for incoming data
 *
 * @Param name
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
#if 0
to do
实现统一接口
typedef enum nat_rule
{
    struct snat_rule snat;
    struct dnat_rule dnat;
}nat_rule;
#endif
char *dnat_make_cmd(dnat_rule *rule)
{
    char *cmd;
    char *cip;
    cip = inet_ntoa(rule->addr);
    if(!cip)
        return NULL;
    cmd = (char *)calloc(DEFAULT_CMD_SIZE, sizeof(char));
    if(!cmd)
        return NULL;
    sprintf(cmd," PREROUTING -p tcp --dport %d -i %s  -j DNAT --to %s:%d",
            rule->dport, rule->dev_name, cip, rule->cport);
    return cmd;
}

char *snat_make_cmd(snat_rule *rule)
{
    char *cmd;
    char *sip;
    cmd = (char *)calloc(DEFAULT_CMD_SIZE, sizeof(char));
    if(!cmd)
        return NULL;
    sip = inet_ntoa(rule->addr);
    sprintf(cmd," POSTROUTING -s %s -o %s -j MASQUERADE",
            sip, rule->interface);
    return cmd;
}


int dnat_rule_set(dnat_rule *rule, char ** cmd_data, int *len)
{
    int ret;
    char *data;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!rule || !cmd_data)
        return -1;
    data = dnat_make_cmd(rule);
    if(!data)
        return -1;
    sprintf(cmd,"iptables -t nat -A %s", data);
    ret = system(cmd);
    if(ret > 0)
    {
        *cmd_data = data;
        *len = strlen(data);
        return 0;
    }
    return -1;
}

int snat_rule_set(snat_rule * rule, char **cmd_data, int *len)
{
    int ret;
    char *data;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!rule || !cmd_data)
        return -1;
    data = snat_make_cmd(rule);
    if(!data)
        return -1;
    sprintf(cmd,"iptables -t nat -A %s", data);
    ret = system(cmd);
    if(ret > 0)
    {
        *cmd_data = data;
        *len = strlen(data);
        return 0;
    }
    return -1;
}
    
int nat_rule_del(const char * cmd_data)
{
    int ret;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!cmd_data)
        return -1;
    sprintf(cmd, "iptables -t nat -D %s",cmd_data);
    ret = system(cmd);
    return ret;
}
