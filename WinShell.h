#define BUFSIZE MAX_PATH
#define HISNUM 12                   /*������ౣ��12����ʷ����*/
char buf[BUFSIZE];

/*������ʷ����Ľṹ��*/
typedef struct ENV_HISTROY{
	int start;
	int end;
	char his_cmd[HISNUM][100];
}ENV_HISTORY;
ENV_HISTORY envhis;

/*�����ļ���Ŀ¼�����Ϣ*/
typedef struct files_Content                   
{  
	FILETIME time;
	char name[200];
	int type;
	int size;
	struct files_Content *next;
}files_Content;