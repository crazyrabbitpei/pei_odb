#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>                                                                                                                      
#include<unistd.h>
#include <errno.h>
#define FILENAMELENS 100
#define DATASIZE  1073741824//1 MB
int main(int argc, char *argv[])
{
    int fd;
    unsigned long int size;
    unsigned char *buffer;
    int cnt;
    char s[]="Linux Programmer!\n";

    char path[100];
    char *filename,*relfilename;
    buffer = malloc(sizeof(unsigned char)*DATASIZE);

    fd=open(argv[1],O_RDONLY|O_EXCL);

    if(fd<0){
        printf("Opening file : Failed\n");
        printf ("Error no is : %d\n", errno);
        printf("Error description is : %s\n",strerror(errno));
        exit(1);
    }
    size=read(fd,buffer,DATASIZE);
    close(fd);

    printf("read:%s\tsize:%lu\n",argv[1],size);

    filename = malloc(sizeof(char)*strlen(argv[1]));
    relfilename = malloc(sizeof(char)*strlen(argv[1]));
    filename=argv[1];
    strcpy(relfilename,argv[1]);

    char *delim="/";
    char *p[FILENAMELENS];
    cnt=0;
    p[cnt++] = strtok(relfilename,delim);                                                                                                  while((p[cnt]=(strtok(NULL,delim)))){
        cnt++;
    }
    relfilename=p[cnt-1];
    printf("filename:%s\n",relfilename);

    sprintf(path,"./db/download/%s",relfilename);
    fd=open(path,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);//O_TRUNC will cover existing file,if the filename exists
    size = write(fd,buffer,size);
    close(fd);
    printf("write to :%s",path);
}
