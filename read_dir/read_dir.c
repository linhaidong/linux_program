/*************************************************************************
    > File Name: read_dir.c
    > Author:     linhaidong
    > Mail:       linengier@126.com
    > Time:      2017年04月11日 星期二 17时49分07秒
    > Abstract: 
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
int readFileList(char *basePath)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(basePath)) == NULL) {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL) {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
            printf("d_name:%s/%s\n",basePath,ptr->d_name);
        else if(ptr->d_type == 10)    ///link file
            printf("d_name:%s/%s\n",basePath,ptr->d_name);
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base);
        }
    }
    closedir(dir);
    return 1;
}
int print_dir(void)
{
    //DIR *dir;
    char basePath[100];

    ///get the current absoulte path
    memset(basePath,'\0',sizeof(basePath));
    getcwd(basePath, 999);
    printf("the current dir is : %s\n",basePath);

    ///get the file list
    //memset(basePath,'\0',sizeof(basePath));
    //strcpy(basePath,"/bin");
    readFileList(basePath);
    return 0;
}

int main()
{
    print_dir();
}
