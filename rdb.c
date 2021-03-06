#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#include "rdb.h"

extern char dfile_path[];
extern int id,dir_id;

void getDir(int cid,int pid,int newpid,int fp,char *type,int command,char *column);
int rdb_update(int cid,int fp,char *type,int command,char *column,char *newdata);
int rdb_find(char *pattern,char *type,char *sensitive,char *offset,char *sortby,char *range,char *outputnum,char *outputcolumn,int total);
//int find_file(int count,Query *a1,char *sensitive);
void appendChild(int cid,char *record,char *type,int fp);
int deleteChild(int cid,char *record,char *type,int fp);
char *getColumn(char *record,char *column,char *type);
char *getRecord(int rid,char *type);
void print(int rid,char *type,char *data,char *outputcolumn,int sort,char *sortby);
int SensitiveCompare(char *s1, char *s2);
int compare_size(const void *a, const void *b);
int compare_name(const void *a, const void *b);
int compare_type(const void *a, const void *b);
int compare_date(const void *a, const void *b);

typedef struct{
        char column[10];
        char pattern[500];
        char **filter;
        int count;
}Query;
typedef struct{
        int rid;
        char filename[FILENAMELENS];
        char type[100];                                                                                                                        char date[30];
        int size;
        char ds[500];                                                                                                                          char tag[500];

}Sort;

Sort orderby[IDNUM];
int sortid;

int StoreGais(char *name,char *type,int len,char *date,unsigned long int key,int fp,int rid){
    char *record;
    int size;
    record = malloc(sizeof(char)*RECORDLEN);
    //sprintf(record,"@\n@type:%s\n@ctime:%s@size:%d\n",type,date,len);
    sprintf(record,"@\n@type:%s\n@ctime:%s\n@size:%d\n@ds:\n@tag:\n@path:%d\n@dchild:\n@fchild:\n",type,date,len,rid);
    write(fp,record,sizeof(char)*RECORDLEN);
    size = strlen(record);
    free(record);
    /*
    record = malloc(sizeof(char)*PATH);
    sprintf(record,"@path:%s",rid);
    write(fp,record,sizeof(char)*PATH);
    size += strlen(record);
    free(record);

    record = malloc(sizeof(char)*DCHILD);
    sprintf(record,"\n@dchild:");
    write(fp,record,sizeof(char)*DCHILD);
    size += strlen(record);
    free(record);

    record = malloc(sizeof(char)*FCHILD);
    sprintf(record,"\n@fchild:");
    write(fp,record,sizeof(char)*FCHILD);
    size += strlen(record);
    free(record);
    */
    return size;
}

int CreateDir(char *name,int rid){
    char date[25];
    struct tm *p;
    time_t timep;

    int des_file,des_offset,size;
    unsigned long int hv,index_map;
    time_t current_time;
    des_file = open(dfile_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
    if(CheckFile(des_file,dfile_path)==1){
        return -1;
    }
    des_offset = GetOffset(des_file);
    time(&timep);
    p=gmtime(&timep);
    sprintf(date,"%d %d %d %d:%d:%d",(1900+p->tm_year), (1+p->tm_mon),p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    /*
    char *date1;
    current_time = time(NULL);
    date1 = ctime(&current_time);
    */
    


    //TODO:file name same as dir name?
    /*  
    hv = Gethv((unsigned char *)name,(unsigned long int)strlen(name));
    index_map = hv % BUCKETNUMBER;
    if(dir_list[index_map].key!=-1 && dir_list[index_map].key!=0){
        dir_list[index_map].key = hv;
        return -1;
    }
    strcpy(dir_list[index_map].filename,name);
    dir_list[index_map].key = hv;
    dir_list[index_map].offset = des_offset;
    dir_list[index_map].size = StoreGais(name,"dir",0,date,hv,des_file,rid);
    */
    dir_list[dir_id].key = dir_id;
    dir_list[dir_id].offset = des_offset;
    strcpy(dir_list[dir_id].filename,name);
    size = StoreGais(name,"dir",0,date,hv,des_file,rid);
    if(rdb_create(des_file,name,des_offset,size,rid,"dir")==-1){
        return -1;
    }
    
    return dir_id-1;
}

int rdb_create(int fp,char *name,int offset, int size,int parent,char *type){
    char *record;

    //each row : id,name,offset,size
    record = malloc(sizeof(char)*RECORDLEN);
    if(strcmp(type,"dir")==0){
        sprintf(record,"%d,%s,%d,%d\n",dir_id,type,offset,size);
    }
    else{
        sprintf(record,"%d,%s,%d,%d\n",id,type,offset,size);
    }
    write(name_file,record,strlen(record));
    free(record);
    //add id

    if(strcmp(type,"dir")==0){
        //dir id
        dir_id++;
        id_dir = open(id_record_dir,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU|S_IRWXG);//clean old id
        if(CheckFile(id_dir,id_record_dir)==1){
            close(id_dir);
            return -1;
        }
        record = malloc(sizeof(char)*100);
        sprintf(record,"%d",dir_id);
        write(id_dir,record,strlen(record));
        free(record);
        close(id_dir);
        if(parent!=-1){
            getDir(dir_id-1,parent,-1,fp,type,CDIR,"");
        }
    }
    else{
        //file id
        id++;
        id_file = open(id_record_file,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU|S_IRWXG);//clean old id
        if(CheckFile(id_file,id_record_file)==1){
            close(id_file);
            return -1;
        }
        record = malloc(sizeof(char)*100);
        sprintf(record,"%d",id);
        write(id_file,record,strlen(record));
        free(record);
        close(id_file);
        getDir(id-1,parent,-1,fp,type,PUT,"");
    }


    return 0;
    
}
void getDir(int cid,int pid,int newpid,int fp,char *type,int command,char *column){
    char *record;
    int offset;
    record = malloc(sizeof(char)*RECORDLEN);
    offset = dir_list[pid].offset;
    lseek(fp,offset,SEEK_SET);
    read(fp,record,sizeof(char)*RECORDLEN);
    lseek(fp,offset,SEEK_SET);//reset dir offset for updating new record

    if(command==PUT||command==CDIR){
        appendChild(cid,record,type,fp);
    }
    else if(command==DELD||command==DEL){
        deleteChild(cid,record,type,fp);
    }
    else if(command==MOVEF||command==MOVED){
        deleteChild(cid,record,type,fp);
        
        free(record);
        record = malloc(sizeof(char)*RECORDLEN);
        offset = dir_list[newpid].offset;
        lseek(fp,offset,SEEK_SET);
        read(fp,record,sizeof(char)*RECORDLEN);
        lseek(fp,offset,SEEK_SET);//reset dir offset for updating new record

        appendChild(cid,record,type,fp);
        
    }
    /*
    else if(command==EDITF||command==EDITD){
        rdb_update(cid,record,type,fp,column);
    }
    */

    printf("command:%d,dir information:[%s]\n",command,record);
    free(record);
    return ;   
}
void appendChild(int cid,char *record,char *type,int fp){
    char *p,*p1,*p2,*result;
    char temp[RECORDLEN];
    int i=0;
    if(strcmp(type,"dir")==0){
        p1 = strstr(record,"@dchild:");
        p2 = strstr(record,"@fchild:");
        strcpy(temp,p2);
        result = p1;
        
        while(*p1!=':'){
            p1++;
        }
        p1++;
        if(*p1 == '\n'){
            sprintf(p1,"%d\n%s",cid,temp); 
        }
        else{
            while(*p1!='\n'){
                p1++;
            }
            *p1 = ',';
            p1++;
            //append new child
            sprintf(p1,"%d\n%s",cid,temp); 
        }
        
        printf("child:[%s]\n",result);
        write(fp,record,sizeof(char)*RECORDLEN);
    }
    else{
        p = strstr(record,"@fchild:");
        result = p;
        while(*p!=':'){
            p++;
        }
        p++;
        if(*p == '\n'){
            sprintf(p,"%d\n",cid); 
        }
        else{
            while(*p!='\n'){
                p++;
            }
            *p = ',';
            p++;
            //append new child
            sprintf(p,"%d\n",cid); 
        }
        printf("child:[%s]\n",result);
        write(fp,record,sizeof(char)*RECORDLEN);
    }
    return ;
}

int deleteChild(int cid,char *record,char *type,int fp){
    char *p,*p1,*p2,*result;
    char temp[RECORDLEN],temp2[RECORDLEN];
    char temp_child[RECORDLEN];
    char **did,**fid;

    char *delim=",@:";

    int i=0,mark=0,deli=0,totali=-1;;
    if(strcmp(type,"dir")==0){
        p1 = strstr(record,"@dchild:");
        p2 = strstr(record,"@fchild:");
        strcpy(temp,p2);
        result = p1;
        
        while(*p1!=':'){
            p1++;
        }
        p1++;
        if(*p1 == '\n'){//no dir
            //sprintf(p1,"%d\n%s",cid,temp);
            return -1;
        }
        else{
            strcpy(temp2,p1);

            did =(char**)malloc(sizeof(char*)*IDNUM);
            did[i] = strtok(temp2,delim);
            if(strcmp(did[i],"fchild")==0){
                    printf("none,none,none</br>");
                    return 0;
            }

            if(atoi(did[i])==cid){
                i--;
                mark=1;
            }
            else{
                printf("did:[%s]",did[i]);
                strcat(temp_child,did[i]);
            }

            i++;
            totali++;
            while((did[i]=strtok(NULL,delim))!=NULL){
                if(strcmp(did[i],"fchild")==0){
                    printf("none,none,none</br>");
                    break;
                }
                    if(atoi(did[i])==cid){
                            deli = i;
                            i--;

                    }
                    else{
                        printf("[%s]",did[i]);
                        if(mark==0){
                            strcat(temp_child,",");
                            strcat(temp_child,did[i]);
                        }
                        else{ 
                            strcat(temp_child,did[i]);
                            mark=0;   
                        }
                    }
                    i++;
                    totali++;
            }
            if(totali==deli){
                sprintf(p1,"%s\n%s",temp_child,temp); 
            }
            else{ 
                sprintf(p1,"%s%s",temp_child,temp); 
            }
        }
        
        printf("total i is [%d],deli is [%d]\nfinal child:[%s]\n",totali,deli,result);
        write(fp,record,sizeof(char)*RECORDLEN);
    }
    else{
        p = strstr(record,"@fchild:");
        result = p;
        while(*p!=':'){
            p++;
        }
        p++;
        if(*p == '\n'){//nothing file
                return -1;
        }
        else{
            strcpy(temp,p);
            fid =(char**)malloc(sizeof(char*)*IDNUM);
            fid[i] = strtok(temp,delim);

            if(atoi(fid[i])==cid){
                i--;
                mark=1;
            }
            else{
                printf("id:[%s]",fid[i]);
                strcat(temp_child,fid[i]);
                //sprintf(temp_child,"%s",fid[i]); 
            }
            i++;
            totali++;
            while((fid[i]=strtok(NULL,delim))!=NULL){
                    if(atoi(fid[i])==cid){
                            deli = i;
                            i--;
                    }
                    else{
                        printf("[%s]",fid[i]);
                        if(mark==0){
                            strcat(temp_child,",");
                            strcat(temp_child,fid[i]);
                        }
                        else{ 
                            strcat(temp_child,fid[i]);
                            mark=0;   
                        }
                        //sprintf(temp_child,"%s,%s",temp_child,fid[i]); 
                    }
                    i++;
                    totali++;
            }
            if(totali==deli){
                sprintf(p,"%s\n",temp_child); 
            }
            else{
                sprintf(p,"%s",temp_child); 
            }
        }

        printf("final child:[%s]\n",result);
        //printf("final child:[%s]\n",temp_child);
        write(fp,record,sizeof(char)*RECORDLEN);
    }
    return 0;

}
char *rdb_read(int rid,char *column,char *type){
    int ds_file,i=0;
    int name,offset,size;
    char temp[RECORDLEN];
    char *content;
    char **did,**fid;
    char *delim1="@";
    char *delim2=",\n";
    char *data;

    //list dir's all childs
    if(strcmp(type,"dir")==0){
        //parse column to get data(id or description)
        if(strcmp(column,"child")==0){//list all childs
                data = getRecord(rid,"dir");
                getColumn(data,"@dchild","dir");
                getColumn(data,"@fchild","file");

        }
        else if(strcmp(column,"@dchild")==0){
                data = getRecord(rid,"dir");
                getColumn(data,"@dchild","dir");
        }
        else{//list childs's total columns
                data = getRecord(rid,type);
                content = getColumn(data,column,type);
                //if(content==NULL){break;}
                //print(rid,type,content);
        }
        //get record with the id
    }
    else if(strcmp(type,"file")==0){
                data = getRecord(rid,type);
                content = getColumn(data,column,type);
                //print(rid,type,content);
    }
    
    free(data);
    return content;
}
char *getRecord(int rid,char *type){
        int offset;
        char *data;
        int ds_file;
        ds_file = open(dfile_path,O_RDONLY);//dfile
        if(strcmp(type,"dir")==0){
            offset = dir_list[rid].offset;//get gais offset
        }
        else{
            offset = file_list[rid].offset;//get gais offset
        }
        lseek(ds_file,offset,SEEK_SET);
        data = malloc(sizeof(char)*RECORDLEN);
        read(ds_file,data,sizeof(char)*(RECORDLEN));
        close(ds_file);
        return data;
}
void print(int rid,char *type,char *data,char *outputcolumn,int sort,char *sortby){
    char filename[FILENAMELENS];
    char *temp,*temp_outputcolumn,*result;
    char *delim = "(),";
    char **columns;
    int COLUMN_NUM=20;
    int i=0,j=0;
    //printf("outputcolumn:%s,type:%s,sort:%d,sortby:%s\n",outputcolumn,type,sort,sortby);
    //LIST command
    if(strcmp(outputcolumn,"")==0){
            if(strcmp(type,"dir")==0){
                    printf("%d,%s,%s<nl>",rid,dir_list[rid].filename,data);
            }
            else{
                    printf("%d,%s,%s<nl>",rid,file_list[rid].filename,data);
            }
            return ;
    }
    //FIND command

    //sortby
    if(sort==1){
            if(strcmp(type,"dir")==0||strcmp(type,"all")==0){
                strcpy(orderby[sortid].filename,dir_list[rid].filename);
            }
            else if(strcmp(type,"file")==0||strcmp(type,"all")==0){
                strcpy(orderby[sortid].filename,file_list[rid].filename);
            }
            orderby[sortid].rid = rid;
            temp = getColumn(data,"@type",type);
            strcpy(orderby[sortid].type,temp);
            temp = getColumn(data,"@ctime",type);
            strcpy(orderby[sortid].date,temp);
            temp = getColumn(data,"@size",type);
            orderby[sortid].size = atoi(temp);
            temp = getColumn(data,"@ds",type);
            strcpy(orderby[sortid].ds,temp);
            temp = getColumn(data,"@tag",type);
            strcpy(orderby[sortid].tag,temp);
            //printf("id:%d[%d]%s\n",sortid,orderby[sortid].size,orderby[sortid].filename);
    }
    //no sort:default->sort by offset
    else{
            columns = (char **)malloc(sizeof(char *)*COLUMN_NUM);
            temp_outputcolumn = malloc(sizeof(char)*strlen(outputcolumn));
            strcpy(temp_outputcolumn,outputcolumn);
            columns[i] = strtok(temp_outputcolumn,delim);
            //printf("outputcolumn[%d]:[%s]\n",i,columns[i]);
            i++;
            while((columns[i] = strtok(NULL,delim))!=NULL){
                    //printf("outputcolumn[%d]:[%s]\n",i,columns[i]);
                    i++;
            }

            result = malloc(sizeof(char)*(COLUMN_NUM)*500);
            for(j=0;j<i;j++){
                    temp = getColumn(data,columns[j],type);
                    if(temp==NULL){continue;}
                    //printf("[%s]:[%s]\n",columns[j],temp);
                    if(strcmp(columns[j],"@all")==0&&j==0){
                            sprintf(result,"%s",temp);
                            break;
                    }
                    else{
                        if(j!=0){
                                sprintf(result,"%s%s:%s",result,columns[j],temp);
                        }
                        else{ 
                                sprintf(result,"%s:%s",columns[j],temp);
                        }
                        //
                        /*
                        if(strcmp(columns[j],"@type")==0){
                            strcpy(orderby[sortid].type,temp);
                        }
                        else if(strcmp(columns[j],"@ctime")==0){
                            strcpy(orderby[sortid].date,temp);
                        }
                        else if(strcmp(columns[j],"@size")==0){
                            orderby[sortid].size = atoi(temp);
                        }
                        else if(strcmp(columns[j],"@ds")==0){
                            strcpy(orderby[sortid].ds,temp);
                        }
                        else if(strcmp(columns[j],"@tag")==0){
                            strcpy(orderby[sortid].tag,temp);
                        }
                        */
                }
            }
            //printf("result:\n[%s]",result);
            if(strcmp(type,"dir")==0){
                    /*
                       if(strcmp(outputcolumn,"@all")==0){
                       printf("%d,%s,%s<nl>",rid,dir_list[rid].filename,data);
                       }
                     */
                    printf("%d,%s,%s\n",rid,dir_list[rid].filename,result);
            }
            else{
                    /*
                       if(strcmp(outputcolumn,"@all")==0){
                       printf("%d,%s,%s<nl>",rid,file_list[rid].filename,data);
                       }
                     */
                    printf("%d,%s,%s\n",rid,file_list[rid].filename,result);
            }
            free(result);
            free(temp_outputcolumn);
            free(columns);
    }
    return ;
}
char *getColumn(char *record,char *column,char *type){
        char temp[RECORDLEN];
        char *childs,*fchild,*dchild;
        char **did,**fid;
        char *delim1="@";
        char *delim2=",\n";
        char *data;
        int i=0;
        if(strcmp(column,"@all")==0){
                return record;
        }

        childs = strstr(record,column);
        if(childs==0){return childs;};

        strcpy(temp,childs);
        //parse childs id
        dchild = malloc(sizeof(char)*RECORDLEN);
        dchild = strtok(temp,delim1);

        did =(char**)malloc(sizeof(char*)*IDNUM);
        //get dir id
        while(*dchild!=':'){dchild++;}
        dchild++;

        if(*dchild!='\n'){
                if(strcmp(column,"@fchild")==0||strcmp(column,"@dchild")==0){
                        did[i] = strtok(dchild,delim2);
                        //printf("did:[%s]",did[i]);
                        data = getRecord(atoi(did[i]),type);
                        print(atoi(did[i]),type,data,"",0,"");
                        i++;
                        while((did[i] = strtok(NULL,delim2))!=NULL){
                                //printf("!!!test!!!\n");
                                //return ;
                                if(strcmp(did[i],"\n")==0){break;}
                                //printf("[%s]\n",did[i]);
                                data = getRecord(atoi(did[i]),type);
                                print(atoi(did[i]),type,data,"",0,"");
                                i++;

                        }
                }
                else{
                    //printf("record:%s,column:%s,type:%s\n",record,column,type);
                    //printf("dchild:%s\n",dchild);
                    return dchild;
                }
        }
        else{
            if(strcmp(column,"@fchild")==0||strcmp(column,"@dchild")==0){
            }
            else{
                return dchild;
            }

        }
        return 0;
        
}
int rdb_find(char *pattern,char *type,char *sensitive,char *offset,char *sortby,char *range,char *outputnum,char *outputcolumn,int total){
        int rid,i=1,j=0,k=0,l;
        int from=atoi(offset),to=atoi(outputnum);
        //printf("[%s]from:%d,to:%d\n",type,from ,to);
        //printf("id:%d,dir_id:%d\n",id,dir_id);
        //printf("pattern:%s\n",pattern);
        int count=0,equal=0;
        int columns=10;
        char *data,*content;
        char *delim=";";
        char *delim2=":";
        char *delim3="(),";
        char *delim4="|!";
        char **column,**temp;
        char *pattern1;
        
        char *delim5 = " ";//Sun Jun 21 //19:02:16 2015
        char **date,**date1;
        int date_c=0;
        int temp_date_c=0;
        char *delim6 = "~=";
        int size1=0,size2=0,size=0;
        char date_temp[10];
        int year=0,month=0,day=0,year1=0,month1=0,day1=0,pyear=0,pmonth=0,pday=0;
        pattern1 = malloc(sizeof(char)*SLEN);

        strcpy(pattern1,pattern);
        //printf("copy:%s\n",pattern1);
        Query array[100];
        //parse pattern
        column = (char **)malloc(sizeof(char*)*columns);

        column[count] = strtok(pattern1,delim);
        //strcpy(array[count].column,strtok(column[count],delim2));
        //strcpy(array[count].pattern,strtok(NULL,delim2));
        //printf("column[%d]:%s\n",count,column[count]);
        //printf("[%d]%s:%s\n",count,array[count].column,array[count].pattern);
        count++;

        while((column[count] = strtok(NULL,delim))!=NULL){
            if(column[count][0]=='@'){
                //printf("column[%d]:%s\n",count,column[count]);
                //printf("[%d]%s:%s\n",count,array[count].column,array[count].pattern);
                count++;
                if(count>100){
                    printf("Query too long.");
                    return 0;
                 }
            }
        }

        i=0;
        while(i<count){//get column name and its pattern
                strcpy(array[i].column,strtok(column[i],delim2));
                strcpy(array[i].pattern,strtok(NULL,delim2));
                //printf("[%d]%s:%s\n",i,array[i].column,array[i].pattern);
                i++;
        }

        i=0;
        while(i<count){//cut column's pattern to small pattern
                array[i].filter = (char **)malloc(sizeof(char*)*100);
                k=0;
                /*parse column's pattern*/
                array[i].filter[k] = strtok(array[i].pattern,delim3);
                //printf("column[%d]:%s,filter[%d]:%s\n",i,array[i].column,k,array[i].filter[k]);
                k++;
                while((array[i].filter[k] = strtok(NULL,delim3))!=NULL){
                        //printf("column[%d]:%s,filter[%d]:%s\n",i,array[i].column,k,array[i].filter[k]);
                        k++;
                }
                array[i].count = k;
            i++;
        }
        //return;
        //get records's column
        if(strcmp(type,"file")==0){
            i=from;
            //total=0;
            //while(file_list[i].key!=-1){
            while(i<id){
                    if(file_list[i].key==-1){i++;continue;}
                    equal=0;
                    if(total==to){break;}
                    for(j=0;j<count;j++){//column name check
                            /*compare @filename*/
                            if(strcmp(array[j].column,"@filename")==0){
                                    for(l=0;l<array[j].count;l++){//compare multi patterns of column
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(file_list[i].filename,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                            }
                                                            else if(strstr(file_list[i].filename,temp[0])>0){
                                                            }
                                                            free(temp);
                                                            break;
                                                    case '!'://not
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(file_list[i].filename,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        equal=-1;
                                                                    }
                                                            }
                                                            else if(strstr(file_list[i].filename,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
                                                    default ://and
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    //printf("compare:%s,%s\n",file_list[i].filename,array[j].filter[l]);
                                                                    if(SensitiveCompare(file_list[i].filename,array[j].filter[l])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else{
                                                                    if(strstr(file_list[i].filename,array[j].filter[l])>0){
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                            
                                                            break;
                                            }

                                    }
                                    
                                    if(equal==0){
                                            data = getRecord(i,type);
                                            //print(i,type,data);
                                    }
                            }
                            else{//except @filename
                                data = getRecord(i,type);
                                if(strcmp(array[j].column,"@all")==0){//search all column
                                    for(l=0;l<array[j].count;l++){
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(data,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                            }
                                                            else if(strstr(data,temp[0])>0){
                                                            }
                                                            free(temp);
                                                            break;
                                                            //@break
                                                    case '!'://not
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(data,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        //return -1;
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else if(strstr(data,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
                                                            //@break1
                                                    default ://and
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    //printf("compare:%s\n",array[j].filter[l]);
                                                                    if(SensitiveCompare(data,array[j].filter[l])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else{
                                                                if(strstr(data,array[j].filter[l])>0){
                                                                }
                                                                else{
                                                                    equal=-1;
                                                                }
                                                            }
                                            
                                                            break;
                                            }
                                            //@break2
                                    }
                                    
                                }
                                else if(strcmp(array[j].column,"@ctime")==0){//search all column
                                    pyear=0;pmonth=0;pday=0;
                                    strcpy(date_temp,array[j].filter[0]);
                                    temp_date_c=0;
                                    date_c=0;
                                    date = (char **)malloc(sizeof(char *)*6);
                                    date1 = (char **)malloc(sizeof(char *)*6);
                                    content = getColumn(data,array[j].column,type);
                                    sscanf(content,"%4d %d %2d",&pyear,&pmonth,&pday);
                                    //printf("pyear:%d,pmonth:%d,pday:%d\n",pyear,pmonth,pday);
                                    
                                    //if(content==NULL){equal=-1;break;}

                                    year=0;month=0;day=0;
                                    year1=0;month1=0;day1=0;

                                    date_c=0;
                                    date1[date_c] = strtok(date_temp,delim6);
                                    sscanf(date1[date_c],"%4d%2d%2d",&year,&month,&day);
                                    date_c++;
                                    if((date1[date_c] = strtok(NULL,delim6))!=NULL){
                                        sscanf(date1[date_c],"%4d%2d%2d",&year1,&month1,&day1);
                                    }

                                    if(year==0){
                                            break;
                                    }
                                    if(month==0){
                                            month=pmonth;
                                    }
                                    if(day==0){
                                            day=pday;
                                    }
                                    if(month1==0){
                                            month1=20;
                                    }
                                    if(day1==0){
                                            day1=40;
                                    }
                                    //printf("year:%d,month:%d,day:%d\n->year:%d,month:%d,day:%d\n",year,month,day,year1,month1,day1);
                                    if(year1==0){
                                        if(pyear!=year||pmonth!=month||pday!=day){
                                            equal=-1;
                                        }
                                    }
                                    else{
                                        //year month day < pyear pmonth pday < year1 month1 day1
                                        if(pyear>year1||pyear<year||pmonth>month1||pmonth<month||pday>day1||pday<day){
                                            equal=-1;
                                        }
                                    }
                                    free(date1);
                                    free(date);
                                }
                                else if(strcmp(array[j].column,"@size")==0){//search all column

                                    content = getColumn(data,array[j].column,type);
                                    if(content==NULL){equal=-1;break;}
                                    size = atoi(content);
                                    for(l=0;l<array[j].count;l++){
                                            switch(array[j].filter[l][0]){
                                                case '~':
                                                        size1 = atoi(strtok(array[j].filter[l],delim6));
                                                        //printf("smaller than size:%d\n",size1);
                                                        if(size>=size1){
                                                                equal=-1;
                                                        }
                                                        break;
                                                case '=':
                                                        size1 = atoi(strtok(array[j].filter[l],delim6));
                                                        //printf("equal size:%d\n",size1);
                                                        if(size!=size1){
                                                                equal=-1;
                                                        }
                                                        break;
                                                default:
                                                        sscanf(array[j].filter[l],"%d~%d",&size1,&size2);
                                                        if(size2==0){
                                                            //printf("bigger than size:%d\n",size1);
                                                            if(size<=size1){
                                                                equal=-1;
                                                            }
                                                        }
                                                        else{
                                                            //printf("during size1:%d~size2:%d\n",size1,size2);
                                                            if(size<size1||size>size2){
                                                                equal=-1;
                                                            }
                                                        }

                                            }
                                    }
                                }
                                else{//except @all,@size,@ctime column
                                    //printf("column:%s\n",array[j].column);
                                    content = getColumn(data,array[j].column,type);
                                    //puts("--FIN--");
                                    //printf("content:%s\n",content);
                                    if(content==NULL){equal=-1;break;}
                                    for(l=0;l<array[j].count;l++){
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(content,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                            }
                                                            else if(strstr(content,temp[0])>0){
                                                            }
                                                            free(temp);
                                                            break;
                                                    case '!'://not
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(content,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        //return -1;
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else if(strstr(content,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
                                                    default ://and
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(content,array[j].filter[l])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else{
                                                                    if(strstr(content,array[j].filter[l])>0){
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                            
                                                            break;
                                            }

                                    }
                                }
                            }
                    }
                    if(equal==0){
                            if(strcmp(sortby,"offset")==0){//sortby offset
                                print(i,type,data,outputcolumn,0,"");
                            }
                            else{//other sort
                                print(i,type,data,outputcolumn,1,sortby);
                                //printf("[%d]%s\n",orderby[sortid].size,orderby[sortid].filename);
                                sortid++;
                            }
                            total++;
                    }
                    i++;
            }

            if(total==0){
                    //printf("none,none,none<nl>");
            }
            else{
                   //printf("total:%d\n",total);   
            }

        }
        else if(strcmp(type,"dir")==0){
            i=from;
            //total=0;
            //while(dir_list[i].key!=-1){
            while(i<dir_id){
                    if(dir_list[i].key==-1){i++;continue;}
                    if(total==to){break;}
                    equal=0;
                    for(j=0;j<count;j++){//column name check
                            /*compare @filename*/
                            if(strcmp(array[j].column,"@filename")==0){
                                    for(l=0;l<array[j].count;l++){//compare multi patterns of column
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(dir_list[i].filename,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                            }
                                                            else if(strstr(dir_list[i].filename,temp[0])>0){
                                                            }
                                                            free(temp);
                                                            break;
                                                    case '!'://not
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(dir_list[i].filename,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        //return -1;
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else if(strstr(dir_list[i].filename,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
                                                    default ://and
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(dir_list[i].filename,array[j].filter[l])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else{
                                                                    if(strstr(dir_list[i].filename,array[j].filter[l])>0){
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                            
                                                            break;
                                            }

                                    }
                                    
                                    if(equal==0){
                                            data = getRecord(i,type);
                                            //print(i,type,data);
                                    }
                            }
                            else{//except @filename
                                data = getRecord(i,type);
                                if(strcmp(array[j].column,"@all")==0){//search all column
                                    for(l=0;l<array[j].count;l++){
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(data,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                            }
                                                            else if(strstr(data,temp[0])>0){
                                                            }
                                                            free(temp);
                                                            break;
                                                            //@break
                                                    case '!'://not
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(data,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        //return -1;
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else if(strstr(data,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
                                                            //@break1
                                                    default ://and
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(data,array[j].filter[l])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else{
                                                                if(strstr(data,array[j].filter[l])>0){
                                                                }
                                                                else{
                                                                    equal=-1;
                                                                }
                                                            }
                                            
                                                            break;
                                            }
                                            //@break2
                                    }
                                    
                                }
                                else if(strcmp(array[j].column,"@ctime")==0){//search all column
                                    pyear=0;pmonth=0;pday=0;
                                    strcpy(date_temp,array[j].filter[0]);
                                    temp_date_c=0;
                                    date_c=0;
                                    date = (char **)malloc(sizeof(char *)*6);
                                    date1 = (char **)malloc(sizeof(char *)*6);
                                    content = getColumn(data,array[j].column,type);
                                    sscanf(content,"%4d %d %2d",&pyear,&pmonth,&pday);
                                    //printf("pyear:%d,pmonth:%d,pday:%d\n",pyear,pmonth,pday);
                                    
                                    //if(content==NULL){equal=-1;break;}

                                    year=0;month=0;day=0;
                                    year1=0;month1=0;day1=0;

                                    date_c=0;
                                    date1[date_c] = strtok(date_temp,delim6);
                                    sscanf(date1[date_c],"%4d%2d%2d",&year,&month,&day);
                                    date_c++;
                                    if((date1[date_c] = strtok(NULL,delim6))!=NULL){
                                        sscanf(date1[date_c],"%4d%2d%2d",&year1,&month1,&day1);
                                    }

                                    if(year==0){
                                            break;
                                    }
                                    if(month==0){
                                            month=pmonth;
                                    }
                                    if(day==0){
                                            day=pday;
                                    }
                                    if(month1==0){
                                            month1=20;
                                    }
                                    if(day1==0){
                                            day1=40;
                                    }
                                    //printf("year:%d,month:%d,day:%d\n->year:%d,month:%d,day:%d\n",year,month,day,year1,month1,day1);
                                    if(year1==0){
                                        if(pyear!=year||pmonth!=month||pday!=day){
                                            equal=-1;
                                        }
                                    }
                                    else{
                                        //year month day < pyear pmonth pday < year1 month1 day1
                                        if(pyear>year1||pyear<year||pmonth>month1||pmonth<month||pday>day1||pday<day){
                                            equal=-1;
                                        }
                                    }
                                    free(date1);
                                    free(date);
                                }
                                else{//except @all column
                                    content = getColumn(data,array[j].column,type);
                                    if(content==NULL){equal=-1;break;}
                                    for(l=0;l<array[j].count;l++){
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(content,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                            }
                                                            else if(strstr(content,temp[0])>0){
                                                            }
                                                            free(temp);
                                                            break;
                                                    case '!'://not
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(content,temp[0])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        //return -1;
                                                                        equal=-1;
                                                                    }
                                                            }
                                                            else if(strstr(content,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
                                                    default ://and
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(content,array[j].filter[l])==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                                            else{
                                                                    if(strstr(content,array[j].filter[l])>0){
                                                                    }
                                                                    else{
                                                                            equal=-1;
                                                                    }
                                                            }
                                            
                                                            break;
                                            }

                                    }
                                }
                            }
                    }
                    if(equal==0){
                            if(strcmp(sortby,"offset")==0){
                                print(i,type,data,outputcolumn,0,"");
                            }
                            else{
                                print(i,type,data,outputcolumn,1,sortby);
                                //printf("[%d]%s\n",orderby[sortid].size,orderby[sortid].filename);
                                sortid++;
                            }
                            total++;
                    }
                    i++;
            }

            if(total==0){
                    //printf("none,none,none<nl>");
            }
            else{
                   //printf("total:%d\n",total);   
            }

        }
    free(column);
    free(pattern1);
    return total;
}

int SortByColumn(char *sortby,char *outputcolumn){
        int i,count;
        int total = sortid;
        char *record;
        record = malloc(sizeof(char)*RECORDLEN);
        if(strcmp(sortby,"size")==0){
                qsort(orderby,total,sizeof(Sort),compare_size);
        }
        else if(strcmp(sortby,"filename")==0){
                qsort(orderby,total,sizeof(Sort),compare_name);
        }
        else if(strcmp(sortby,"type")==0){
                qsort(orderby,total,sizeof(Sort),compare_type);
        }
        else if(strcmp(sortby,"date")==0){
                qsort(orderby,total,sizeof(Sort),compare_date);
        }

        //printf("sortid:%d\n",total);
        
        for(i=0;i<total;i++){
                record = malloc(sizeof(char)*RECORDLEN);
                sprintf(record,"@\n@type:%s@ctime:%s@size:%d\n@ds:%s@tag:%s",orderby[i].type,orderby[i].date,orderby[i].size,orderby[i].ds,orderby[i].tag);
                //sprintf(record,"@\n@type:%s@ctime:%s@size:%d\n@ds:%s@tag:%s@path:%d\n@dchild:%s\n@fchild:%s\n",orderby[i].type,orderby[i].date,orderby[i].size,orderby[i].ds,orderby[i].tag,0,orderby[i].dchild,orderby[i].fchild);
                //printf("rid:%d [%d]%s->\n%s\n------\n",orderby[i].rid,orderby[i].size,orderby[i].filename,record);
                if((orderby[i].type,"dir")==0){
                    print(orderby[i].rid,"dir",record,outputcolumn,0,sortby);
                }
                else{
                    print(orderby[i].rid,"file",record,outputcolumn,0,sortby);
                }
                free(record);
        }
        
    return 0;
}
int compare_size(const void *a, const void *b){
    Sort *a1 = (Sort *)(a);
    Sort *b1 = (Sort *)(b);
    return b1->size - a1->size; 
}
int compare_name(const void *a, const void *b){
    Sort *a1 = (Sort *)(a);
    Sort *b1 = (Sort *)(b);
    return strcmp(b1->filename,a1->filename);
}
int compare_type(const void *a, const void *b){
    Sort *a1 = (Sort *)(a);
    Sort *b1 = (Sort *)(b);
    return strcmp(b1->type,a1->type);
}
int compare_date(const void *a, const void *b){
    Sort *a1 = (Sort *)(a);
    Sort *b1 = (Sort *)(b);
    return strcmp(b1->date,a1->date);
}
/*
int find_file(int count,Query *a1,char *sensitive){
        
}
*/
/*
int find_dir(){

}
*/
int rdb_update(int cid,int fp,char *type,int command,char *column,char *newdata){
    char *record;
    char *p1,*p2,*temp1,temp2[1000];
    int offset;
    record = malloc(sizeof(char)*RECORDLEN);
    if(strcmp(type,"dir")==0){
        offset = dir_list[cid].offset;
    }
    else{ 
        offset = file_list[cid].offset;
    }
    lseek(fp,offset,SEEK_SET);
    read(fp,record,sizeof(char)*RECORDLEN);
    lseek(fp,offset,SEEK_SET);//reset dir offset for updating new record
    
    //printf("cid:%d,type:%s,column:%s\nrecord:%s\nnewdata:%s\n",cid,type,column,record,newdata);
    
    p1 = strstr(record,column);
    temp1 = p1;
    temp1++;
    p2 = strstr(temp1,"@");
    strcpy(temp2,p2);
    
    //printf("p1:%s\np2:%s\n",p1,p2);
    
    while(*temp1!=':'){temp1++;}
    temp1++;
    sprintf(temp1,"%s\n%s",newdata,temp2);
    //printf("result:%s",record);
    write(fp,record,sizeof(char)*RECORDLEN);
    
    return 0;
}
int rdb_delete(){
}
int SensitiveCompare(char *s1, char *s2){
        int i=0;
        int l1,l2;
        char *p;
        //printf("----compare--\ns1:%s,s2:%s\n-----\n",s1,s2);
        char p1[100],p2[100];
        //char *p1,*p2;
        /*
        l1 = strlen(s1);
        l2 = strlen(s2);
        p1 = malloc(sizeof(char)*l1);
        p2 = malloc(sizeof(char)*l2);
        */
        strcpy(p1,s1);
        strcpy(p2,s2);
        for(i=0;i<l1;i++){
            p1[i] = tolower(p1[i]);
        }
        p1[i]='\0';
        for(i=0;i<l2;i++){
            p2[i] = tolower(p2[i]);
        }
        p2[i]='\0';
        //printf("p1:%s,p2:%s\n",p1,p2);
        p=strstr(p1,p2);
        if(p!=0){
            return 0;
        }
        else{
            return -1;
        }
}
