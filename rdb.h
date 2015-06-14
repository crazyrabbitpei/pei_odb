#define FILENAMELENS 50
#define BUCKETNUMBER 1000000
#define DBININUM 2
#define READPER  1073741824//1 GB
#define DATASIZE  1073741824//1 GB
#define RECORDLEN 40000
#define IDNUM 1000000

typedef enum{
    PUT,
    GET,
    LIST,
    DETAIL,
    DEL,
    DELD,
    RENAME,
    RENAMED,
    FIND,
    CDIR

}command;

//rdb
extern char id_record_file[100];
extern char id_record_dir[100];
extern char dfile_map_path[100];
extern char dfile_path[100];
extern int index_file,map_file,ini_file,name_file,id_file,id_dir;


typedef struct{
    /*dir_map(record file/dir description's position in dfile file)*/
    char filename[FILENAMELENS];
    int offset;
    int key;
}dir_map;
typedef struct{
        /* file id  map to hv(index file) */
        unsigned long int key;//odb:object key
            char filename[FILENAMELENS];
                int offset;//rdb:gais record offset

} map;
extern dir_map dir_list[BUCKETNUMBER];
extern map file_list[BUCKETNUMBER];

int CheckFile(int fd,char *filename);
int StoreGais(char *name,char *type,int len,char *date,unsigned long int key,int fp,int dir_path);
int CreateDir(char *name,int dir_path);
int rdb_create(int fp,char *name,int offset, int size,int parent,char *type);
int rdb_read(int rid,char *column,char *type);
void getDir(int cid,int pid,int fp,char *type,int command);
unsigned long int Gethv(unsigned char *data,unsigned long int size);
int GetOffset(int file);
