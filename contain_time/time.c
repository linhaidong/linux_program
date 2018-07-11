/*************************************************************************
    > File Name: time.c
    > Author:     linhaidong
    > Mail:       linhaidong@alibaba-inc.com 
    > Time:      2017年06月20日 19时57分29秒 CST
    > Abstract: 
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
void my_time()
{
    localtime(&time);
    fprintf(stderr, "%s\n", ctime(&time));
}
int  main()
{
    my_time();
}
