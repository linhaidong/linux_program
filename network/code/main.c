/************************************************************************
    > File Name: main.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年03月14日 星期二 14时16分13秒
 ************************************************************************/
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "network.h"
#include "iptable.h"
#define STACK_SIZE (1024 * 10240)

// sync primitive
int checkpoint[2];

static char child_stack[STACK_SIZE];


typedef struct network_param
{
    int   socket;
    int   prefix;
    struct in_addr addr;
    struct in_addr bcast;
    struct in_addr gateway;
    const char * eth_name;
}network_param;

typedef struct mat_br
{
    char * name;
    int prefix;
    struct in_addr addr;
    struct in_addr bcast;
}mat_br;

int child_main(void* arg)
{
    char  buf[10]= {0};
    int pid;
    network_param * param = (network_param *)arg;
    /*wait parent add interface*/
    read(param->socket,buf,sizeof(buf));
    if(strncmp(buf, "network", 7) != 0)
    {
        printf("get cmd error\r\n");
        exit(0);
    }
    network_dev_up("lo");
    /*set ip and up interface*/
    ipv4_addr_add(param->eth_name, &param->addr, &param->bcast, param->prefix);
    network_dev_up(param->eth_name);

    /*set default router to bridge ip*/
    add_default_route(&param->gateway); 
    //system("route add default gw 172.16.0.1");
    system("ifconfig");
    system("route");
    printf("child process run......\r\n");
    pid = fork();
    if(pid < 0)
    {

    }
    else if(pid == 0)
    {
        /*exec in init process, may be influate environment*/
        execl("/bin/server","server", NULL);
    }
    else
    {
        while(1)
        {
            read(param->socket,buf,sizeof(buf));
            if(strncmp(buf, "stop", 4) != 0)
            {
                printf("get exit error\r\n");
                exit(0);
            }
        }
    }
    //execl("/bin/ping", "ping", "172.16.0.1", NULL);
    return 0;
} 

int main(int argc, char * argv[])
{
    int pid = 0;
    char buf[10];
    
    int sockets[2];
    dnat_rule dnat;
    snat_rule snat;


    network_param param;
    mat_br br;

    br.name = "test";
    const char * eth = "e_i", * veth = "v_i";
    
    br_init();
    br_add_bridge(br.name);

#if 0
    if(inet_aton("172.16.0.1", &br.addr) == 0)
    {
        printf("inet_aton error\r\n");
    }
    if(inet_aton("172.16.0.255", &br.bcast) == 0)
    {
        printf("inet_aton error\r\n");
    }
    br.prefix = 24;
    ipv4_addr_add(br.name, &br.addr, &br.bcast, br.prefix);
    network_dev_up(br.name);
#if 0
    /*add snat rule */
    memset(&snat, 0, sizeof(snat));
    snat.interface = "eth0";
    snat.addr = br.addr;
    snat.prefix = br.prefix;
    snat_rule_set(&snat);
#endif
    /*add virtual pair of interface */
    if(network_veth_create(eth, veth) != 0)
    {

        printf("create veth error \r\n");
        goto error1;

    }
    network_dev_up(eth);
    
    /*add vittual interface to bridge*/
    br_add_interface(br.name, eth);
    
    
    
    param.eth_name =  veth;
    param.gateway = br.addr;
    param.bcast = br.bcast;
    param.prefix = br.prefix;
    /*chose ip for child virtual interface*/
    inet_aton("172.16.0.2",&param.addr); 
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
        printf("error %d on socketpair:%s ", errno, strerror(errno));
        goto error2;
    }
    param.socket = sockets[1];
    /*child*/
    pid = clone(child_main, child_stack+STACK_SIZE, CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD, &param);
    if(pid == -1)
    {
        printf("create child process error\r\n");
        goto error2;
    }
    /*parent*/
    /*add virtual interface to child namespace*/
    netdev_move_to_namespace(veth, pid);

#if 0
    /*set iptables rule*/
    memset(&dnat, 0, sizeof(dnat));
    dnat.proto = PROTO_TCP;
    /*chose prot for child process*/
    dnat.dport = 7777;
    dnat.addr = param.addr;
    //inet_aton("172.16.0.2",&dnat.addr); 
    dnat.cport = 9999;
    dnat_rule_set(&dnat);

#endif
    /**/
    write(sockets[0], "network", 7); 
    while(1)
    {
        read(0,buf, 10);
        if(strncmp(buf, "exit", 4) == 0)
        {
            printf("end!!\r\n");
            write(sockets[0], "stop", 4); 
            kill(pid, 9);
            break;
        }
    }
error2:
    if(dnat.cmd)
    {
        nat_rule_del(dnat.cmd);
        free(dnat.cmd);
    }
    if(snat.cmd)
    {
        nat_rule_del(snat.cmd);
        free(snat.cmd);
    }
    network_dev_down(eth);
    netdev_delete_by_name(eth);
error1:
    /*delete virtual interface pair*/
    network_dev_down(br.name);
#endif
    br_del_bridge(br.name);
#if 0
   if(strcmp(argv[1], "cre") == 0)
       br_add_bridge("test");
   if(strcmp(argv[1], "del") == 0)
       br_del_bridge("test");
   if(strcmp(argv[1], "addr") == 0)
   {
      struct in_addr addr, mask;
      if(inet_aton("172.16.0.1", &addr) == 0)
      {
          printf("inet_aton error\r\n");
      }
      if(inet_aton("255.255.0.0", &mask) == 0)
      {
          printf("inet_aton error\r\n");
      }
      ipv4_addr_add("test", &addr, &mask, 0);
      network_dev_up("test");
   }
   if(strcmp(argv[1], "veth") == 0)
   {
       printf("create veth\r\n");
       if(network_veth_create("e_mar", "v_mar") != 0)
           printf("create veth error \r\n");
       network_dev_up("e_mar");
       //br_add_interface("test", "e_mar");
   }
   /*
      root@lin-ThinkPad-X240:~# ls -alt  /proc/14920/ns/net 
      lrwxrwxrwx 1 root root 0  3月 14 17:15 /proc/14920/ns/net -> net:[4026532417]
    */
#endif
   return 0;
}
