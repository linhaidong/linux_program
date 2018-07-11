/*************************************************************************
    > File Name: interface.h
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年01月10日 星期二 15时40分33秒
 ************************************************************************/
#ifndef MATRIX_INTERFACE_H
#define MATRIX_INTERFACE_H
#include <arpa/inet.h>
typedef enum net_proto
{
    PROTO_TCP =0,
    PROTO_UDP
}net_proto;
typedef struct  dnat_rule
{
    net_proto  proto;
    int dport;
    struct in_addr addr;
    int cport;
    char *cmd;
    int cmd_len;
}dnat_rule;

typedef struct  snat_rule
{
    const char * interface;
    int prefix;
    struct in_addr addr;
    char *cmd;
    int cmd_len;
}snat_rule;

extern int dnat_rule_set(dnat_rule *rule);
extern int snat_rule_set(snat_rule * rule);
extern int nat_rule_del(const char * cmd_data);
#endif
