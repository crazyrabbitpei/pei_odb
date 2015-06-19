#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<ctype.h>
#include<unistd.h>
#include <openssl/md5.h>
#include <errno.h>
#include<time.h>
#include "cgic.h" 
#include "rdb.h" 

typedef enum{
    OFF,
    ON
}show;


typedef struct{
    /*index file format(record object position in file_[id]) */
    unsigned long int key;
    int file_id;
    int offset;
    int size;
}detail;

typedef struct{
    /*db init setting*/
    int DBFILENUM;
    long int MAXDBFILESIZE;
    //int BUCKETNUMBER;
    int CURFILEID;
}Config;

//TODO
Config init(char *set);
detail GetIndexFile(int fileid);//to get index[fileid] current offset,size

int GetOffset(int file);
int GetFileSize(int file);
void GetFileId(char *path);
unsigned long int Gethv(unsigned char *data,unsigned long int size);
unsigned long int hash33(unsigned char *key,unsigned long int size);
/*-----------------------------------*/
int ReadIniFile(char *filename,int option);
int ReadIndexFile(char *filename,int option);
int ReadMapFile(char *filename,int option);
int ReadDirMapFile(char *filename, int option);
int ReadNameFile(char *filename,int option, int command, char *relfilename,int page, char *dirname);

void WriteAll(int index_file,int map_file,int map_dir,int ini_file,int name_file,int id_file,int id_dir);
int CheckFile(int fd,char *filename);
void msg();
/*-----------------------------------*/
/*
   void StoreToDB();
   void StoreToIndexFile();
   void StoreToMap();
 */
/*-----------------------------------*/
unsigned long int GetFile(char *getid,char *filename,int size,int option,char *newfilename,int ini_file,int name_file,char *path);
int PutFile(int dir_path ,char *filename,char *relfilename,int index_file,int map_file,int map_dir,int ini_file,int name_file,char *path,char *newfilename);
char *Rename(char *getid,char *filename,int option,int ini_file,int name_file,char *path,char *newfilename);
//TODO : package function
/*
   void ListFile();
   void DeleteFile();
 */
Config dbini[DBININUM];
detail records[BUCKETNUMBER]={-1};
map file_list[BUCKETNUMBER]={-1};
dir_map dir_list[BUCKETNUMBER];
    

int index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir;
//odb
char index_path[100]="./db/index";
char db_path[100]="";
char map_path[100]="./db/map";
char dir_map_path[100]="./db/dir_map";
//rdb
char id_record_file[100]="./db/id_file";
char id_record_dir[100]="./db/id_dir";
char dfile_map_path[100]="./db/dfile_map";
char dfile_path[100]="./db/dfile";
int id,dir_id;   
//db config
char config_path[100]="./db/init";
char result_path[100]="./db/download/";

//void unencode(char *src, char *last, char *dest);
void unencode(char *src, char *last, char *dest)
{
    for(; src != last; src++, dest++)
        if(*src == '+')
            *dest = ' ';
        else if(*src == '%') {
            int code;
            if(sscanf(src+1, "%2x", &code) != 1) code = '?';
            *dest = code;
            src +=2; 
        }     
        else
            *dest = *src;
    *dest = '\n';
    *++dest = '\0';

}

int cgiMain()
//int cgiMain(int argc, char *argv[])
{
    char filename[FILENAMELENS],relfilename[FILENAMELENS]="",newfilename[FILENAMELENS]="",dirname[FILENAMELENS]="",getid[FILENAMELENS],column[50];
    int option=-1,cnt,index,page=1;
    char temp[3]="";
    char t[100]="0";
    char path[100]="./db/file_";
    char type[10];
    char temp_path[1000];
    char temp_newpath[1000];
    int dir_path,dir_newpath,temp_id;

#if 1
    char command[10],*cm;
    char *method;
    int len;
    int m,n;
    int output,data_file,des_file;
    char date[50];
    char *input, *data;


    input = malloc(sizeof(char)*DATASIZE);
    data = malloc(sizeof(char)*DATASIZE);

    //check method
    method = getenv("REQUEST_METHOD");
    if(strcmp(method,"POST")!=0){
        printf("<p>Method wrong.</p>");
        return 1;
    }
    //fprintf(cgiOut,"<p>method:%s</p>",method);
    ////------------------------------------------------////
    /*                   Command  detect                  */
    ////------------------------------------------------////
    //check query
    if(cgiFormString("command", command, sizeof(command))==cgiFormNotFound){
         printf("<p>Command [%s] doesn't exist!</p>",command);
         return 1;
    }

    if(strcmp(command,"PUT")==0){
        id_file = open(id_record_file,O_RDWR|O_CREAT,S_IRWXU|S_IRWXG);
        read(id_file,t,sizeof(char*));
        id = atoi(t);
        close(id_file);

        printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=PUT;
        if(cgiFormString("path", temp_path, sizeof(temp_path))==cgiFormNotFound){
             printf("<p>Path [%s] doesn't exist!</p>",path);
             return 1;
        }
        dir_path = atoi(temp_path);
    }
    else if(strcmp(command,"GET")==0){
        option=GET;
        //printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        
        if(cgiFormString("filename", filename, sizeof(filename))==cgiFormNotFound){
             printf("<p>Filename [%s] doesn't exist!</p>",filename);
             return 1;
        }
        
        if(cgiFormString("getid", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
        
        //printf("option:GET,getid:%s,filename:%s",getid,filename);
        //return 0;
    }
    else if(strcmp(command,"LIST")==0){
    printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=LIST;
        if(cgiFormString("search", dirname, sizeof(dirname))==cgiFormNotFound){
             printf("<p>Filename [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("column", column, sizeof(column))==cgiFormNotFound){
             printf("<p>Column [%s] doesn't exist!</p>",filename);
             return 1;
        }
        
        if(cgiFormString("page", temp, sizeof(temp))==cgiFormNotFound){
             printf("<p>Page [%s] doesn't exist!</p>",temp);
             return 1;
        }
        page = atoi(temp);
    }
    else if(strcmp(command,"FIND")==0){
    printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=FIND;
        if(cgiFormString("search", filename, sizeof(filename))==cgiFormNotFound){
             printf("<p>Filename [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("column", column, sizeof(column))==cgiFormNotFound){
             printf("<p>Column [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("page", temp, sizeof(temp))==cgiFormNotFound){
             printf("<p>Page [%s] doesn't exist!</p>",temp);
             return 1;
        }
        page = atoi(temp);
    }
    else if(strcmp(command,"DETAIL")==0){
    printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option = DETAIL;
    }
    else if(strcmp(command,"DEL")==0){
    printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=DEL;
        strcpy(type,"file");
        if(cgiFormString("filename", filename, sizeof(filename))==cgiFormNotFound){
             printf("<p>DEL:Filename [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("getid", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
        if(cgiFormString("path", temp_path, sizeof(temp_path))==cgiFormNotFound){
             printf("<p>Path [%s] doesn't exist!</p>",path);
             return 1;
        }
        dir_path = atoi(temp_path);
    }
    else if(strcmp(command,"DELD")==0){
        printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=DELD;
        strcpy(type,"dir");
        if(cgiFormString("filename", filename, sizeof(filename))==cgiFormNotFound){
             printf("<p>DEL:Filename [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("getid", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
        if(cgiFormString("path", temp_path, sizeof(temp_path))==cgiFormNotFound){
             printf("<p>Path [%s] doesn't exist!</p>",path);
             return 1;
        }
        dir_path = atoi(temp_path);
    }
    else if(strcmp(command,"MOVEF")==0){
        printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=MOVEF;
        strcpy(type,"file");
        if(cgiFormString("getid", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
        if(cgiFormString("path", temp_path, sizeof(temp_path))==cgiFormNotFound){
             printf("<p>Path [%s] doesn't exist!</p>",path);
             return 1;
        }
        if(cgiFormString("newpath", temp_newpath, sizeof(temp_newpath))==cgiFormNotFound){
             printf("<p>Path [%s] doesn't exist!</p>",path);
             return 1;
        }
        dir_path = atoi(temp_path);
        dir_newpath = atoi(temp_newpath);
        //printf("dir path:%d,new_path:%d,getid:%d",dir_path,dir_newpath,atoi(getid));
        //return -1;
    }
    else if(strcmp(command,"MOVED")==0){
        printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=MOVED;
        strcpy(type,"dir");
        if(cgiFormString("getid", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
        if(cgiFormString("path", temp_path, sizeof(temp_path))==cgiFormNotFound){
             printf("<p>Path [%s] doesn't exist!</p>",path);
             return 1;
        }
        if(cgiFormString("newpath", temp_newpath, sizeof(temp_newpath))==cgiFormNotFound){
             printf("<p>Path [%s] doesn't exist!</p>",path);
             return 1;
        }
        dir_path = atoi(temp_path);
        dir_newpath = atoi(temp_newpath);
        //printf("dir path:%d,new_path:%d,getid:%d",dir_path,dir_newpath,atoi(getid));
        //return -1;
    }
    else if(strcmp(command,"RENAME")==0){
    printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=RENAME;
        strcpy(type,"file");
        if(cgiFormString("filename", filename, sizeof(filename))==cgiFormNotFound){
             printf("<p>RENAME:Filename [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("newfilename", newfilename, sizeof(newfilename))==cgiFormNotFound){
            printf("New filename?\n");
            return 1;
        }
        if(cgiFormString("getid", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
    }
    else if(strcmp(command,"RENAMED")==0){
    printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=RENAMED;
        strcpy(type,"dir");
        if(cgiFormString("filename", filename, sizeof(filename))==cgiFormNotFound){
             printf("<p>RENAME:Filename [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("newfilename", newfilename, sizeof(newfilename))==cgiFormNotFound){
            printf("New filename?\n");
            return 1;
        }
        if(cgiFormString("getid", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
    }
    else if(strcmp(command,"EDITF")==0){
        option=EDITF;
        strcpy(type,"file");
        if(cgiFormString("column", column, sizeof(column))==cgiFormNotFound){
             printf("<p>Column [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("search", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
        printf("command:%d,column:%s,getid:%s,type:%s",option,column,getid,type);
        return 0;
    }
    else if(strcmp(command,"EDITD")==0){
        option=EDITD;
        strcpy(type,"dir");
        if(cgiFormString("column", column, sizeof(column))==cgiFormNotFound){
             printf("<p>Column [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("getid", getid, sizeof(getid))==cgiFormNotFound){
             printf("<p>id [%s] doesn't exist!</p>",getid);
             return 1;
        }
        printf("command:%d,column:%s,getid:%s,type:%s",option,column,getid,type);
        return 0;
    }
    else if(strcmp(command,"CDIR")==0){
        id_dir = open(id_record_dir,O_RDWR|O_CREAT,S_IRWXU|S_IRWXG);
        read(id_dir,t,sizeof(char*));
        dir_id = atoi(t);
        close(id_dir);

        printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        option=CDIR;
        if(cgiFormString("filename", filename, sizeof(filename))==cgiFormNotFound){
             printf("<p>Foldername [%s] doesn't exist!</p>",filename);
             return 1;
        }
        if(cgiFormString("path", temp_path, sizeof(temp_path))==cgiFormNotFound){
             printf("<p>Path [%s] doesn't exist!</p>",path);
             return 1;
        }
        dir_path = atoi(temp_path);
        //printf("id:%d->t:%s\n",id,t);
        //return 0;
    }
    else{
        printf("%s%c%c\n","Content-Type:text/html;charset=utf-8",13,10);
        fprintf(cgiOut, "<p>Illegal option:[%s]</p>", command);
        if(cgiFormFileName("filename", filename, sizeof(filename)) != cgiFormSuccess){
            fprintf(cgiOut,"<p>Filename [%s] doesn't exist.</p>\n",filename);
            return 1;
        }
        fprintf(cgiOut,"<p>filename:%s</p>",filename);
        return 1;
    }

    if(option==PUT){
        if(cgiFormFileName("filename", filename, sizeof(filename)) != cgiFormSuccess){
            fprintf(cgiOut,"<p>Filename [%s] doesn't exist.</p>\n",filename);
            return 1;
        }
        //strcpy(relfilename,filename);
    }
    strcpy(relfilename,filename);
/*
    int choice;
    cnt=0;
    while((choice = getopt( argc, argv, "f::c:o::l"))!=-1){
        switch(choice)
        {
            case 'f':
                filename = malloc(sizeof(char)*strlen(optarg));
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
                else if(strcmp(optarg,"DETAIL")==0){
                    option = DETAIL;
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
                fprintf(stderr, "Not supported option:%c\n",choice);
                break;
        }
    }
    if(optind!=argc||option==-1){
        msg();
    }
    if(option==PUT||option==GET||option==RENAME||option==DEL){
        if(strcmp(relfilename,"")==0){
            msg();
        }    
    }
*/
    ////------------------------------------------------////
    /*           Read config,index,map file               */
    ////------------------------------------------------////
    /*---------------read config file---------------*/
    if(ReadIniFile(config_path,OFF)==0){//dbini exists
        GetFileId(path);
        ini_file = open(config_path,O_WRONLY);
        if(CheckFile(ini_file,config_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            //return 0;
            exit(1);
        }
    }
    //TODO:Config by user
    else{//dbini doesn't exist,init now. Also id_record
        ini_file = open(config_path,O_WRONLY|O_CREAT,S_IRWXU);
        id_file = open(id_record_file,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU|S_IRWXG);
        id_dir = open(id_record_dir,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU|S_IRWXG);
        if(CheckFile(ini_file,config_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
        }
        write(id_file,"0",sizeof(char));
        write(id_dir,"0",sizeof(char));
        id=0;
        dir_id=0;
        close(id_file);
        close(id_dir);
        

        dbini[0].DBFILENUM=10;
        dbini[0].MAXDBFILESIZE=DATASIZE;
        dbini[0].CURFILEID=0;
        ReadIniFile(config_path,OFF);
        write(ini_file,dbini,sizeof(Config)*DBININUM);
        GetFileId(path);
    }
    /*---------------read index file----------------*/
    if(ReadIndexFile(index_path,OFF)==0){
        index_file = open(index_path,O_WRONLY);
        if(CheckFile(index_file,index_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            //return 0;
            exit(1);
        }
    }
    else{
        index_file = open(index_path,O_WRONLY|O_CREAT,S_IRWXU);
        if(CheckFile(index_file,index_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            //return 0;
            exit(1);
        }
        for(cnt=0;cnt<BUCKETNUMBER;cnt++){
            records[cnt].key=-1;
        }
        write(index_file,records,sizeof(detail)*BUCKETNUMBER);
    }
    /*---------------read map file----------------*/
    if(ReadMapFile(map_path,OFF)==0){
        map_file = open(map_path,O_WRONLY);
        if(CheckFile(map_file,map_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            //return 0;
            exit(1);
        }
    }
    else{
        map_file = open(map_path,O_WRONLY|O_CREAT,S_IRWXU);
        if(CheckFile(map_file,map_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            //return 0;
            exit(1);
        }
        for(cnt=0;cnt<BUCKETNUMBER;cnt++){
            file_list[cnt].key=-1;
        }
        write(map_file,file_list,sizeof(map)*BUCKETNUMBER);
    }
    /*---------------read dir_map file----------------*/
    if(ReadDirMapFile(dir_map_path,OFF)==0){
        map_dir = open(dir_map_path,O_WRONLY);
        if(CheckFile(map_dir,dir_map_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
        }
    }
    else{
        map_dir = open(dir_map_path,O_WRONLY|O_CREAT,S_IRWXU);
        if(CheckFile(map_dir,dir_map_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
        }
        for(cnt=0;cnt<BUCKETNUMBER;cnt++){
            dir_list[cnt].key=-1;
        }
        write(map_dir,dir_list,sizeof(dir_map)*BUCKETNUMBER);
    }
    /*---------------read dir_map and file_map----------------*/
    //map for file/dir description(gais reocrd)
    
    if(ReadNameFile(dfile_map_path,OFF,option,relfilename,page,dirname)==0){
        name_file = open(dfile_map_path,O_WRONLY|O_APPEND);
        if(CheckFile(name_file,dfile_map_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
        }
    }
    else{
        name_file = open(dfile_map_path,O_WRONLY|O_APPEND|O_CREAT,S_IRWXU);
        if(CheckFile(name_file,dfile_map_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
        }
        if(CreateDir("/",-1)==-1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
        }
    }
    
    ////------------------------------------------------////
    /*   Check if filename exist, and rename for import   */
    /*   status(3) or get the file(1 or 2 )               */
    ////------------------------------------------------////
    /*separate real file name*/
    /*
    if(option!=LIST&&option!=DETAIL){
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
        strcpy(relfilename,p[cnt-1]);
    }
    */
    //GET:download file use orign name
    //GET & RENAME:download file use refer name(newfilename)
    //PUT:just for check filename exists or not(import status)
    if(option==GET){
        if(GetFile(getid,relfilename,strlen(relfilename),option,newfilename,ini_file,name_file,path)!=0){//if this filename exists
            if(option==PUT){
                //printf("filename [%s] exists,",relfilename);
                //strcpy(relfilename,Rename(relfilename,option,ini_file,name_file,path,newfilename));
                //printf("so will rename to [%s]\n",relfilename);
            }
            else if(option==GET){
                WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
                return 0;   
            }

        }
    }
    ////------------------------------------------------////
    /*                      PutFile                       */
    ////------------------------------------------------////
    if(option==PUT){
        if(PutFile(dir_path,filename,relfilename,index_file,map_file,map_dir,ini_file,name_file,path,newfilename)==0){//success import
            printf("Import Success.\n");
        }
        else{
            printf("Nothing import.\n");
        }
        WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
        return 0;
    }

    ////------------------------------------------------////
    /*                  Move folder                       */
    ////------------------------------------------------////
    if(option==MOVEF||option==MOVED){
        des_file = open(dfile_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
        getDir(atoi(getid),dir_path,dir_newpath,des_file,type,option);
        close(des_file);
        
        WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
        return 0;
    }
    ////------------------------------------------------////
    /*  Delete file, will not really delete data from     */
    /*  db file, but will delete from map and name file.  */
    /*  So, if next time, when content that had been      */
    /*  imported before be imported again, will show      */
    /*  content exists.                                   */
    ////------------------------------------------------////
    if(option==DEL||option==DELD){
        if((index=GetFile(getid,relfilename,strlen(relfilename),option,newfilename,ini_file,name_file,path))!=-1){//if this filename exists
            //rdb
            if(option==DELD){
                des_file = open(dfile_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
                getDir(index,dir_path,-1,des_file,"dir",option);
                close(des_file);
            //odb
            }
            else if(option==DEL){//Delete dir doesn't need to mark odb file
                //file_list[index].key = -1;
                des_file = open(dfile_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
                getDir(index,dir_path,-1,des_file,"file",option);
                close(des_file);

            }
            printf("Delete %s [%s],id:%d\n",type,relfilename,index);
        }
        else{ 
            printf("%s [%s] doesn't exist.\n",type,relfilename);
        }
        WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
        return 0;
    }

    ////------------------------------------------------////
    /*                  Rename file                       */
    ////------------------------------------------------////
    if(option==RENAME||option==RENAMED){
        if(strcmp(newfilename,"")==0){
            printf("New filename?\n");
            msg();
        }
        unsigned long int index,key,hv,newindex;
        char newdate[100];
        char newtype[100];
        int newsize=0,newoffset=0;
        if((index=GetFile(getid,relfilename,strlen(relfilename),option,newfilename,ini_file,name_file,path))!=-1){
            printf("rename file [%s] to ",relfilename);
            strcpy(relfilename,Rename(getid,relfilename,option,ini_file,name_file,path,newfilename));
            printf("[%s]\n",relfilename);
            //TODO:when file's newrename is same as dir name ,will occur error, dir will disappear
            /*record file/dir 's data position' and reset info which at old name position*/
            //odb
            if(option==RENAME){
                strcpy(file_list[index].filename,"");
                strcpy(file_list[index].filename,relfilename);
                //key = file_list[index].key;
                //file_list[index].key=-1;
            }
            //rdb
            else{
                //newsize = dir_list[index].size;
                //newoffset = dir_list[index].offset;
                strcpy(dir_list[index].filename,"");
                strcpy(dir_list[index].filename,relfilename);
                //dir_list[index].key = -1;
                //dir_list[index].size=-1;
                //dir_list[index].offset=-1;
            }
            //get new index with new name
            //hv = Gethv((unsigned char *)relfilename,(unsigned long int)strlen(relfilename));
            //newindex = hv % BUCKETNUMBER;

            //odb
            //if(option==RENAME){
            //    file_list[newindex].key = key;
            //}
            //rdb
            /*
            strcpy(dir_list[newindex].filename,relfilename);
            dir_list[newindex].key = hv;
            dir_list[newindex].size=newsize;
            dir_list[newindex].offset=newoffset;
            printf("new key:%lu\tsize:%d\toffset:%s",dir_list[newindex].key,dir_list[newindex].size,dir_list[newindex].filename);
            */
        }
        else{ 
            printf("file [%s] doesn't exist.\n",relfilename);
        }
        WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
        return 0;
    }

    ////------------------------------------------------////
    /*      Check data import successfuly or not          */
    ////------------------------------------------------////
    if(option==DETAIL){
        ReadIndexFile(index_path,ON);     
        ReadMapFile(map_path,ON);     
    }

    else if(option==FIND){
        /*
        if(ReadNameFile(dir_map_path,ON,option,relfilename,page,dirname)==-1){
            //printf("File [%s] doesn't exist.</br>",relfilename);
        } 
        */
        return 0;
    }

    ////------------------------------------------------////
    /*            List data by column and rid             */
    ////------------------------------------------------////
    if(option==LIST){
        //ReadNameFile(dir_map_path,ON,option,relfilename,page,dirname);     
        rdb_read(atoi(dirname),column,"dir");
        return 0;
    }
    ////------------------------------------------------////
    /*                  Create Folder                     */
    ////------------------------------------------------////
    if(option==CDIR){
        if((temp_id=CreateDir(relfilename,dir_path))==-1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
        }
        printf("id:%d",temp_id);
        //printf("Create dir [%s] success!\n",relfilename);
    }
    WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
    /*
    close(index_file);
    close(map_file);
    close(name_file);
    close(ini_file);
    */
#endif
    return 0;
}


void msg(){
    fprintf(stderr,"usage:./a.out -f [filename] -c [command] -o [newfilename]\n");
    exit(1);
}

void WriteAll(int index_file,int map_file,int map_dir,int ini_file,int name_file,int id_file,int id_dir){
    /*---------------write file's detail---------------*/    
    //fwrite to file index
    write(index_file,records,sizeof(detail)*BUCKETNUMBER);
    //fwrite to map file
    write(map_file,file_list,sizeof(map)*BUCKETNUMBER);
    //fwrite to name file
    write(map_dir,dir_list,sizeof(dir_map)*BUCKETNUMBER);
    //fwrite to dbini
    write(ini_file,dbini,sizeof(Config)*DBININUM);

    close(index_file);
    close(map_file);
    close(map_dir);
    close(name_file);
    close(ini_file);
    close(id_file);
    close(id_dir);
}
int GetOffset(int file){
    return lseek(file,0,SEEK_END);
}
int GetFileSize(int file){
    int len = lseek(file,0,SEEK_END);
    lseek(file,0,SEEK_SET);
    if(len>DATASIZE){
        //TODO :split data to small data
        printf("[X]data size > %d\n",DATASIZE);
        exit(1);
    }
    return len;
}
unsigned long int Gethv(unsigned char *data,unsigned long int size){
    //return 0;
    return hash33(data,size);
}
void GetFileId(char *path){
    sprintf(db_path,"%s%d",path,dbini[0].CURFILEID);
}

int PutFile(int dir_path,char *filename,char *relfilename,int index_file,int map_file,int map_dir,int ini_file,int name_file,char *path,char *newfilename){
    char temp[FILENAMELENS];
    char contentType[1024];
    int got;
    FILE *fp;
    cgiFilePtr file;
    
    //char date[100];
    char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    time_t current_time;
    struct tm *p;
    char* date;
    current_time = time(NULL);
    date = ctime(&current_time);
    /*
    time(&timep);
    p=gmtime(&timep);
    sprintf(date,"%d%d%d%d%d%d",(1900+p->tm_year), (1+p->tm_mon),p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
    */
    unsigned char *data;
    unsigned long int filesize;
    int data_file,db_file,des_file;

    int len=0,cnt=0;
    int offset,des_offset,size;
    int option;
    unsigned long int index_record,index_map,index,hv;
    
    if(option==PUT&&strcmp(newfilename,"")!=0){
        strcpy(relfilename,"");
        strcpy(relfilename,newfilename);
    }

    //read db to get current offset, and get filesize
    GetFileId(path);
    db_file = open(db_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
    if(CheckFile(db_file,db_path)==1){
        WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
        return 1;
    }
    offset = GetOffset(db_file);
    
    //read description file to get offset
    des_file = open(dfile_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
    if(CheckFile(des_file,dfile_path)==1){
        WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
        return 1;
    }
    des_offset = GetOffset(des_file);


    //TODO file size over 50kb will broken
    //filename
    fprintf(cgiOut, "The filename submitted was: ");
    cgiHtmlEscape(relfilename);
    //sprintf(temp,"touch ./data/%s",relfilename);
    //system(temp);
    fprintf(cgiOut, "<p>\n");
    //file size
    cgiFormFileSize("filename", &len);
    fprintf(cgiOut, "The file size was: %d bytes<p>\n", len);
    //content type
    cgiFormFileContentType("filename", contentType, sizeof(contentType));
    fprintf(cgiOut, "The alleged content type of the file was: ");
    cgiHtmlEscape(contentType);

    fprintf(cgiOut, "<p>\n");
    //ckeck download file
    if (cgiFormFileOpen("filename", &file) != cgiFormSuccess) {
        fprintf(cgiOut, "Could not open the file.<p>\n");
        return 1;
    }
    fprintf(cgiOut, "<pre>\n");

    fprintf(cgiOut,"read:%s\tsize:%d\n",filename,len);

    //if current file offset + data size > DBFILESIZE, then open a new file(file_n) to store data
    if(offset+len>dbini[0].MAXDBFILESIZE){
        fprintf(cgiOut,"[offset:%d,size:%d->limit %ld size]\n",offset,len,dbini[0].MAXDBFILESIZE);
        if(dbini[0].CURFILEID+1>=dbini[0].DBFILENUM){
            fprintf(cgiOut,"[X] Max File num is %d, current file num is %d\n",dbini[0].DBFILENUM,dbini[0].CURFILEID+1);
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            return 1;
        }
        else{
            fprintf(cgiOut,"file [%d] is full, now open file [%d]\n",dbini[0].CURFILEID,dbini[0].CURFILEID+1);
            dbini[0].CURFILEID = dbini[0].CURFILEID+1;
            offset=0;
        }
        write(ini_file,dbini,sizeof(Config)*DBININUM);
        GetFileId(path);
        close(db_file);
        //db_file = open(db_path,O_WRONLY|O_CREAT,S_IRWXU|S_IRGRP);
        if(CheckFile(db_file,db_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            return 1;
        }
    }   
    close(db_file);

    data = malloc(sizeof(unsigned char)*len);
    //fwrite to db
    //cgic method
    fp=fopen(db_path,"ab");
    while (cgiFormFileRead(file, data, sizeof(unsigned char)*len, &got) == cgiFormSuccess) 
    { 
        fwrite(data,1,got,fp);
    }
    fclose(fp);
    cgiFormFileClose(file);
    //old
    /*
    data_file = open(filename,O_RDONLY|O_EXCL);

    if(CheckFile(data_file,filename)==1){
        WriteAll(index_file,map_file,ini_file,name_file);
        return 1;
    }

    filesize = GetFileSize(data_file);

    read(data_file,data,sizeof(unsigned char)*filesize);
    close(data_file);
    */

    hv=0;
    hv = Gethv(data,len);
#if 1
    //printf("hv:%ld</br>",hv);
    index_record = hv % BUCKETNUMBER;
    //ckeck file content, if exist then ask to cover existing file or ignore this import action
    //printf("key:%ld</br>",records[index_record].key);

    if(records[index_record].key==hv){
        printf("file [%s] content  exist!</br>",relfilename);
        //printf("records[%ld].key:%ld=%ld</br>",index_record,records[index_record].key,hv);
        //only store map
        /*
        hv = Gethv((unsigned char *)relfilename,(unsigned long int)strlen(relfilename));
        index_map = hv % BUCKETNUMBER;
        */
        
        //rdb
        file_list[id].key = index_record;
        file_list[id].offset = des_offset;
        strcpy(file_list[id].filename,relfilename);
        fprintf(cgiOut, "file_list[%d].key=%d\n",id,file_list[id].key);
        printf("id:%d\n",id);
        //store gais record to dir_list
        size = StoreGais(relfilename,contentType,len,date,hv,des_file,dir_path);
        if(rdb_create(des_file,relfilename,des_offset,size,dir_path,contentType)==-1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
        }
        /*
        strncpy(dir_list[index_map].filename,relfilename,strlen(relfilename)+1);
        dir_list[index_map].key = hv;
        dir_list[index_map].offset = des_offset;
        */

        WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
        return 1;
    }
    //printf("write to index</br>");
    //store into index
    records[index_record].key = hv;
    records[index_record].file_id = dbini[0].CURFILEID;
    records[index_record].size = len;
    records[index_record].offset = offset;

    printf("records[%d].key=%d\n",index_record,records[index_record].key);
    //printf("write to map</br>");
    //store map
    //hv = Gethv((unsigned char *)relfilename,(unsigned long int)strlen(relfilename));
    //index_map = hv % BUCKETNUMBER;
    file_list[id].key = index_record;
    file_list[id].offset = des_offset;
    strcpy(file_list[id].filename,relfilename);
    fprintf(cgiOut, "file_list[%d].key=%d\n",id,file_list[id].key);
    printf("id:%d\n",id);
    
    //rdb
    //store gais record to dir_list
    size = StoreGais(relfilename,contentType,len,date,hv,des_file,dir_path);

    if(rdb_create(des_file,relfilename,des_offset,size,dir_path,contentType)==-1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            exit(1);
    }
    
    /*
    dir_list[index_map].size = StoreGais(relfilename,contentType,len,date,hv,des_file,dir_path);
    rdb_create(name_file);
    
    strncpy(dir_list[index_map].filename,relfilename,strlen(relfilename)+1);
    dir_list[index_map].key = hv;
    dir_list[index_map].offset = des_offset;
    */
  
#endif
    WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
    close(des_file);
    return 0;
}

unsigned long int GetFile(char *getid,char *filename,int size,int option,char *newfilename,int ini_file,int name_file,char *path){
    unsigned long int hv,index;
    int index_record;
    int cnt;
    char *data;
    int db_file,result,index_file,map_file,map_dir;
    index = atoi(getid);
    //printf("download file [%s],id:%d</br>",filename,index);
#if 1
    /*
    hv = Gethv((unsigned char *)filename,(unsigned long int)size);
    index = hv % BUCKETNUMBER;
    */

    //if(strcmp(file_list[index].filename,filename)==0){
    if(option==DELD||option==RENAMED){
        if(dir_list[index].key!=-1){
            return index;
        }
        else{
            return -1;
        }
    }
    //printf("file_list[%d].key=%d",index,file_list[index].key);
    if(file_list[index].key!=-1){
        //printf("(filename [%s] exist!)</br>",filename);
        //return 1;
        if(option==FIND||option==PUT||option==RENAME||option==DEL){//option 3 is put status,so just ckeck filename exists or not
            //printf("(filename [%s] exist!)</br>",filename);
            return index;
        }
        index_record = file_list[index].key;
        //printf("get file index [%d]:file [%d] %u&#09;%d&#09;%d</br>",index_record,records[index_record].file_id,records[index_record].key,records[index_record].offset,records[index_record].size);

        sprintf(db_path,"%s%d",path,records[index_record].file_id);
        db_file = open(db_path,O_RDONLY);
        if(CheckFile(db_file,db_path)==1){
            WriteAll(index_file,map_file,map_dir,ini_file,name_file,id_file,id_dir);
            return 1;
        }
        lseek(db_file,records[index_record].offset,SEEK_SET);
        data = malloc(sizeof(char)*records[index_record].size);
        read(db_file,data,sizeof(char)*(records[index_record].size));
        if(option==GET){
            if(strcmp(newfilename,"")!=0){
                strcpy(filename,"");
                strcpy(filename,newfilename);
            }
            sprintf(result_path,"%s%s",result_path,filename);
            //printf("download file [%s]</br>",result_path);
        }
        printf("%s%c%c","Content-Type: application/octet-stream",13,10);
        printf("%s%c%c","Content-Type:application/x-download",13,10);
        printf("%s%s%c%c","Content-Disposition: attachment;filename=",filename,13,10);
        fprintf(cgiOut,"Content-Length: %d\n\n",records[index_record].size);
        fwrite(data,sizeof(char),records[index_record].size,stdout);
        /*
        result = open(result_path,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU|S_IRGRP|S_IROTH);
        if(CheckFile(result,result_path)==1){
            WriteAll(index_file,map_file,ini_file,name_file);
            return 1;
        }
        write(result,data,sizeof(char)*records[index_record].size);
        */
        close(db_file);
        //close(result);
        return 1;
    }
    else{
        if(option==PUT){
            if(strcmp(newfilename,"")!=0){
                strcpy(filename,"");
                strcpy(filename,newfilename);
            }
        }
        //printf("(id %d doesn't exist!),fname[%d].key=%d</br>",index,index,file_list[index].key);
        return -1;
    }
#endif
}

char *Rename(char *getid,char *filename,int option,int ini_file,int name_file,char *path,char *newfilename){
    char *new;
    char *delim=".";
    char *p[FILENAMELENS];
    char temp[100],name[100];
    char type[FILENAMELENS];
    int cnt=0;
    int num=0;
    strcpy(temp,filename);
    p[cnt] = strtok(temp,delim);
    //printf("[%d]%s\n",cnt,p[cnt]);
    while(p[cnt]){
        //printf("[%d]%s\n",cnt,p[cnt]);
        p[++cnt]=(strtok(NULL,delim));
        //cnt++;
    }
    new = malloc(sizeof(char)*FILENAMELENS);

    if((option==RENAME||option==RENAMED)&&strcmp(newfilename,"")!=0){
        sprintf(new,"%s",newfilename);
        strcpy(newfilename,"");
    }
    else{
        if(cnt>1){
            strcpy(type,p[cnt-1]);
            if(strcmp(newfilename,"")!=0){
                sprintf(new,"%s.%s",newfilename,type);
            }
            else{
                //TODO : num wrong
                sscanf(p[0],"(%d)",&num);
                num++;
                //printf("</br>name:%s</br>num:%d</br>",p[0],num);
                sprintf(new,"%s(%d).%s",p[0],num,type);
            }
        }
        else{
            strcpy(type,"");
            if(strcmp(newfilename,"")!=0){
                sprintf(new,"%s",newfilename);
            }
            else{
                sscanf(p[0],"%s(%d)",name,&num);
                num++;
                sprintf(new,"%s(%d)",name,num);
            }
        }
    }
    /*
    if(GetFile(getid,new,strlen(new),option,newfilename,ini_file,name_file,path)!=0){//if this filename exists
        new = Rename(getid,new,option,ini_file,name_file,path,newfilename);    
    }
    */
    return new;
}
int CheckFile(int fd,char *filename){
    if(fd<0){
        printf("Opening file %s: Failed</br>",filename);
        printf ("Error no is : %d</br>", errno);
        printf("Error description is : %s</br>",strerror(errno));
        return 1;
    }
    return 0;
}
int ReadIniFile(char *filename,int option){
    int index_file;
    int cnt=0;
    index_file = open(filename,O_RDONLY);
    if(index_file>=0){
        read(index_file,dbini,sizeof(Config)*DBININUM);
        if(option==ON){
            for(cnt=0;cnt<DBININUM;cnt++){
                if(dbini[cnt].DBFILENUM!=0){
                    printf("dbini file->DBFILENUM:%d&#09;/MAXDBFILESIZE:%ld&#09;CURFILEID:%d</br>",dbini[cnt].DBFILENUM,dbini[cnt].MAXDBFILESIZE,dbini[cnt].CURFILEID);
                }
            }
        }
        close(index_file);
        return 0;
    }
    else{
        if(option==ON){
            printf("Init %s</br>",filename);
        }
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
        if(option==ON){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(records[cnt].key!=-1){
                    printf("[%d]index file:%ld&#09;%d&#09;%d&#09;%d</br>",cnt,records[cnt].key,records[cnt].file_id,records[cnt].offset,records[cnt].size);
                }
            }
        }
        close(index_file);
        return 0;
    }
    else{
        if(option==ON){
            printf("Init %s</br>",filename);
        }

        close(index_file);
        return 1;
    }
}
int ReadMapFile(char *filename, int option){
    int index_file;
    int cnt=0;
    index_file = open(filename,O_RDONLY);
    if(index_file>=0){
        read(index_file,file_list,sizeof(map)*BUCKETNUMBER);
        if(option==ON){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(file_list[cnt].key!=-1){
                    printf("[%d]map file:key:%d,filename:%s</br>",cnt,file_list[cnt].key,file_list[cnt].filename);
                }
            }
        }
        close(index_file);
        return 0;
    }
    else{
        if(option==ON){
            printf("Init %s</br>",filename);
        }
        close(index_file);
        return 1;
    }
}
int ReadDirMapFile(char *filename, int option){
    int index_file;
    int cnt=0;
    index_file = open(filename,O_RDONLY);
    if(index_file>=0){
        read(index_file,dir_list,sizeof(dir_map)*BUCKETNUMBER);
        if(option==ON){
            for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                if(dir_list[cnt].key!=-1){
                    printf("[%d]map file:key:%d,filename:%s</br>",cnt,dir_list[cnt].key,dir_list[cnt].filename);
                }
            }
        }
        close(index_file);
        return 0;
    }
    else{
        if(option==ON){
            printf("Init %s</br>",filename);
        }
        close(index_file);
        return 1;
    }
}
int ReadNameFile(char *filename, int option, int command, char *relfilename, int page, char *dirname){
    //Read gais records and then parse them
    int file_index;
    int cnt=0,num=0,i=0;
    int start=(page*16)-16;
    int end = page*16;
    char temp[RECORDLEN],temp_name[FILENAMELENS],type[100];

    char find_path[FILENAMELENS]="";


    int dir_id = atoi(dirname);
    file_index = open(filename,O_RDONLY);

    if(file_index>=0){
        if(option==ON){
            if(command==LIST){
                //get dir's all childs
                //rdb_read(dir_id,"child","dir");   

#if 0
                FILE *fp;



                fp = fopen(filename,"r");
                while(fgets(temp,100,fp)){
                    //printf("%s",temp);
                    fid = atoi(strtok(temp,delim));
                    strcpy(type,strtok(NULL,delim));
                    if(strcmp(type,"dir")==0){
                        if(dir_list[fid].key==-1){continue;}
                        strcpy(temp_name,dir_list[fid].filename);
                        
                    }
                    else{
                        if(file_list[fid].key==-1){continue;}
                        strcpy(temp_name,file_list[fid].filename);
                    }

                    offset = atoi(strtok(NULL,delim));
                    size = atoi(strtok(NULL,delim));

                    lseek(ds_file,offset,SEEK_SET);
                    data = malloc(sizeof(char)*size);
                    read(ds_file,data,sizeof(char)*(size));
                    //printf("%d,%s,%d,%d\n",fid,temp_name,offset,size);
                    //judge dirname
                    sprintf(find_path,"@path:%s",dirname);
                    if(strstr(data,find_path)){
                        printf("%d,%s,%s</br>",fid,temp_name,data);
                    }
                    free(data);
                    
                }
                fclose(fp);


                for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                    //if(strcmp(dir_list[cnt].filename,"")!=0){
                    if(dir_list[cnt].key!=0 && dir_list[cnt].key!=-1){
                        if(num>=start&&num<end){

                            //printf("[%d]Filename:%s</br>",cnt,dir_list[cnt].filename);
                            //printf("%d,%s,%s,%s</br>",dir_list[cnt].size,dir_list[cnt].filename,dir_list[cnt].date,dir_list[cnt].type);
                            //printf("%d,%s,%d</br>",dir_list[cnt].key,dir_list[cnt].filename,dir_list[cnt].offset);
                            lseek(ds_file,dir_list[cnt].offset,SEEK_SET);
                            data = malloc(sizeof(char)*dir_list[cnt].size);
                            read(ds_file,data,sizeof(char)*(dir_list[cnt].size));

                            /*------------*/
                            //judge dirname
                            sprintf(find_path,"@path:%s",dirname);

                            if(strstr(data,find_path)){

                                printf("%lu,%s,%s</br>",dir_list[cnt].key,dir_list[cnt].filename,data);
                            }
                            /*------------*/
                            free(data);
                        }
                        
                        num++;
                        if(num>=end){
                             break;
                        }
                        
                    }
                }
#endif
            }
            /*
            else if(command==FIND){
                for(cnt=0;cnt<BUCKETNUMBER;cnt++){
                    //if(strcmp(dir_list[cnt].filename,"")!=0){
                    if(dir_list[cnt].key!=-1){
                        for(i=0;i<strlen(dir_list[cnt].filename);i++){
                            temp[i] = tolower(dir_list[cnt].filename[i]);
                        }
                        temp[i]='\0';
                        if(strstr(temp,relfilename)!=0){
                            if(num>=start&&num<end){
                                //printf("[%d]Filename:%s</br>",cnt,dir_list[cnt].filename);
                                //printf("%d,%s,%s,%s</br>",dir_list[cnt].size,dir_list[cnt].filename,dir_list[cnt].date,dir_list[cnt].type);
                                //printf("%d,%s,%d</br>",dir_list[cnt].key,dir_list[cnt].filename,dir_list[cnt].offset);
                                printf("%d,%d</br>",dir_list[cnt].key,dir_list[cnt].offset);
                            }
                            num++;
                            if(num>=end){
                                break;
                            }
                        }
                    }
                }
                
                if(num==0){
                    printf("-1,-1,-1</br>");
                    close(index_file);
                    return -1;
                }
            }
            */
        }
        close(file_index);
        return 0;   
    }
    else{//file doesn't exist
        if(option==ON){
            printf("Init %s</br>",filename);
        }
        close(file_index);
        return 1;
    }
}
unsigned long int hash33(unsigned char *key,unsigned long int size)
{
    unsigned char *ptr = key;
    unsigned long int hv = 0;
    unsigned long int cnt=0;
    while(cnt<size){
        hv = ((hv << 5) + hv) + *ptr;//2^5+1=33
        ptr++;
        cnt++;
    }
    return hv;
}
