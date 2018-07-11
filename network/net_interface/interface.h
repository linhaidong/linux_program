/*************************************************************************
    > File Name: interface.h
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年01月10日 星期二 15时40分33秒
 ************************************************************************/
#ifndef MATRIX_INTERFACE_H
#define MATRIX_INTERFACE_H
#include <arpa/inet.h>
typedef struct  dnat_rule
{
    const char * dev_name;
    int dport;
    struct in_addr addr;
    int cport;
}dnat_rule;

typedef struct  snat_rule
{
    const char * interface;
    struct in_addr addr;
    struct in_addr mask;
}snat_rule;

extern int netspace_create(const char * name);
extern int netspace_del(const char * name);
extern int virtual_interface_create(const char * if_e, const char *if_v);
extern int virtual_interface_del(const char * if_e);
extern int interface_set_addr(const char * interface, struct in_addr addr);
extern int interface_del_addr(const char * interface);
extern int interface_to_namespace(const char * name_space, const char * interface);
extern int interface_set_up(const char * interface);
extern int def_router_set(struct in_addr addr);
extern char *dnat_make_cmd(dnat_rule *rule);
extern char *snat_make_cmd(snat_rule *rule);
extern int dnat_rule_set(dnat_rule *rule, char ** cmd_data, int *len);
extern int snat_rule_set(snat_rule * rule, char **cmd_data, int *len);
extern int nat_rule_del(const char * cmd_data);
#endif
