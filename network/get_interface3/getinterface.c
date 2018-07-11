/*************************************************************************
    > File Name: getinterface.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2017年04月25日 星期二 15时49分13秒
    > Abstract: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
extern char *gethostname_a(void){
  char *name;
  int r, len = 128;

  name = (char *)malloc(len);
  while((r = gethostname(name, len)) < 0){
    if(errno == ENAMETOOLONG){
      len *= 2;
      name = (char *)realloc(name, len);
    }
    else {
      break;
    }
  }

  if(r < 0)
    return NULL;
  else
    return name;
}

extern unsigned int gethostaddr(void){
  int i = 0;
  char *hostname;
  struct hostent *hostinfo;


  char * paddr;
  hostname = gethostname_a();
  printf("host name is %s\r\n", hostname);
  hostinfo = gethostbyname (hostname);
  printf("num is %d\r\n", hostinfo->h_length);
  printf("address is %d\r\n", *(unsigned int *) hostinfo->h_addr);
  printf("host name is %s\r\n", hostinfo->h_name);
  paddr = hostinfo->h_addr;
  while (paddr)
  {
    printf("IPAddress:%s\r\n", inet_ntoa(*((struct in_addr*)paddr)));
    paddr = hostinfo->h_addr_list[++i]; 
  }
  return 0;
}
 
int main()
{
    gethostaddr();
    return 0;
}
