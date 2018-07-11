/*************************************************************************
    > File Name: interface3.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2017年12月12日 15时57分29秒 CST
    > Abstract: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
char * get_local_mac_by_name(const char *name)
{
    struct ifreq ifreq; 
    int sock;
    char * mac_addr;
    if((sock=socket(AF_INET,SOCK_STREAM,0)) <0) { 
        return NULL; 
    } 
    strcpy(ifreq.ifr_name, name); 
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) <0) { 
        return NULL; 
    } 
    mac_addr = (char *)calloc(32, sizeof(char));
    if(mac_addr) {
        sprintf(mac_addr, "%02X-%02X-%02X-%02X-%02X-%02X", 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[0], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[1], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[2], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[3], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[4], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[5]); 
    }
    return  mac_addr; 
}
int main()
{
    char buf[218] = {0};
    FILE *pf = popen("ifconfig -a", "r");
    
    if (!pf){
        printf("popen failed\r\n");
        return -1;
    }
    
    while(fgets(buf, sizeof(buf), pf) != NULL ){
        if (strstr(buf, "Link encap:Ethernet")){
            printf("buf=%s\r\n", buf);
            char *p= buf;
            int i = 0;
            for ( i=0; i < strlen(buf); i++) {
                if (buf[i] == ' ') {
                    buf[i] = '\0';
                    break;
                }
            }
            break;
        }
    }
    char * name = strdup(buf);
    printf("name is %s\r\n", name);
    char * mac =  get_local_mac_by_name(name);
    printf("mac add = %s\r\n", mac);
    if (mac) free(mac);
    free(name);
    pclose(pf);

}
