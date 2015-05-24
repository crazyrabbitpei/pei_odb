#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include "rdb.h"
#define RECORDLEN 1000
extern char dfile_path[];
extern name name_list[];
int StoreGais(char *name,char *type,int len,char *date,unsigned long int key,int fp){
    char *record;
    int size;
    record = malloc(sizeof(char)*RECORDLEN);
    sprintf(record,"@\n@key:%lu\n@name:%s\n@type:%s\n@ctime:%s@size:%d\n@path:/\n",key,name,type,date,len);
    write(fp,record,strlen(record));
    size = strlen(record);
    free(record);
    close(fp);
    return size;
}

int CreateDir(char *name){
    char* date;
    int des_file,des_offset;
    unsigned long int hv,index_map;
    time_t current_time;
    des_file = open(dfile_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
    des_offset = GetOffset(des_file);

    current_time = time(NULL);
    date = ctime(&current_time);
    
    hv = Gethv((unsigned char *)name,(unsigned long int)strlen(name));
    index_map = hv % BUCKETNUMBER;
    if(name_list[index_map].key!=-1 && name_list[index_map].key!=0){
        name_list[index_map].key = hv;
        return -1;
    }
    name_list[index_map].key = hv;
    name_list[index_map].offset = des_offset;
    name_list[index_map].size = StoreGais(name,"dir",0,date,hv,des_file);
    
    return 0;
}
