/*************************************************************************
    > File Name: popen.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2018年03月13日 14时56分10秒 CST
    > Abstract: 
 ************************************************************************/
#include <stdio.h>  
#include <errno.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
/* version without signal handling */  
int system_without_signal(const char *cmd_string)  
{  
    pid_t pid;  
    int status = -1;  
  
    if (cmd_string == NULL)  
        return (1);     /* always a command processor with UNIX */  
  
    if ((pid = fork()) < 0) {  
        status = -1;    /* probably out of processes */  
    } else if (pid == 0) {  /* child */  
        execl("/bin/sh", "sh", "-c", cmd_string, (char *)0);  
        _exit(127); /* execl error */  
    } else {                /* parent */  
//      sleep(1);  
        pid_t wait_pid;  
        while ((wait_pid = waitpid(pid, &status, 0)) < 0) {  
            printf("[in system_without_signal]: errno = %d(%s)\n",  
                                        errno, strerror(errno));  
            if (errno != EINTR) {  
                status = -1;    /* error other than EINTR form waitpid() */  
                break;  
            }  
        }  
        printf("[in system_without_signal]: pid = %ld, wait_pid = %ld\n",  
                                        (long)pid, (long)wait_pid);  
        printf("[in system_without_signal] %d\r\n", status);  
    }  
  
    return (status);  
}  

typedef struct cmd_proc {
    pid_t pid;
    int   fd;
    FILE * pf;
}cmd_proc;

static int cmd_exec(const char * cmd, cmd_proc * proc)
{
    int flag;
    pid_t pid;
    int pfd[2];

    if(pipe(pfd))
        return -1;
    flag = fcntl(pfd[1], F_GETFD);
    flag |= FD_CLOEXEC;
    fcntl(pfd[1], F_SETFD, flag);

    pid = fork();
    if(pid < 0){
        close(pfd[0]);
        close(pfd[1]);
        return -1;
    } else if(pid == 0) {
        close(pfd[0]);
        dup2(pfd[1], 1);
        execl("/bin/sh", "sh", "-c", cmd, (char *)0);  
        exit(0);
    } else {
        close(pfd[1]);
        proc->pid = pid;
        proc->fd = pfd[0];
        return 0;
    }
    return -1;
}

static long int get_current_time()
{
    long int time = 0;
    struct timeval tv;  
    gettimeofday(&tv, NULL);  
    time = tv.tv_sec*1000 + (int)tv.tv_usec/1000; 
    return time;
}

int  netdet_popen(FILE ** pf,const char  * cmd,  long int timeout)
{
    int ret;
    int num = 0;
    int read_count = 0;
    char buf[2048];   
    struct timeval t;
    long int time_count1 = 0, time_count2 = 0;
    long int cmd_time = 0;
    fd_set set;
    cmd_proc proc;

    memset(&proc, 0, sizeof(cmd_proc));
    proc.pf = tmpfile();
    if(!proc.pf) {
        printf("create tmp file failed\r\n");
        return -2;
    }

    time_count1 = get_current_time();
    if(netdet_cmd_exec(cmd, &proc) == 0)
    {
        while(1)
        {
            FD_ZERO(&set);
            FD_SET(proc.fd, &set);
            t.tv_sec = 0 ;
            t.tv_usec = 10000;
            ret = select(proc.fd+1, &set, NULL, NULL, &t);
            if(ret == 1){
                num = read(proc.fd, buf, sizeof(buf));
                if(num > 0){
                    //printf("cmd get buf:%s\r\n", buf);
                    fwrite(buf, 1, num, proc.pf);
                    read_count += num;
                } else if(num <= 0) {
                    printf("read error length < 0");
                    break;
                }
            } else if(ret == 0) {
                if(kill(proc.pid, 0) != 0) {
                    printf("exec procss exit\r\n");
                    break;
                }
            } else {
                if(errno == EBADF || errno == EINTR) {
                    printf("read break down\r\n");
                    break;
                }
            }
    
            if (ret == -5)
            {
                printf("cmd timeout");
                break;
            }
            time_count2 = get_current_time();
            cmd_time += (long)time_count2 - (long)time_count1;
            time_count1 = time_count2;
            if(cmd_time >  timeout) {
                printf("netdet exec timeout\r\n");
                kill(proc.pid, SIGINT);
                ret = -5;
            }
        }
        //pipe file should be close
        close(proc.fd);
        if(read_count > 0) {
            fseek(proc.pf, 0, SEEK_SET);
            *pf =  proc.pf;
            return read_count;
        }
    }
    if(proc.pf)
        fclose(proc.pf);
    return -1;
}



int main()
{
    char *str = "ping -c 10 www.baidu.com";
    FILE *pf;
    int ret = netdet_popen(&pf, str, 5000);
    if (ret > 0)
    {
        char * str = (char *)malloc(ret);
        fread(str, sizeof(char), ret, pf);
        printf("get str: %s\r\n", str);
        fclose(pf);
    }
    return 0;
}
