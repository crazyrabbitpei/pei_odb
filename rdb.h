#define FILENAMELENS 100
#define BUCKETNUMBER 1000000
#define DBININUM 2
#define READPER  1073741824//1 GB
#define DATASIZE  1073741824//1 GB
typedef struct{
    //char filename[FILENAMELENS];
    unsigned long int key;
    int offset;
    int size;
    //char date[100];
    //char type[100];

}name;
int StoreGais(char *name,char *type,int len,char *date,unsigned long int key,int fp);
int CreateDir(char *name);
unsigned long int Gethv(unsigned char *data,unsigned long int size);
int GetOffset(int file);
