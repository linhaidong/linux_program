/*************************************************************************
    > File Name: net_space.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年01月09日 星期一 17时46分56秒
 ************************************************************************/
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "interface.h"
#define STACK_SIZE (1024 * 1024)

// sync primitive
int checkpoint[2];

static char child_stack[STACK_SIZE];
char* const child_args[] = {
  "/bin/bash",
  NULL
};
typedef struct child_argv
{
    char * interface_name;
    struct in_addr address;
    struct in_addr router;
}child_argv;


int child_main(void* arg)
{
  char c;
  child_argv *param = (child_argv *)arg;
  // init sync primitive
  close(checkpoint[1]);

  // setup hostname
  printf(" - [%5d] World !\n", getpid());
  sethostname("In Namespace", 12);

  // remount "/proc" to get accurate "top" && "ps" output
  mount("proc", "/proc", "proc", 0, NULL);

  // wait for network setup in parent
  read(checkpoint[0], &c, 1);

  // setup network
  interface_set_up("lo");
  interface_set_addr(param->interface_name, param->address);
  interface_set_up(param->interface_name);
  def_router_set(param->router);
#if 0
  system("ip link set lo up");
  system("ip link set veth11 up");
  system("ip addr add 169.254.1.2/30 dev veth11");
#endif
  execv(child_args[0], child_args);
  printf("Ooops\n");
  return 1;
}
int main()
{
    child_argv c_argv;
    char space_name[10]={0};
    char *ve_name = "veth0";
    char *vv_name = "vveth0";
    char *vveth0_addr = "192.168.8.2";
    char *veth0_addr = "192.168.8.1";
 
    snat_rule s_rule;
    dnat_rule d_rule;
    char *snat_str = NULL, *dnat_str = NULL;
    int snat_len, dnat_len;
    struct in_addr ve_ip, vve_ip;
    
    
    
    if(inet_aton(vveth0_addr, &vve_ip) == 0)
    {
        printf("address error\r\n");
        return -1;
    }
    if(inet_aton(veth0_addr, &ve_ip) == 0)
    {
        printf("address error\r\n");
        return -1;
    }
    s_rule.interface = d_rule.dev_name = "eth0";
    s_rule.addr = vve_ip;

    d_rule.dport = d_rule.cport = 9999;
    d_rule.addr = vve_ip;
    //create namespace
    c_argv.interface_name = vv_name;
    c_argv.address = vve_ip;
    c_argv.router = ve_ip; 
    // init sync primitive
    pipe(checkpoint);

    printf(" - [%5d] Hello ?\n", getpid());

    int child_pid = clone(child_main, child_stack+STACK_SIZE,
            CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD, &c_argv);

    sprintf(space_name, "%d", (int)child_pid);
    printf("child pid is %s\r\n", space_name);
    // further init: create a veth pair
    //create virtual interface
    virtual_interface_create(ve_name, vv_name);
    //add vv_eth0 to namespace
    interface_to_namespace(space_name, vv_name);
    //set interface veth0
    interface_set_addr(ve_name, ve_ip);
    interface_set_up(ve_name);
    
    //add nat rule
    dnat_rule_set(&d_rule, &dnat_str, &dnat_len); 
    snat_rule_set(&s_rule, &snat_str, &snat_len); 
    
    // signal "done"
    close(checkpoint[1]);

    waitpid(child_pid, NULL, 0);
    if(!dnat_str)
        nat_rule_del(dnat_str);
    if(!snat_str)
        nat_rule_del(snat_str);
    virtual_interface_del(ve_name);
    //netspace_del(space_name);
    return 0;
}
