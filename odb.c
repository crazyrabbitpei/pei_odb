#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include <errno.h>
#define FILENAMELENS 100
#define BUCKETNUMBER 1000000
//#define READPER  5242880//5 MB
#define READPER  10737418240//1 GB
//#define DATASIZE  10737418240//1 GB
#define DATASIZE  1073741824//1 M
#define OFF 0
#define ON 1
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
unsigned long int hash33(unsigned char *key,unsigned long int size);

int GetOffset(int file);
int GetFileSize(int file);
unsigned long int Gethv(unsigned char *data,unsigned long int size);
void GetFileId(char *path);
/*-----------------------------------*/
int ReadIniFile(char *filename,int option);
int ReadIndexFile(char *filename,int option);
int ReadMapFile(char *filename,int option);
void WriteAll(int index_file,int map_file,int ini_file);
int CheckFile(int fd,char *filename);
void msg();
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
int GetFile(char *filename,int size,int option,char *newfilename,int ini_file,char *path);
int PutFile(char *filename,char *relfilename,int index_file,int map_file,int ini_file,char *path,char *newfilename);
char *Rename(char *filename,int option,int ini_file,char *path,char *newfilename);
int ListFile();

Config dbini[2];
detail records[BUCKETNUMBER];
map fname_to_hv[BUCKETNUMBER];
char index_path[100]="./db/index";
char db_path[100]="";
char map_path[100]="./db/map";
char config_path[100]="./db/init";
char result_path[100]="./db/download/";

typedef enum{
    PUT,
    GET,
    LIST,
    DEL,
    RENAME
}command;

int main(int argc, char *argv[])
{
    int index_file,map_file,ini_file;
    char *filename,*relfilename,newfilename[100]="";
    int option=-1,cnt;
    char path[100]="./db/file_";

    int choice;
    cnt=0;
    while((choice = getopt( argc, argv, "f::c:o::"))!=-1){
        switch(choice)
        {
            case 'f':
                filename = malloc(sizeof(char)*strlen(optarg));
                relfilename = malloc(sizeof(char)*strlen(optarg));
                filename=optarg;
                strcpy(relfilename,optarg);
                break;

            case 'c':
                if(strcmp(optarg,"PUT")==0){
                    option=PUT;
                }
                else if(strcmp(optarg,"GET")==0){
                    option=GET;
                }
                else if(strcmp(optarg,"LIST")==0){
                    option=LIST;
                }
                //TODO
                else if(strcmp(optarg,"DEL")==0){
                    option=DEL;
                }
                else if(strcmp(optarg,"RENAME")==0){
                    option=RENAME;
                }
                break;

            case 'o':
                strcpy(newfilename,optarg);
                break;

            case '?':
                fprintf(stderr, "Illegal option:-%c\n", optopt);
                break;

            default:
                fprintf(stderr, "Not supported option\n");
                break;
        }
    }
    if(optind!=argc||option==-1){
        msg();
        exit(1);
    }

    ////------------------------------------------------////
    /*           Read config,index,map file               */
    ////------------------------------------------------////
    /*---------------read config file---------------*/
    if(ReadIniFile(config_path,OFF)==0){//dbini exists
        GetFileId(path);
        ini_file = open(config_path,O_WRONLY);
        if(CheckFile(ini_file,config_path)==1){
            WriteAll(index_file,map_file,ini_file);
            //return 0;
            exit(1);
        }
    }
    //TODO:Config by user
    else{//dbini doesn't exist,init now
        ini_file = open(config_path,O_WRONLY|O_CREAT,S_IRWXU);
        if(CheckFile(ini_file,config_path)==1){
            WriteAll(index_file,map_file,ini_file);
            //return 0;
            exit(1);
        }
        dbini[0].DBFILENUM=2;
        dbini[0].MAXDBFILESIZE=31457280;//30 MB
        dbini[0].CURFILEID=0;
        ReadIniFile(config_path,ON);
        write(ini_file,dbini,sizeof(Config)*2);
        GetFileId(path);
    }
    /*---------------read index file----------------*/
    if(ReadIndexFile(index_path,OFF)==0){
        index_file = open(index_path,O_WRONLY);
        if(CheckFile(index_file,index_path)==1){
            WriteAll(index_file,map_file,ini_file);
            //return 0;
            exit(1);
        }
    }
    else{
        index_file = open(index_path,O_WRONLY|O_CREAT,S_IRWXU);
        if(CheckFile(index_file,index_path)==1){
            WriteAll(index_file,map_file,ini_file);
            //return 0;
            exit(1);
        }
    }
    /*---------------read map file----------------*/
    if(ReadMapFile(map_path,OFF)==0){
        map_file = open(map_path,O_WRONLY);
        if(CheckFile(map_file,map_path)==1){
            WriteAll(index_file,map_file,ini_file);
            //return 0;
            exit(1);
        }
    }
    else{
        map_file = open(map_path,O_WRONLY|O_CREAT,S_IRWXU);
        if(CheckFile(map_file,map_path)==1){
            WriteAll(index_file,map_file,ini_file);
            //return 0;
            exit(1);
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
    p[cnt] = strtok(relfilename,delim);
    //printf("[%d]%s\n",cnt-1,p[cnt-1]);
    while(p[cnt]){
        //printf("[%d]%s\n",cnt,p[cnt]);
        p[++cnt]=(strtok(NULL,delim));
        //cnt++;
    }
    relfilename=p[cnt-1];
    //GET:download file use orign name
    //GET & RENAME:download file use refer name(newfilename)
    //PUT:just for check filename exists or not(import status)
    if(option==PUT||option==GET){
        if(strcmp(newfilename,"")!=0){
            strcpy(relfilename,"");
            strcpy(relfilename,newfilename);
            strcpy(newfilename,"");
        }
        if(GetFile(relfilename,strlen(relfilename),option,newfilename,ini_file,path)!=0){//if this filename exists
            if(option==PUT){
                printf("filename [%s] exists,",relfilename);
                relfilename = Rename(relfilename,option,ini_file,path,newfilename);
                printf("so will rename to [%s]\n",relfilename);
            }
            else if(option==GET){
                WriteAll(index_file,map_file,ini_file);
                return 0;   
            }    
        }
    }
    ////------------------------------------------------////
    /*                      PutFile                       */
    ////------------------------------------------------////
    if(option==PUT){
        if(PutFile(filename,relfilename,index_file,map_file,ini_file,path,newfilename)==0){//success import
        }
        else{
            printf("Nothing import.\n");
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
    }
    ////------------------------------------------------////
    /*                  Rename file                       */
    ////------------------------------------------------////
    //TODO[1]:in text book
    if(option==RENAME){
        if(strcmp(newfilename,"")==0){
            printf("New filename?\n");
            msg();
        }
        unsigned long int index,key,hv;

        if((index=GetFile(relfilename,strlen(relfilename),option,newfilename,ini_file,path))!=0){
            printf("rename file [%s] to ",fname_to_hv[index].filename);
            strcpy(relfilename,Rename(relfilename,option,ini_file,path,newfilename));
            printf("[%s]\n",relfilename);
            strcpy(fname_to_hv[index].filename,"");
            key = fname_to_hv[index].key;
            fname_to_hv[index].key=-1;

            hv = Gethv((unsigned char *)relfilename,(unsigned long int)strlen(relfilename));
            index = hv % BUCKETNUMBER;
            strncpy(fname_to_hv[index].filename,relfilename,strlen(relfilename)+1);
            fname_to_hv[index].key = key;
        
            WriteAll(index_file,map_file,ini_file);
        }
        else{ 
            printf("file [%s] doesn't exist.\n",relfilename);
            WriteAll(index_file,map_file,ini_file);
            return 0;
        }
    }

    ////------------------------------------------------////
    /*      List:Check data import successfuly or not     */
    ////------------------------------------------------////
    if(option==LIST){
        ReadIndexFile(index_path,1);     
        ReadMapFile(map_path,1);     
    }

    return 0;
}
void msg(){
    printf("usage:./a.out -f [filename] -c [command] -o [newfilename]");
    exit(1);
}

void WriteAll(int index_file,int map_file,int ini_file){
    /*---------------write file's detail---------------*/    
    //fwtite to file index
    write(index_file,records,sizeof(detail)*BUCKETNUMBER);
    //fwtite to map file
    write(map_file,fname_to_hv,sizeof(map)*BUCKETNUMBER);
    //fwrite to dbini
    write(ini_file,dbini,sizeof(Config)*2);

    close(index_file);
    close(map_file);
    close(ini_file);
}
int GetOffset(int file){
    return lseek(file,0,SEEK_END);
}
int GetFileSize(int file){
    return lseek(file,0,SEEK_END);
}
unsigned long int Gethv(unsigned char *data,unsigned long int size){
    return hash33(data,size);
}
void GetFileId(char *path){
    sprintf(db_path,"%s%d",path,dbini[0].CURFILEID);
}

int PutFile(char *filename,char *relfilename,int index_file,int map_file,int ini_file,char *path,char *newfilename){
    unsigned char *data;
    unsigned long int filesize;
    int data_file,db_file;

    int len=0,cnt=0;
    int offset;
    int option;
    unsigned long int index_record,index_map,index,hv;
    if(strcmp(newfilename,"")!=0){
        strcpy(relfilename,"");
        strcpy(relfilename,newfilename);
    }
    data_file = open(filename,O_RDONLY|O_EXCL);

    if(CheckFile(data_file,filename)==1){
        WriteAll(index_file,map_file,ini_file);
        return 1;
    }
    data = malloc(sizeof(unsigned char)*DATASIZE);
    filesize=read(data_file,data,sizeof(unsigned char)*DATASIZE);
    //filesize = GetFileSize(data_file);
    close(data_file);
    printf("read:%s\tsize:%lu\n",filename,filesize);

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
    GetFileId(path);
    db_file = open(db_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
    if(CheckFile(db_file,db_path)==1){
        WriteAll(index_file,map_file,ini_file);
        return 1;
    }
    offset = GetOffset(db_file);

    //if current file offset + data size > DBFILESIZE, then open a new file(file_n) to store data
    if(offset+filesize>dbini[0].MAXDBFILESIZE){
        printf("[offset:%d,size:%lu->limit %ld size]\n",offset,filesize,dbini[0].MAXDBFILESIZE);
        if(dbini[0].CURFILEID+1>=dbini[0].DBFILENUM){
            printf("[X] Max File ID is %d, current file num is %d\n",dbini[0].DBFILENUM,dbini[0].CURFILEID+1);
            WriteAll(index_file,map_file,ini_file);
            return 1;
        }
        else{
            printf("file [%d] is full, now open file [%d]\n",dbini[0].CURFILEID,dbini[0].CURFILEID+1);
            dbini[0].CURFILEID = dbini[0].CURFILEID+1;
        }
        write(ini_file,dbini,sizeof(Config)*2);
        GetFileId(path);
        close(db_file);
        db_file = open(db_path,O_WRONLY|O_CREAT,S_IRWXU|S_IRGRP);
        if(CheckFile(db_file,db_path)==1){
            WriteAll(index_file,map_file,ini_file);
            return 1;
        }
    }   
    //store into index
    records[index_record].key = hv;
    records[index_record].file_id = dbini[0].CURFILEID;
    records[index_record].size = filesize;
    records[index_record].offset = offset;

    //store map
    hv = Gethv((unsigned char *)relfilename,(unsigned long int)strlen(relfilename));
    index_map = hv % BUCKETNUMBER;
    strncpy(fname_to_hv[index_map].filename,relfilename,strlen(relfilename)+1);
    fname_to_hv[index_map].key = index_record;

    WriteAll(index_file,map_file,ini_file);

    //fwrite to db
    write(db_file,data,sizeof(char)*filesize);
    close(db_file);

    return 0;
}

int GetFile(char *filename,int size,int option,char *newfilename,int ini_file,char *path){
    unsigned long int hv,index;
    int index_record;
    int cnt;
    char *data;
    int db_file,result,index_file,map_file;
    

    
    hv = Gethv((unsigned char *)filename,(unsigned long int)size);
    index = hv % BUCKETNUMBER;
    if(strcmp(fname_to_hv[index].filename,filename)==0){
        if(option==PUT||option==RENAME){//option 3 is put status,so just ckeck filename exists or not
            printf("(filename [%s] exist!)\n",fname_to_hv[index].filename);
            return index;
        }
        index_record = fname_to_hv[index].key;
        printf("get file index [%d]:file [%d] %u\t%d\t%d\n",index_record,records[index_record].file_id,records[index_record].key,records[index_record].offset,records[index_record].size);

        sprintf(db_path,"%s%d",path,records[index_record].file_id);
        db_file = open(db_path,O_RDONLY);
        if(CheckFile(db_file,db_path)==1){
            WriteAll(index_file,map_file,ini_file);
            return 1;
        }
        lseek(db_file,records[index_record].offset,SEEK_SET);
        data = malloc(sizeof(char)*records[index_record].size);
        read(db_file,data,sizeof(char)*records[index_record].size);
        if(option==GET){
            if(strcmp(newfilename,"")!=0){
                strcpy(filename,"");
                strcpy(filename,newfilename);
            }
            sprintf(result_path,"%s%s",result_path,filename);
            printf("download file [%s]\n",filename);
        }
        
        result = open(result_path,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU|S_IRGRP|S_IROTH);
        if(CheckFile(result,result_path)==1){
            WriteAll(index_file,map_file,ini_file);
            return 1;
        }
        write(result,data,sizeof(char)*records[index_record].size);
        close(db_file);
        close(result);
        return 1;
    }
    else{
        if(option!=RENAME){
            if(strcmp(newfilename,"")!=0){
                strcpy(filename,"");
                strcpy(filename,newfilename);
            }
        }
        printf("(filename %s doesn't exist!)\n",filename);
        return 0;
    }
}
char *Rename(char *filename,int option,int ini_file,char *path,char *newfilename){
    char *new;
    char *delim=".";
    char *p[FILENAMELENS];
    char *temp,*type;
    int cnt=0;
    temp = filename;
    p[cnt] = strtok(temp,delim);
    //printf("[%d]%s\n",cnt-1,p[cnt-1]);
    while(p[cnt]){
        //printf("[%d]%s\n",cnt,p[cnt]);
        p[++cnt]=(strtok(NULL,delim));
        //cnt++;
    }
    new = malloc(sizeof(char)*FILENAMELENS);

    if(option==RENAME&&strcmp(newfilename,"")!=0){
        sprintf(new,"%s",newfilename);
        strcpy(newfilename,"");
    }
    else{
        if(cnt>1){
            type = p[cnt-1];
            if(strcmp(newfilename,"")!=0){
                sprintf(new,"%s.%s",newfilename,type);
            }
            else{
                sprintf(new,"%s_%d.%s",p[0],1,type);
            }
        }
        else{
            type="";
            if(strcmp(newfilename,"")!=0){
                sprintf(new,"%s",newfilename);
            }
            else{
                sprintf(new,"%s_%d",p[0],1);
            }
        }
    }
    if(GetFile(new,strlen(new),option,newfilename,ini_file,path)!=0){//if this filename exists
        new = Rename(new,option,ini_file,path,newfilename);    
    }
    return new;
}
int CheckFile(int fd,char *filename){
    if(fd<0){
        printf("Opening file %s: Failed\n",filename);
        printf ("Error no is : %d\n", errno);
        printf("Error description is : %s\n",strerror(errno));
        return 1;
    }
    return 0;
}
int ReadIniFile(char *filename,int option){
    int index_file;
    int cnt=0;
    index_file = open(filename,O_RDONLY);
    if(index_file>=0){
        read(index_file,dbini,sizeof(Config)*2);
        if(option==1){
            for(cnt=0;cnt<2;cnt++){
                if(dbini[cnt].DBFILENUM!=0){
                    printf("dbini file->DBFILENUM:%d\tMAXDBFILESIZE:%ld\tCURFILEID:%d\n",dbini[cnt].DBFILENUM,dbini[cnt].MAXDBFILESIZE,dbini[cnt].CURFILEID);
                }
            }
        }
        close(index_file);
        return 0;
    }
    else{
        printf("Init %s\n",filename);
        close(index_file);
        return 1;
    }
}
int ReadIndexFile(char *filename,int option){
    int index_file;
    int cnt=0;
    index_file = open(filename,O_RDONLY);
    if(index_file>=0){
        read(index_file,records,sizeof(detail)*BUCKETNUMBER);
        if(option==1){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(records[cnt].key!=0){
                    printf("[%d]index file:%u\t%d\t%d\t%d\n",cnt,records[cnt].key,records[cnt].file_id,records[cnt].offset,records[cnt].size);
                }
            }
        }
        close(index_file);
        return 0;
    }
    else{
        printf("Init %s\n",filename);
        close(index_file);
        return 1;
    }
}
int ReadMapFile(char *filename, int option){
    int index_file;
    int cnt=0;
    index_file = open(filename,O_RDONLY);
    if(index_file>=0){
        read(index_file,fname_to_hv,sizeof(map)*BUCKETNUMBER);
        if(option==1){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(fname_to_hv[cnt].key!=0&&fname_to_hv[cnt].key!=-1){
                    printf("[%d]map file:%s->%d\n",cnt,fname_to_hv[cnt].filename,fname_to_hv[cnt].key);
                }
            }
        }
        close(index_file);
        return 0;
    }
    else{
        printf("Init %s\n",filename);
        close(index_file);
        return 1;
    }
}
unsigned long int hash33(unsigned char *key,unsigned long int size)
{
    unsigned char *ptr = key;
    unsigned int hv = 0;
    int cnt=0;
    while(cnt<size){
        hv = ((hv << 5) + hv) + *ptr;//2^5+1=33
        ptr++;
        cnt++;
    }
    return hv;
}
