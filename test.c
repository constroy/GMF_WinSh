#include<stdio.h>
#include <windows.h>
 int main()
{
	int a,b,tmp;
	a=0;b=1;
	while(1){
		printf("%d\n",a );
		tmp=a+b;
		a=b;
		b=tmp;
		Sleep(500);
	}
	return 0;
}