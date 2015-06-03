#include<stdio.h>
#include <windows.h>
 int main()
{
	FILE *in;
	int a,b,tmp;
	a=0;b=1;
	while(1){
		in = fopen("Fibonacci.txt","a");
		fprintf(in,"%d\n",a );
		fclose(in);
		tmp=a+b;
		a=b;
		b=tmp;
		Sleep(500);
	}
	return 0;
}