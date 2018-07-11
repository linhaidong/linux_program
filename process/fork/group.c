/*************************************************************************
    > File Name: group.c
    > Author: lin_engier
    > Mail: linengier@126.com 
    > Created Time: 2017年01月03日 星期二 19时19分22秒
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
/*register at atexit()*/
int plat_create_process()
{
    pid_t pid;
    pid = fork();
    if(pid < 0)
        exit(-1);
    if(pid == 0)
        return 0;
    else if(pid > 0)
        return pid;
    return -1;
}
int plat_wait_process(int * status)
{
     return  waitpid(-1, status, WNOHANG);
}
void sig_handler(int sig)
{
    printf("child exit\r\n");
}
int main()
{
    int pid;
    pid = plat_create_process();
    if(pid == 0)
    {
        signal(SIGCHLD, sig_handler);
        printf("child process\r\n");
        setpgid(0,0);
        printf("pid of process is %d\r\n", getpid());
        printf("pgid of process is %d\r\n", getpgid(0));
        system("./test.py");
    }
    else if(pid > 0)
    {
        printf("process process\r\n");
        int status;
        int ret;
        while(1)
        {
            ret = plat_wait_process(&status);
            if(ret < 0)
                sleep(1);
            else if(ret > 0)
            {
                printf("pid %d exit\r\n", ret);
                continue;
            }
        }
    }
    return 0;
}
