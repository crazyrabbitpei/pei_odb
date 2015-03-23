#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>                                                                                                                      
#include<unistd.h>
#define FILENAMELENS 100
#define DATASIZE  1073741824//1 MB
int main(int argc, char *argv[])
{
    int fd,size,cnt;
    char s[]="Linux Programmer!\n";
    char *buffer;
    char path[100];
    char *filename,*relfilename;
    buffer = malloc(sizeof(unsigned char)*DATASIZE);

    fd=open(argv[1],O_RDONLY);
    size=read(fd,buffer,DATASIZE);
    close(fd);
    
    printf("read:%s\tsize:%lu\n",argv[1],strlen(buffer));

    filename = malloc(sizeof(char)*strlen(argv[1]));
    relfilename = malloc(sizeof(char)*strlen(argv[1]));
    filename=argv[1];
    strcpy(relfilename,argv[1]);

    char *delim="/";
    char *p[FILENAMELENS];
    cnt=0;
    p[cnt++] = strtok(relfilename,delim);                                                                                                  while((p[cnt]=(strtok(NULL,delim)))){
        printf("[%d]%s\n",cnt,p[cnt]);                                                                                                         cnt++;

    }
    relfilename=p[cnt-1];
    printf("filename:%s\n",relfilename);

    sprintf(path,"./db/download/%s",relfilename);
    fd=open(path,O_WRONLY|O_CREAT,S_IRWXU);
    size = write(fd,buffer,strlen(buffer));
    //printf("data:%s\tsize:%d\n",buffer,size);
    close(fd);
    printf("write to :%s",path);
}
