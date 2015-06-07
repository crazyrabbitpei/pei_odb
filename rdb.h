#define FILENAMELENS 100
#define BUCKETNUMBER 1000000
#define DBININUM 2
#define READPER  1073741824//1 GB
#define DATASIZE  1073741824//1 GB
//rdb
extern char id_record[100];
extern char dfile_map_path[100];
extern char dfile_path[100];
extern int index_file,map_file,ini_file,name_file,id_file;

typedef struct{
    /*dfile_map(record file/dir description's position in dfile file)*/
    char filename[FILENAMELENS];
    unsigned long int key;
    int offset;
    int size;
    //char date[100];
    //char type[100];

}name;
int StoreGais(char *name,char *type,int len,char *date,unsigned long int key,int fp,char *dir_path);
int CreateDir(char *name,char *dir_path);
int rdb_create(char *name,int offset, int size,char *parent);
unsigned long int Gethv(unsigned char *data,unsigned long int size);
int GetOffset(int file);
