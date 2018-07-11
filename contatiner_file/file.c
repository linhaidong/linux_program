/*************************************************************************
    > File Name: file.c
    > Author:     linhaidong
    > Mail:       linengeir@126.com
    > Time:      2017年06月23日 10时26分45秒 CST
    > Abstract: 
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void log(const char *format, ...)
{
    char * time_str;
    va_list argp;
    char str[1000];
    struct stat file_stat;
    if(logfile && log_count++ > MAX_LOG_LINE) {
        truncate(LOG_FILE, 0);
        rewind(logfile);
        log_count = 0;
    }
    stat(LOG_FILE, &file_stat);
    printf("File size:                %lld bytes\n", (long long)file_stat.st_size);
    va_start(argp, format);
    vsnprintf(str, sizeof(str), format, argp);
    time_str = get_local_time();
    if (logfile) {
        fprintf(logfile, "%s netdetect: %s\n", time_str, str);
    } else {
        fprintf(stderr, "%s netdetect: %s\n", time_str, str);
    }
    va_end(argp);
    if(time_str) free(time_str);
}
