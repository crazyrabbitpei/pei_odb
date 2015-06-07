#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#include "rdb.h"
#define RECORDLEN 1000
extern char dfile_path[];
extern int id;

int StoreGais(char *name,char *type,int len,char *date,unsigned long int key,int fp,char *dir_path){
    char *record;
    int size;
    record = malloc(sizeof(char)*RECORDLEN);
    sprintf(record,"@\n@name:%s\n@type:%s\n@ctime:%s@size:%d\n@path:%s\n",name,type,date,len,dir_path);
    write(fp,record,strlen(record));
    size = strlen(record);
    free(record);
    close(fp);

    return size;
}

int CreateDir(char *name,char *dir_path){
    char* date;
    int des_file,des_offset,size;
    unsigned long int hv,index_map;
    time_t current_time;
    des_file = open(dfile_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
    des_offset = GetOffset(des_file);

    current_time = time(NULL);
    date = ctime(&current_time);
    //TODO:file name same as dir name?
    /*  
    hv = Gethv((unsigned char *)name,(unsigned long int)strlen(name));
    index_map = hv % BUCKETNUMBER;
    if(name_list[index_map].key!=-1 && name_list[index_map].key!=0){
        name_list[index_map].key = hv;
        return -1;
    }
    strcpy(name_list[index_map].filename,name);
    name_list[index_map].key = hv;
    name_list[index_map].offset = des_offset;
    name_list[index_map].size = StoreGais(name,"dir",0,date,hv,des_file,dir_path);
    */
    size = StoreGais(name,"dir",0,date,hv,des_file,dir_path);
    rdb_create(name,des_offset,size,dir_path);
    
    return 0;
}

int rdb_create(char *name,int offset, int size,char *parent){
    char *record;

    //each row : id,name,offset,size
    record = malloc(sizeof(char)*RECORDLEN);
    sprintf(record,"%d,%s,%d,%d\n",id,name,offset,size);
    write(name_file,record,strlen(record));
    free(record);
    //add id
    id++;
    id_file = open(id_record,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU|S_IRWXG);//clean old id
    record = malloc(sizeof(char)*100);
    sprintf(record,"%d",id);
    write(id_file,record,strlen(record));
    close(id_file);

    return 0;
    
}
int rdb_read(){
    //get record with the id

    //parese column(use , :)
    
    //get record in dfile(use offset,size) 

}
int rdb_update(){
}
int rdb_delete(){
    //id=-1
}

