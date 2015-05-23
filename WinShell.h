#define BUFSIZE MAX_PATH
#define HISNUM 12                   /*可以最多保存12个历史命令*/
char buf[BUFSIZE];

/*保存历史命令的结构体*/
typedef struct ENV_HISTROY{
	int start;
	int end;
	char his_cmd[HISNUM][100];
}ENV_HISTORY;
ENV_HISTORY envhis;

/*保存文件或目录相关信息*/
typedef struct files_Content                   
{  
	FILETIME time;
	char name[200];
	int type;
	int size;
	struct files_Content *next;
}files_Content;