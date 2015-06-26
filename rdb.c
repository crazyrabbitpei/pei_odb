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
int rdb_find(char *pattern,char *type,char *sensitive,char *offset,char *sortby,char *range,char *outputnum,char *outputcolumn);
//int find_file(int count,Query *a1,char *sensitive);
void appendChild(int cid,char *record,char *type,int fp);
int deleteChild(int cid,char *record,char *type,int fp);
char *getColumn(char *record,char *column,char *type);
char *getRecord(int rid,char *type);
void print(int rid,char *type,char *data);
int SensitiveCompare(char *s1, char *s2);
typedef struct{
        char column[10];
        char pattern[500];
        char **filter;
        int count;
}Query;

int StoreGais(char *name,char *type,int len,char *date,unsigned long int key,int fp,int rid){
    char *record;
    int size;
    record = malloc(sizeof(char)*RECORDLEN);
    //sprintf(record,"@\n@type:%s\n@ctime:%s@size:%d\n",type,date,len);
    sprintf(record,"@\n@type:%s\n@ctime:%s@size:%d\n@ds:\n@tag:\n@path:%d\n@dchild:\n@fchild:\n",type,date,len,rid);
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
    char* date;
    int des_file,des_offset,size;
    unsigned long int hv,index_map;
    time_t current_time;
    des_file = open(dfile_path,O_RDWR|O_CREAT,S_IRWXU|S_IRGRP);
    if(CheckFile(des_file,dfile_path)==1){
        return -1;
    }
    des_offset = GetOffset(des_file);

    current_time = time(NULL);
    date = ctime(&current_time);
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
void print(int rid,char *type,char *data){
    char filename[FILENAMELENS];
    if(strcmp(type,"dir")==0){
        printf("%d,%s,%s<nl>",rid,dir_list[rid].filename,data);
    }
    else{
        printf("%d,%s,%s<nl>",rid,file_list[rid].filename,data);
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
        if(strcmp(column,"all")==0){
                return record;
        }

        childs = strstr(record,column);
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
                        print(atoi(did[i]),type,data);
                        i++;
                        while((did[i] = strtok(NULL,delim2))!=NULL){
                                //printf("!!!test!!!\n");
                                //return ;
                                if(strcmp(did[i],"\n")==0){break;}
                                //printf("[%s]\n",did[i]);
                                data = getRecord(atoi(did[i]),type);
                                print(atoi(did[i]),type,data);
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
int rdb_find(char *pattern,char *type,char *sensitive,char *offset,char *sortby,char *range,char *outputnum,char *outputcolumn){
        int rid,i=1,j=0,k=0,l;
        int from=atoi(offset),to=atoi(outputnum);
        printf("from:%d,to:%d\n",from ,to);
        int count=0,total=0,equal=0;
        int columns=10;
        char *data,*content;
        char *delim=";";
        char *delim2=":";
        char *delim3="(),";
        char *delim4="|!";
        char **column,**temp;
        char *pattern1;
        pattern1 = malloc(sizeof(char)*(strlen(pattern)));
        strcpy(pattern1,pattern);
        printf("copy:%s\n",pattern1);
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
                //strcpy(array[count].column,strtok(column[count],delim2));
                //strcpy(array[count].pattern,strtok(NULL,delim2));
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
                printf("[%d]%s:%s\n",i,array[i].column,array[i].pattern);
                i++;
        }
        i=0;
        while(i<count){//cut column's pattern to small pattern
                array[i].filter = (char **)malloc(sizeof(char*)*100);
                k=0;
                /*parse column's pattern*/
                array[i].filter[k] = strtok(array[i].pattern,delim3);
                printf("column[%d]:%s,filter[%d]:%s\n",i,array[i].column,k,array[i].filter[k]);
                k++;
                while((array[i].filter[k] = strtok(NULL,delim3))!=NULL){
                        printf("column[%d]:%s,filter[%d]:%s\n",i,array[i].column,k,array[i].filter[k]);
                        k++;
                }
                array[i].count = k;
            i++;
        }
        //get records's column
        if(strcmp(type,"file")==0){
            i=from;
            total=0;
            while(file_list[i].key!=-1){
                    equal=0;
                    //if(total==to){break;}
                    for(j=0;j<count;j++){//column name check
                            /*compare @filename*/
                            if(strcmp(array[j].column,"@filename")==0){
                                    for(l=0;l<array[j].count;l++){//compare multi patterns of column
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(temp[0],file_list[i].filename)==0){
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
                                                                    if(SensitiveCompare(temp[0],file_list[i].filename)==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        return -1;
                                                                    }
                                                            }
                                                            else if(strstr(file_list[i].filename,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
                                                    default ://and
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    printf("compare:%s,%s\n",file_list[i].filename,array[j].filter[l]);
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
                                                                    if(SensitiveCompare(temp[0],data)==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                            }
                                                            else if(strstr(data,temp[0])>0){
                                                            }
                                                            free(temp);
                                                            break;
                                                    case '!'://not
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(temp[0],data)==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        return -1;
                                                                    }
                                                            }
                                                            else if(strstr(data,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
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

                                    }
                                    
                                }
                                else{//except @all column
                                    content = getColumn(data,array[j].column,type);
                                    for(l=0;l<array[j].count;l++){
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(temp[0],content)==0){
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
                                                                    if(SensitiveCompare(temp[0],content)==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        return -1;
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
                            print(i,type,data);
                            total++;
                    }
                    i++;
            }

            if(total==0){
                    printf("none,none,none<nl>");
            }
            else{
                   printf("%d<nl>",total);   
            }

        }
        else if(strcmp(type,"dir")==0){
            i=from;
            total=0;
            while(dir_list[i].key!=-1){
                    //if(total==to){break;}
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
                                                                    if(SensitiveCompare(temp[0],dir_list[i].filename)==0){
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
                                                                    if(SensitiveCompare(temp[0],dir_list[i].filename)==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        return -1;
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
                                                                    if(SensitiveCompare(temp[0],data)==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                    }
                                                            }
                                                            else if(strstr(data,temp[0])>0){
                                                            }
                                                            free(temp);
                                                            break;
                                                    case '!'://not
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(temp[0],data)==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        return -1;
                                                                    }
                                                            }
                                                            else if(strstr(data,temp[0])>0){
                                                                    equal=-1;
                                                            }
                                                            free(temp);
                                                            break;
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

                                    }
                                    
                                }
                                else{//except @all column
                                    content = getColumn(data,array[j].column,type);
                                    for(l=0;l<array[j].count;l++){
                                            switch(array[j].filter[l][0]){
                                                    case '|'://or
                                                            temp = (char **)malloc(sizeof(char*)*1);
                                                            temp[0] = strtok(array[j].filter[l],delim4);
                                                            if(strcmp(sensitive,"yes")==0){
                                                                    if(SensitiveCompare(temp[0],content)==0){
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
                                                                    if(SensitiveCompare(temp[0],content)==0){
                                                                        //printf("sensitiveCompare:0\n");
                                                                        return -1;
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
                            print(i,type,data);
                            total++;
                    }
                    i++;
            }

            if(total==0){
                    printf("none,none,none<nl>");
            }
            else{
                   printf("%d<nl>",total);   
            }

        }
        else{//search file and dir
        }
    free(column);
    free(pattern1);
    return 0;
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
        l1 = sizeof(s1)+1;
        l2 = sizeof(s2)+1;
        char *p1,*p2;
        p1 = malloc(sizeof(char)*l1);
        p2 = malloc(sizeof(char)*l2);
        for(i=0;i<l1+1;i++){
            p1[i] = tolower(s1[i]);
        }
        p1[i]='\0';
        for(i=0;i<l2+1;i++){
            p2[i] = tolower(s2[i]);
        }
        p2[i]='\0';
        printf("p1:%s,p2:%s\n",p1,p2);
        if(i=strstr(p1,p2)>0){
            //printf("i:%d\n",i);
            free(p1);
            free(p2);
            return 0;
        }
        else{
            free(p1);
            free(p2);
            return -1;
        }
}
