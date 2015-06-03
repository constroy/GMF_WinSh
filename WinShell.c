#define _WIN32_WINNT 0x0501         /*Windows API�汾*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>
#include <Wincon.h>
#include <tlhelp32.h>
#include <malloc.h>
#include <string.h>
#include <direct.h>
#include "WinShell.h"


          
main()
{
	/******************************�����������õ��ĺ���**************************************/
	BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam);
	HWND ReturnWnd(DWORD processID);
	BOOL fp(char *pid);
	void cd_cmd(char *dir);                     /*��ʾcd����*/
	void dir_cmd(char *dir);                    /*��ʾdir����*/
	void ftime(FILETIME filetime);              /*��ʾ�ļ�����ʱ��*/
	void GetProcessList();                      /*���ϵͳ��ǰ�����б�*/
	void history_cmd();                         /*���������������*/
	void add_history(char *inputcmd);           /*�������������ӵ�������ʷ��*/
	HANDLE process(int bg, char appName[]);                     /*��������*/
    BOOL killProcess(char *pid);                /*kill����*/
    BOOL WINAPI ConsoleHandler(DWORD CEvent);   /*�ص�����*/
	void help();                                /*��ʾ������Ϣ*/

	char c, *input, *arg[2], path[BUFSIZE];
	int input_len = 0, is_bg = 0, i, j, k;
	HANDLE hprocess;              /*����ִ�н��������ؽ��̾��*/
	DWORD dwRet;

	while(1)
	{    
		/*��ָ�����������ָ�������ʼ��*/
		for(i= 0; i < 2; i++)
			arg[i] = NULL;
		/*��õ�ǰĿ¼�����صĵ�ַ���롰path���У�BUFSIZE������ܹ�����ĵ�ַ����*/
		dwRet = GetCurrentDirectory(BUFSIZE, path);
		
		if( dwRet == 0 )
		{   
			/*���ص�ǰĿ¼ʧ�ܣ����������Ϣ*/
			printf("GetCurrentDirectory failed (%d)\n", GetLastError());
			
		}
		else if(dwRet > BUFSIZE)
		{
			/*BUFSIZE����С�ڷ��ص�ַ���ȣ����������ٳ���*/
			printf("GetCurrentDirectory failed (buffer too small; need %d chars)\n", dwRet);
			
		}
		else
			/*�����ǰĿ¼*/
			printf("%s>", path);
		
			
		/************************************����***********************************************/
		
		input_len = 0;
		/*�������ַ����˵�*/
		while((c = getchar()) ==  ' ' || c == '\t' || c == EOF)
			;	

		if(c == '\n')                   /*����Ϊ��ʱ��������ѭ����ӡ��ʾ��*/
			continue;
		while(c != '\n')
		{
			buf[input_len++] = c;
			c = getchar();
		}
		buf[input_len++] =  '\0';       /*���ϴ�������*/
		
		/*���䶯̬�洢�ռ䣬������ӻ��渴�Ƶ�input��*/
		input = (char*) malloc(sizeof(char)*(input_len));
		strcpy(input, buf);
		

		/************************************����ָ��********************************************/
		
		for (i = 0, j = 0, k = 0; i < input_len; i++){
			if (input[i] == ' ' || input[i] == '\0'){
				if (j == 0)
					continue;
				else
				{
					buf[j++] = '\0';
					arg[k] = (char*)malloc(sizeof(char)*j);
					strcpy(arg[k++], buf);
					j = 0;
				}
			}
			else
			{
				buf[j++] = input[i];
			}
		}
		

		/*********************************�ڲ������******************************************/
		/*cd����*/
		if (strcmp(arg[0], "cd") == 0){
			add_history(input);
			for (i = 3, j = 0; i <= input_len; i++)
				buf[j++] = input[i];
			buf[j] = '\0';
			arg[1] = (char*)malloc(sizeof(char)*j);
			strcpy(arg[1], buf);
			cd_cmd(arg[1]);
			free(input);
			continue;
		}
		
		/*dir����*/
		if (strcmp(arg[0], "dir") == 0){
			char *route;
			add_history(input);
			if (arg[1] == NULL){
				route = path;
				dir_cmd(route);
			}
			else
				dir_cmd(arg[1]);
			free(input);
			continue;
		}

		/*tasklist����*/
		if (strcmp(arg[0], "tasklist") == 0){
			add_history(input);
			GetProcessList();
			free(input);
			continue;
		}


		/*ǰ̨����*/
		if (strcmp(arg[0], "fp") == 0){
			add_history(input);
			if (arg[1]==NULL)
			{
				printf("û��ָ����ִ���ļ�\n");
				free(input);
				continue;
			}
			is_bg = 0;
			hprocess = process(is_bg,arg[1]);
			if (WaitForSingleObject(hprocess,INFINITE)==WAIT_OBJECT_0)
			/*�������ִ����ϣ��ͷſ���̨*/
			free(input);
			continue;
		}

		/*��̨����*/
		if (strcmp(arg[0], "bg&") == 0){
			add_history(input);
			if (arg[1] == NULL)
			{
				printf("û��ָ����ִ���ļ�\n");
				free(input);
				continue;
			}
			is_bg = 1;
			process(is_bg,arg[1]);
			free(input);
			continue;
		}

				/*��̨��ǰ̨����*/
		if (strcmp(arg[0], "fp&") == 0){
			BOOL success;
			int id;
			add_history(input);
			success = fp(arg[1]);
			id = atoi(arg[1]);
			hprocess =  OpenProcess(PROCESS_TERMINATE, FALSE, id);
			if (WaitForSingleObject(hprocess,INFINITE)==WAIT_OBJECT_0)
			/*�������ִ����ϣ��ͷſ���̨*/
			free(input);
			continue;
		}

		/*kill����*/
		if (strcmp(arg[0], "taskkill") == 0){
			BOOL success;
			add_history(input);
			success = killProcess(arg[1]);
			if (!success)
				printf("kill process failed!\n");
			free(input);
			continue;
		}

		/*��ʾ��ʷ����*/
		if (strcmp(arg[0], "history") == 0){
			add_history(input);
			history_cmd();
			free(input);
			continue;
		}

		/*exit����*/
		if (strcmp(arg[0], "exit") == 0){
			add_history(input);
			printf("Bye bye/n");
			free(input);
			break;
		}

		/*help����*/
		if (strcmp(arg[0], "help") == 0){
			add_history(input);
			help();
			free(input);
			continue;
		}
		else
		{
			printf("please type in correct command!\n");
			continue;
		}
	}	
}

/**********************************��������*******************************************/


/*********************************����������*****************************************/


/**********************************cd����**********************************************/

void cd_cmd(char *route)
{
	
	if( !SetCurrentDirectory(route))                /*���õ�ǰĿ¼��ʧ�ܷ��س�����Ϣ*/   
	{
		printf(TEXT("SetCurrentDirectory failed (%d)\n"), GetLastError());	
	}
}


/************************************dir����*********************************************/

void dir_cmd(char *route)
{
	
	WIN32_FIND_DATA FindFileData;                /*�����ҵ����ļ���Ŀ¼��WIN32_FIND_DATA�ṹ����*/
	files_Content head, *p, *q;	                 /*����ָ���ļ��ṹ���ָ��*/
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError;
	char volume_name[256];
	int file = 0, dir = 0;                             /*�ļ���Ŀ��Ŀ¼��Ŀ��ʼֵΪ0*/
	_int64 sum_file = 0;                            /*���ļ���С��Bytes��ʾֵ�ϴ󣬱���Ϊ64λ����*/
	_int64 l_user, l_sum, l_idle;                 /*���ÿռ䣬�����������ÿռ�*/
	unsigned long volume_number;
	char *DirSpec[3];
	
	DirSpec[0] = (char*)malloc(sizeof(char) * 2);	
	strncpy(DirSpec[0], route, 1);	
	*(DirSpec[0] + 1) = '\0';                      /*DirSpec[0]Ϊ��������*/
	DirSpec[1] = (char*)malloc(sizeof(char) * 4);
    strcpy(DirSpec[1], DirSpec[0]); 
	strncat(DirSpec[1], ":\\", 3);                  /*DirSpec[1]���ڻ����������Ϣ*/
    DirSpec[2] = (char*)malloc(sizeof(char) * (strlen(route) + 2));
    DirSpec[3] = (char*)malloc(sizeof(char) * (strlen(route) + 5));
	strcpy(DirSpec[2], route);
	strcpy(DirSpec[3], route);
	strncat(DirSpec[2], "\\", 2);	              /*DirSpec[2]Ϊdir�����Ŀ¼��*/
	strncat (DirSpec[3], "\\*.*", 5);             /*DirSpec[3]���ڲ���Ŀ¼�е��ļ�*/
	
	hFind = FindFirstFile(DirSpec[3], &FindFileData);	
	if (hFind == INVALID_HANDLE_VALUE)            /*���Ҿ������Ϊ��Чֵ������ʧ��*/
	{
		printf ("Invalid file handle. Error is %u\n", GetLastError());
		
	} 
	else 
	{   
		/*��ȡ��������Ϣ*/
		GetVolumeInformation(DirSpec[1], volume_name, 50, &volume_number, NULL, NULL, NULL, 10);
		
		if(strlen(volume_name) == 0)
			printf(" ������ %s �еľ�û�б�ǩ��\n", DirSpec[0]);
		else
			printf(" ������ %s �еľ��� %s \n", DirSpec[0], volume_name); 		
		printf(" ������к��� %X \n\n", volume_number);   
		printf("  %s ��Ŀ¼ \n\n", DirSpec[2]);		
		head.time = FindFileData.ftCreationTime;              /*����ļ�����ʱ�䣬�����ļ��ṹ��*/
        strcpy(head.name, FindFileData.cFileName);             /*����ļ����������ļ��ṹ��*/
		/*�������������Ŀ¼��typeλΪ0*/
		if( FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)					
		{
			head.type = 0;
			dir++;
		}
		else			
		{
			/*��������������ļ���typeλΪ1*/
			head.type = 1;
			head.size = FindFileData.nFileSizeLow;           /*���ļ���С����ṹ����*/
			file++;
			sum_file += FindFileData.nFileSizeLow;             /*���ļ���С�ۼ�*/
		}		
        p = &head;
		/*���������һ�����ݣ���������*/
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			
			q = (files_Content*)malloc(sizeof(files_Content));
			q->time = FindFileData.ftCreationTime;
			strcpy(q->name, FindFileData.cFileName);
			if( FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				q->type = 0;
				dir++;
			}
			else
			{
				q->type = 1;
				q->size = FindFileData.nFileSizeLow;
				file++;
				sum_file += FindFileData.nFileSizeLow;
			}			
			p->next = q;
			p = q;
			
		}
		p->next = NULL;
		p = &head;
		/*���ṹ�������ݵĴ���ʱ�䡢���͡���С�����Ƶ���Ϣ�������*/
		while(p != NULL)
		{
			ftime(p->time);
			if(p->type == 0)
				printf("\t<DIR>\t\t");
			else
				printf("\t\t%9lu", p->size);
			printf ("\t%s\n", p->name);		
			p = p->next;
		}
		free(p);
		/*��ʾ�ļ���Ŀ¼���������̿ռ������Ϣ*/
		printf("%15d ���ļ�\t\t\t%I64d �ֽ� \n", file, sum_file);
		GetDiskFreeSpaceEx(DirSpec[1], (PULARGE_INTEGER)&l_user, (PULARGE_INTEGER)&l_sum, (PULARGE_INTEGER)&l_idle);	
		printf("%15d ��Ŀ¼\t\t\t%I64d �����ֽ� \n", dir, l_idle);
			
		dwError = GetLastError();
		FindClose(hFind);
		/*������������쳣��������������Ϣ*/
		if (dwError != ERROR_NO_MORE_FILES) 
		{
			printf ("FindNextFile error. Error is %u\n", dwError);		
		}
	}	
}


/************************************ʱ�䴦����********************************************/

void ftime(FILETIME filetime)
{
	SYSTEMTIME systemtime;
	/*Win32ʱ��ĵ�32λ*/
	if (filetime.dwLowDateTime == -1)
	{
		wprintf(L"Never Expires ");
	}
	else
	{   
		/*��UTC(Universal Time Coordinated)�ļ�ʱ��ת���ɱ����ļ�ʱ��*/
		if (FileTimeToLocalFileTime(&filetime, &filetime) != 0) 
		{   
            /*��64λʱ��ת��Ϊϵͳʱ��*/
			if (FileTimeToSystemTime(&filetime, &systemtime) != 0)
			{
				char str[50];
				/*��һ����ʽ���ʱ��*/
				wsprintf(str, "%d-%02d-%02d  %02d:%02d", 
					systemtime.wYear, systemtime.wMonth, systemtime.wDay,
					systemtime.wHour, systemtime.wMinute);				
				printf("%s", str);
				
			}
			else
			{   /*wprintf���UNICODE�ַ�*/
				wprintf(L"FileTimeToSystemTime failed ");
			}
			
		}
		else
		{
			wprintf(L"FileTimeToLocalFileTime failed ");
		}
	}	
}


/**************************************��ȡϵͳ��������********************************************/

void GetProcessList()
{
	HANDLE  hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};
	/*��ϵͳ�н��̽�������*/
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if( hProcessSnap == INVALID_HANDLE_VALUE)
		printf("\nCreateToolhelp32Snapshot() failed:%d", GetLastError ());
	
	/*ʹ��ǰҪ���ṹ��С*/    
	pe32.dwSize = sizeof(PROCESSENTRY32);
	/*�г�����*/
	if( Process32First (hProcessSnap, &pe32) )
	{
		DWORD dwPriorityClass;
		printf("\n���ȼ�\t\t����ID\t\t�߳�\t\t������\n");
		do{
			HANDLE hProcess;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			dwPriorityClass = GetPriorityClass (hProcess);
			CloseHandle(hProcess);
			/*������*/
			printf("%d\t", pe32.pcPriClassBase);
			printf("\t%d\t", pe32.th32ProcessID);
			printf("\t%d\t", pe32.cntThreads);
            printf("\t%s\n", pe32.szExeFile);
		}
		while(Process32Next (hProcessSnap, &pe32));
	}
	else
		printf("\nProcess32finst() failed:%d", GetLastError ()); 
	CloseHandle (hProcessSnap);
}


/************************************history����*******************************************/

void add_history(char *inputcmd)
{   
	/*endǰ��һλ*/
	envhis.end = (envhis.end + 1) % HISNUM;
	/*end��startָ��ͬһ����*/
	if(envhis.end == envhis.start)
	{
		/*startǰ��һλ*/
		envhis.start = (envhis.start+1) % HISNUM;
	}
	/*���������endָ���������*/
	strcpy(envhis.his_cmd[envhis.end], inputcmd);	
}


/************************************��ʾhistory����********************************************/

void history_cmd()
{
	// TODO: ע�⣬������history���������⣬����ע���޸ġ�
	int i, j = 1;
	/*ѭ������Ϊ�գ�ʲôҲ����*/
	if(envhis.start == envhis.end)
		;
	else if(envhis.start < envhis.end){
		/*��ʾhistory����������start+1��end������*/
		for(i = envhis.start + 1;i <= envhis.end; i++)
		{
			printf("%d\t%s\n", j, envhis.his_cmd[i]);
			j++;
		}
	}else {
		/*��ʾhistory����������start+1��HISNUM-1������*/
		for(i = envhis.start + 1;i < HISNUM; i++)
		{
			printf("%d\t%s\n", j, envhis.his_cmd[i]);
			j++;
		}
		/*��ʾhistory����������0��end+1������*/
		for (i = 0; i <= envhis.end+1; i++)
		{
			printf("%d\t%s\n", j, envhis.his_cmd[i]);
			j++;
		}
	}
}


/***********************************������������*******************************************/

HANDLE process(int bg, char appName[])
{

	/*��ʼ�����������Ϣ*/
	STARTUPINFO si;
	PROCESS_INFORMATION   pi; 
	/*���ڰ汾����*/
	si.cb = sizeof(si);
	GetStartupInfo(&si);
	/*��ȥpi������*/
	ZeroMemory(&pi, sizeof(pi));
	/*ǰ̨����*/
	if(bg == 0)
	{  	
		/*���ù��ӣ���׽ctrl+c����յ�����������*/		
		if(SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE) == FALSE)
		{
			printf("Unable to install handler!\n");
				return NULL;
		} 
		/*���ý�����س��򣬴˴�����һ���Լ���д�ĳ���,����п���̨�����ע��·����ȷ*/
		CreateProcess(NULL, appName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		return pi.hProcess;
		
	}
	/*��̨����*/
	else
	{
		/*���ý��̴���ѡ��*/
		si.dwFlags = STARTF_USESHOWWINDOW; 
		/*���ش���*/
		si.wShowWindow = SW_HIDE;
		CreateProcess(NULL, appName, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);	
		return NULL;
	}
}


/***********************************��̨��ǰ̨��������*******************************************/

BOOL fp(char *pid)
{
	int id;
	HWND WND;
	id = atoi(pid);
	
	WND = ReturnWnd(id);
	if(WND==NULL)
		return FALSE;
	ShowWindow(WND, SW_SHOWNORMAL );
	 UpdateWindow(WND);
	if(SetForegroundWindow(WND))
		return TRUE;
	else
		return FALSE;
	
}

/***********************************kill��������*******************************************/

BOOL killProcess(char *pid)
{
	int id, i;
	DWORD   dwExitStatus;
	HANDLE hprocess;
	id = atoi(pid);
	hprocess = OpenProcess(PROCESS_TERMINATE, FALSE, id);
	GetExitCodeProcess(hprocess, &dwExitStatus); 
	if(i = TerminateProcess(hprocess, dwExitStatus))
		return TRUE;
	else 
		return FALSE;
}


/***********************************�ص�����*******************************************/

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch(CEvent)
	{
		
	case CTRL_C_EVENT:                        /*��ϵͳ�����¼�������ctrl+c��*/		
		break;
	case CTRL_BREAK_EVENT:		
		break;
	case CTRL_CLOSE_EVENT:		
		break;
	case CTRL_LOGOFF_EVENT:		
		break;
	case CTRL_SHUTDOWN_EVENT:		
		break;
	}
	return TRUE;
}
/*************************************��ѯPID�Ĵ���******************************************/
BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)  
{  
      
    EnumFunArg  *pArg = (LPEnumFunArg)lParam;     
    DWORD  processId;  
    GetWindowThreadProcessId(hwnd, &processId);      
    if( processId == pArg->dwProcessId)    
    {     
        pArg->hWnd = hwnd;  
        return TRUE;      
    }  
    return FALSE;     
}

HWND ReturnWnd(DWORD processID)  
{  
   HWND retWnd=NULL;    
   EnumFunArg wi;    
    wi.dwProcessId   =processID;    
    wi.hWnd   =  NULL;    
    EnumWindows(lpEnumFunc,(LPARAM)&wi);  
   if(wi.hWnd){
	   retWnd=wi.hWnd;  
   }   return retWnd;  
}  

/***********************************��ʾ����*******************************************/

void help()
{
// TODO: ��ӱ�Ҫ��ע�������Ϣ
}

