#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define FILENAMELENS 100
#define BUCKETNUMBER 100
//#define READPER  5242880//5 MB
#define READPER  1073741824//1 GB
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
    int MAXDBFILESIZE;
    //int BUCKETNUMBER;
    int CURFILEID;
    /*DATA.......*/
}Config;

Config init(char *set);
unsigned int hash33(char *key);

int GetOffset(FILE *file);
int GetFileSize(char *data);
int Gethv(char *data);
int GetFileId();
/*-----------------------------------*/
int ReadIndexFile(char *filename,int option);
int ReadMapFile(char *filename,int option);
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
int GetFile(char *file);

detail records[BUCKETNUMBER];
map fname_to_hv[BUCKETNUMBER];
char index_path[100]="./db/index";
char db_path[100]="./db/file_0";
char map_path[100]="./db/map";
char config_path[100]="./db/init";

int main(int argc, char *argv[])
{
    if(argc!=2){
        printf("format error!");
        exit(1);
    }
    FILE *index_file,*db_file,*data_file,*map_file;
    char *filename;
    char *data;

    int len=0,cnt=0;
    unsigned int index_record,index_map,index,hv;
    filename = malloc(sizeof(char)*strlen(argv[1]));
    strcpy(filename,argv[1]);
/*---------------read config file---------------*/
/*---------------read index file----------------*/
    if(ReadIndexFile(index_path,0)==1){
        index_file = fopen(index_path,"w");
    }
    else{
        index_file = fopen(index_path,"w");
    }
/*---------------read map file----------------*/
    if(ReadMapFile(map_path,0)==1){
        map_file = fopen(map_path,"w");
    }
    else{
        map_file = fopen(map_path,"w");
    }

/*-----------------file exist or not-----------------*/
    if(GetFile(filename)==1){
        exit(1);   
    }

/*-----read file and its datail that will be import to db and index file-----*/
    data_file = fopen(filename,"r");
    if(data_file==NULL){exit(1);}
    data = malloc(sizeof(char)*DATASIZE);
    while(len=fread(data,sizeof(char),READPER,data_file)){
        printf("read size:%d\n",len);
    }
    hv = Gethv(data);
    index_record = hv % BUCKETNUMBER;
    records[index_record].key = hv;
    records[index_record].size = GetFileSize(data);
    
    //read db to get current offset
    db_file = fopen(db_path,"a");
    //if(db_file==NULL){exit(1);printf("db file doesn't exist");}
    records[index_record].offset = GetOffset(db_file);
    fclose(db_file);

    //store map
    hv = Gethv(filename);
    index_map = hv % BUCKETNUMBER;
    strncpy(fname_to_hv[index_map].filename,filename,strlen(filename)+1);
    fname_to_hv[index_map].key = index_record;


/*---------------------write file's detail-----------------------*/    
    //fwtite to file index
    fwrite(records,sizeof(detail),BUCKETNUMBER,index_file);
    fclose(index_file);
    //for check
    ReadIndexFile(index_path,1);
    
    //fwtite to map file
    fwrite(fname_to_hv,sizeof(map),BUCKETNUMBER,map_file);
    fclose(map_file);
    ReadMapFile(map_path,1);     
    //fwrite to db
    db_file = fopen(db_path,"ab");
    fwrite(data,sizeof(char),strlen(data),db_file);
    fclose(db_file);
    //printf("write to db:ok\n");
    
    return 0;
}

int GetOffset(FILE *file){
    fseek(file,0,SEEK_END);
    return ftell(file);
}
int GetFileSize(char *data){
    return strlen(data);
}
int Gethv(char *data){
    return hash33(data);
}
int GetFileId(){

}
int GetFile(char *filename){
    unsigned int hv,index;
    int index_record;
    int cnt;
    char *data;
    FILE *db_file,*result,*index_file,*map_file;
    hv = Gethv(filename);
    index = hv % BUCKETNUMBER;
    if(strcmp(fname_to_hv[index].filename,filename)==0){
        printf("file %s exist!\n",fname_to_hv[index].filename);
        index_record = fname_to_hv[index].key;
        printf("get file index [%d]:%u\t%d\t%d\n",index_record,records[index_record].key,records[index_record].offset,records[index_record].size);

        db_file = fopen(db_path,"r");
        if(db_file==NULL){
            printf("db file missing!\n");
            exit(1);
        }
        fseek(db_file,records[index_record].offset,SEEK_SET);
        data = malloc(sizeof(char)*records[index_record].size);
        fread(data,sizeof(char),records[index_record].size,db_file);
        printf("data:%s\n",data);
        
        result = fopen("./db/result","w");
        index_file = fopen(index_path,"w");
        map_file = fopen(map_path,"w");
        fwrite(data,sizeof(char),records[index_record].size,result);
        fwrite(records,sizeof(detail),BUCKETNUMBER,index_file);
        fwrite(fname_to_hv,sizeof(map),BUCKETNUMBER,map_file);

        fclose(db_file);
        fclose(result);
        return 1;
    }
    else{
        printf("file %s doesn't exist!\n",fname_to_hv[index].filename);
        return 0;
    }
}

int ReadIndexFile(char *filename,int option){
    FILE *index_file;
    int cnt=0;
    index_file = fopen(index_path,"rb");
    if(index_file!=NULL){
        fread(records,sizeof(detail),BUCKETNUMBER,index_file);
        if(option==1){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(records[cnt].key!=0){
                    printf("[%d]index file:%u\t%d\t%d\n",cnt,records[cnt].key,records[cnt].offset,records[cnt].size);
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
    index_file = fopen(map_path,"rb");
    if(index_file!=NULL){
        fread(fname_to_hv,sizeof(map),BUCKETNUMBER,index_file);
        
        if(option==1){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(fname_to_hv[cnt].key!=0){
                    printf("[%d]map file:%d\t%s\n",cnt,fname_to_hv[cnt].key,fname_to_hv[cnt].filename);
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
unsigned int hash33(char *key)
{
    char *ptr = key;
    unsigned int hv = 0;
    while(*ptr != '\0'&& *ptr != '\n'){
        hv = ((hv << 5) + hv) + *ptr;//2^5+1=33
        ptr++;

    }
    return hv;

}
