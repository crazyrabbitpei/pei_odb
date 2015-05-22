#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "rdb.h"
#define RECORDLEN 1000
int StoreGais(char *name,char *type,int len,char *date,unsigned long int key,int fp){
    char *record;
    int size;
    record = malloc(sizeof(char)*RECORDLEN);
    sprintf(record,"@\n@key:%lu\n@name:%s\n@type:%s\n@ctime:%s@size:%d\n@path:/",key,name,type,date,len);
    write(fp,record,strlen(record));
    size = strlen(record);
    close(fp);
    return size;
}
