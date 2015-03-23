#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#define FILENAMELENS 100
#define BUCKETNUMBER 1000000
//#define READPER  5242880//5 MB
#define READPER  10737418240//1 GB
#define DATASIZE  10737418240//1 GB
typedef struct{
    /*index file format */
    unsigned int key;
    int file_id;
    int offset;
    int size;
}detail;
typedef struct{
    /* filename map to hv */
    char filename[FILENAMELENS];
    int key;
} map;

typedef struct{
    /*db init setting*/
    int DBFILENUM;
    long int MAXDBFILESIZE;
    //int BUCKETNUMBER;
    int CURFILEID;
    /*DATA.......*/
}Config;

Config init(char *set);
unsigned int hash33(char *key,int size);

int GetOffset(FILE *file);
int GetFileSize(FILE *file);
int Gethv(char *data,int size);
void GetFileId();
/*-----------------------------------*/
int ReadIniFile(char *filename,int option);
int ReadIndexFile(char *filename,int option);
int ReadMapFile(char *filename,int option);
void WriteAll();
int CheckFile(FILE *file,char *filename,char *action);
//data at ./data
//index at ./db/index
//db at ./db
//config at ./db/init
/*-----------------------------------*/
/*
void StoreToDB();
void StoreToIndexFile();
void StoreToMap();
*/
/*-----------------------------------*/
detail GetIndexFile(int fileid);
int GetFile(char *filename,int size,int option,char *newfilename,FILE *ini_file);
int PutFile(char *filename,char *relfilename,FILE *index_file,FILE *map_file,FILE *ini_file);
char *Rename(char *filename,int option,FILE *ini_file);

Config dbini[2];
detail records[BUCKETNUMBER];
map fname_to_hv[BUCKETNUMBER];

char index_path[100]="./db/index";
char db_path[100]="";
char map_path[100]="./db/map";
char config_path[100]="./db/init";
char result_path[100]="";

int main(int argc, char *argv[])
{
    if(argc!=2){
        printf("format error!");
        exit(1);
    }
    FILE *index_file,*map_file,*ini_file;
    char *filename,*relfilename;
    int option,cnt;
    
    filename = malloc(sizeof(char)*strlen(argv[1]));
    relfilename = malloc(sizeof(char)*strlen(argv[1]));
    filename=argv[1];
    strcpy(relfilename,argv[1]);
////------------------------------------------------////
/*           Read config,index,map file               */
////------------------------------------------------////
    /*---------------read config file---------------*/
    if(ReadIniFile(config_path,1)==0){//dbini exists
        GetFileId();
        ini_file = fopen(config_path,"w");
        if(CheckFile(ini_file,config_path,"w")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
    }
    else{//dbini doesn't exist
        ini_file = fopen(config_path,"w");
        if(CheckFile(ini_file,config_path,"w")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
        dbini[0].DBFILENUM=2;
        dbini[0].MAXDBFILESIZE=31457280;//30 MB
        dbini[0].CURFILEID=0;
        fwrite(dbini,sizeof(Config),2,ini_file);
        GetFileId();
    }
    /*---------------read index file----------------*/
    if(ReadIndexFile(index_path,0)==1){
        index_file = fopen(index_path,"w");
        if(CheckFile(index_file,index_path,"w")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
    }
    else{
        index_file = fopen(index_path,"w");
        if(CheckFile(index_file,index_path,"w")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
    }
    /*---------------read map file----------------*/
    if(ReadMapFile(map_path,0)==1){
        map_file = fopen(map_path,"w");
        if(CheckFile(map_file,map_path,"w")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
    }
    else{
        map_file = fopen(map_path,"w");
        if(CheckFile(map_file,map_path,"w")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
    }
////------------------------------------------------////
/*   Check if filename exist, and rename for import   */
/*   status(3) or get the file(1 or 2 )               */
////------------------------------------------------////
    /*separate real file name*/
    char *delim="/";
    char *p[FILENAMELENS];
    cnt=0;
    p[cnt++] = strtok(relfilename,delim);
    while(p[cnt]=(strtok(NULL,delim))){
        //printf("[%d]%s\n",cnt,p[cnt]);
        cnt++;
    }
    relfilename=p[cnt-1];
    printf("filename:%s\n",relfilename);
    //1:download file use orign name
    //2:download file use refer name(result_path)
    //3:just for check filename exists or not(import status)
    //TODO
    option=3; 
    if(GetFile(relfilename,strlen(relfilename),option,result_path,ini_file)==1){//if this filename exists
        if(option==3){
            printf("filename [%s] exists,",relfilename);
            relfilename = Rename(relfilename,option,ini_file);
            printf("so will rename to [%s]\n",relfilename);
        }
        else{
            WriteAll(index_file,map_file,ini_file);
            return 0;   
        }    
    }
////------------------------------------------------////
/*                      PutFile                       */
////------------------------------------------------////
    if(PutFile(filename,relfilename,index_file,map_file,ini_file)==0){//success import
    }
    else{
        printf("Import fail\n");
        WriteAll(index_file,map_file,ini_file);
        return 0;
    }


////------------------------------------------------////
/*          Check data import successfuly or not      */
////------------------------------------------------////

    ReadIndexFile(index_path,1);     
    ReadMapFile(map_path,1);     

    return 0;
}


void WriteAll(FILE *index_file,FILE *map_file,FILE *ini_file){
    /*---------------write file's detail---------------*/    
    //fwtite to file index
    fwrite(records,sizeof(detail),BUCKETNUMBER,index_file);
    //fwtite to map file
    fwrite(fname_to_hv,sizeof(map),BUCKETNUMBER,map_file);
    //fwrite to dbini
    fwrite(dbini,sizeof(Config),2,ini_file);

    fclose(index_file);
    fclose(map_file);
    fclose(ini_file);
}
int GetOffset(FILE *file){
    fseek(file,0,SEEK_END);
    return ftell(file);
}
int GetFileSize(FILE *file){
    fseek(file,0,SEEK_END);
    return ftell(file);
}
int Gethv(char *data,int size){
    return hash33(data,size);
}
void GetFileId(){
        sprintf(db_path,"./db/file_%d",dbini[0].CURFILEID);
}
int PutFile(char *filename,char *relfilename,FILE *index_file,FILE *map_file,FILE *ini_file){
    unsigned char *data;
    int len=0,cnt=0,filesize,offset;
    int option;
    unsigned int index_record,index_map,index,hv;
    //FILE *data_file,*db_file;
    FILE *db_file;
    int data_file;

    
    //data_file = fopen(filename,"rb");
    data_file = open(filename,O_RDWR);
    /*
    if(CheckFile(data_file,filename,"rb")==1){
        WriteAll(index_file,map_file,ini_file);
        return 0;
    }
    */
    data = malloc(sizeof(unsigned char)*DATASIZE);
    //while(len=fread(data,sizeof(unsigned char),READPER,data_file)){
    len=read(data_file,data,sizeof(data));
        printf("read size:%d\n",len);
    filesize = len;
    //filesize = GetFileSize(data_file);
    //fclose(data_file);   
    close(data_file);   
    
    hv=0;
    hv = Gethv(data,filesize);
    index_record = hv % BUCKETNUMBER;
    //ckeck file content, if exist then ask to cover existing file or ignore this import action
    if(records[index_record].key==hv){
        printf("file [%s] content  exist!\n",relfilename);
        WriteAll(index_file,map_file,ini_file);
        return 1;
    }


    //read db to get current offset, and get filesize
    GetFileId();
    db_file = fopen(db_path,"ab");
    if(CheckFile(db_file,db_path,"ab")==1){
        WriteAll(index_file,map_file,ini_file);
        return 0;
    }
    offset = GetOffset(db_file);
    //if current file offset + data size > DBFILESIZE, then open a new file(file_n) to store data
    if(offset+filesize>dbini[0].MAXDBFILESIZE){
        printf("[offset:%d,size:%d->limit %ld size]\n",offset,filesize,dbini[0].MAXDBFILESIZE);
        if(dbini[0].CURFILEID+1>=dbini[0].DBFILENUM){
            printf("[X] Max File ID is %d, current file num is %d\n",dbini[0].DBFILENUM,dbini[0].CURFILEID+1);
            WriteAll(index_file,map_file,ini_file);
            return 1;
        }
        else{
            printf("file [%d] is full, now open file [%d]\n",dbini[0].CURFILEID,dbini[0].CURFILEID+1);
            dbini[0].CURFILEID = dbini[0].CURFILEID+1;
        }
        fwrite(dbini,sizeof(Config),2,ini_file);
        GetFileId();
        fclose(db_file);
        db_file = fopen(db_path,"wb");
        if(CheckFile(db_file,db_path,"wb")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
    }   
    //store into index

    records[index_record].key = hv;
    records[index_record].file_id = dbini[0].CURFILEID;
    records[index_record].size = filesize;
    records[index_record].offset = offset;


    //store map
    hv = Gethv(relfilename,strlen(relfilename));
    index_map = hv % BUCKETNUMBER;
    strncpy(fname_to_hv[index_map].filename,relfilename,strlen(relfilename)+1);
    fname_to_hv[index_map].key = index_record;

    WriteAll(index_file,map_file,ini_file);
    
    //fwrite to db
    fwrite(data,sizeof(char),strlen(data),db_file);
    fclose(db_file);

    return 0;
}
int GetFile(char *filename,int size,int option,char *newfilename,FILE *ini_file){
    unsigned int hv,index;
    int index_record;
    int cnt;
    char *data;
    char result_path[100]="";
    FILE *db_file,*result,*index_file,*map_file;
    hv = Gethv(filename,size);
    index = hv % BUCKETNUMBER;
    if(strcmp(fname_to_hv[index].filename,filename)==0){
        if(option==3){//option 3 is put status,so just ckeck filename exists or not
            printf("filename [%s] exist!\n",fname_to_hv[index].filename);
            return 1;
        }
        index_record = fname_to_hv[index].key;
        printf("get file index [%d]:file [%d] %u\t%d\t%d\n",index_record,records[index_record].file_id,records[index_record].key,records[index_record].offset,records[index_record].size);
        
        sprintf(db_path,"./db/file_%d",records[index_record].file_id);
        db_file = fopen(db_path,"rb");
        if(CheckFile(db_file,db_path,"rb")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
        fseek(db_file,records[index_record].offset,SEEK_SET);
        data = malloc(sizeof(char)*records[index_record].size);
        fread(data,sizeof(char),records[index_record].size,db_file);
        if(option==1){//1:original filename,2:refer filename
            sprintf(result_path,"./db/download/%s",filename);
            printf("download file [%s]\n",filename);
        }
        else{
            sprintf(result_path,"./db/download/%s",newfilename);
            printf("download file [%s], and rename to [%s]\n",filename,newfilename);
        }
        result = fopen(result_path,"wb");
        if(CheckFile(result,result_path,"wb")==1){
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
        fwrite(data,sizeof(char),records[index_record].size,result);
        fclose(db_file);
        fclose(result);
        return 1;
    }
    else{
        printf("file %s doesn't exist!\n",filename);
        return 0;
    }
}
char *Rename(char *filename,int option,FILE *ini_file){
    char *new;
    char *delim=".";
    char *p[FILENAMELENS];
    char *temp,*type;
    int cnt=0;
    temp = filename;
    p[cnt++] = strtok(temp,delim);
    //printf("[%d]%s\n",cnt-1,p[cnt-1]);
    while(p[cnt]=(strtok(NULL,delim))){
        //printf("[%d]%s\n",cnt,p[cnt]);
        cnt++;
    }
    new = malloc(sizeof(char)*FILENAMELENS);
    if(cnt>1){
        type = p[cnt-1];
        sprintf(new,"%s_%d.%s",p[0],1,type);
    }
    else{
        type="";
        sprintf(new,"%s_%d",p[0],1);
    }
    if(GetFile(new,strlen(new),option,result_path,ini_file)==1){//if this filename exists
       new = Rename(new,option,ini_file);    
    }


    return new;
}
int CheckFile(FILE *file,char *filename,char *action){
    if(file==NULL){
        printf("file [%s] can't [%s]\n",filename,action);
        return 1;
    }
    return 0;
}
int ReadIniFile(char *filename,int option){
    FILE *index_file;
    int cnt=0;
    index_file = fopen(filename,"rb");
    if(index_file!=NULL){
        fread(dbini,sizeof(Config),2,index_file);
        if(option==1){
            for(cnt=0;cnt<2;cnt++){
                if(dbini[cnt].DBFILENUM!=0){
                    printf("dbini file->DBFILENUM:%d\tMAXDBFILESIZE:%d\tCURFILEID:%d\n",dbini[cnt].DBFILENUM,dbini[cnt].MAXDBFILESIZE,dbini[cnt].CURFILEID);
                }
            }
        }
        fclose(index_file);
        return 0;
    }
    else{
        return 1;
    }
}
int ReadIndexFile(char *filename,int option){
    FILE *index_file;
    int cnt=0;
    index_file = fopen(filename,"rb");
    if(index_file!=NULL){
        fread(records,sizeof(detail),BUCKETNUMBER,index_file);
        if(option==1){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(records[cnt].key!=0){
                    printf("[%d]index file:%u\t%d\t%d\t%d\n",cnt,records[cnt].key,records[cnt].file_id,records[cnt].offset,records[cnt].size);
                }
            }
        }
        fclose(index_file);
        return 0;
    }
    else{
        return 1;
    }
}
int ReadMapFile(char *filename, int option){
    FILE *index_file;
    int cnt=0;
    index_file = fopen(filename,"rb");
    if(index_file!=NULL){
        fread(fname_to_hv,sizeof(map),BUCKETNUMBER,index_file);
        
        if(option==1){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(fname_to_hv[cnt].key!=0){
                    printf("[%d]map file:%s->%d\n",cnt,fname_to_hv[cnt].filename,fname_to_hv[cnt].key);
                }
            }
        }
        
        fclose(index_file);
        return 0;
    }
    else{
        return 1;
    }
}
unsigned int hash33(char *key,int size)
{
    char *ptr = key;
    unsigned int hv = 0;
    int cnt=0;
    while(cnt<size){
        hv = ((hv << 5) + hv) + *ptr;//2^5+1=33
        ptr++;
        cnt++;
    }
    return hv;
}
