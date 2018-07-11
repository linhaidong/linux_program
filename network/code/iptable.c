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
#include "iptable.h"
#define DEFAULT_CMD_SIZE  128

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
    if(rule->proto == PROTO_TCP)
    sprintf(cmd," PREROUTING -p tcp --dport %d -j DNAT --to-destination %s:%d",
            rule->dport, cip, rule->cport);
    else if(rule->proto == PROTO_UDP)
        sprintf(cmd," PREROUTING -p udp --dport %d  -j DNAT --to-destination %s:%d",
            rule->dport,  cip, rule->cport);

    return cmd;
}

char *snat_make_cmd(snat_rule *rule)
{
    char *cmd;
    char *sip;


    if(!rule->interface || (rule->prefix < 0 || rule->prefix > 32))
        return NULL;
    cmd = (char *)calloc(DEFAULT_CMD_SIZE, sizeof(char));
    if(!cmd)
        return NULL;
    sip = inet_ntoa(rule->addr);
    sprintf(cmd," POSTROUTING -s %s/%d -o %s -j MASQUERADE",
            sip, rule->prefix, rule->interface);
    return cmd;
}


int dnat_rule_set(dnat_rule *rule)
{
    int ret;
    char *data;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!rule)
        return -1;
    data = dnat_make_cmd(rule);
    if(!data)
        return -1;
    sprintf(cmd,"iptables -t nat -A %s", data);
    ret = system(cmd);
    if(ret > 0)
    {
        rule->cmd = data;
        rule->cmd_len = strlen(data);
        return 0;
    }
    return -1;
}

int snat_rule_set(snat_rule * rule)
{
    int ret;
    char *data;
    char cmd[DEFAULT_CMD_SIZE]={0};
    if(!rule)
        return -1;
    data = snat_make_cmd(rule);
    if(!data)
        return -1;
    sprintf(cmd,"iptables -t nat -A %s", data);
    ret = system(cmd);
    if(ret > 0)
    {
        rule->cmd = data;
        rule->cmd_len = strlen(data);
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
