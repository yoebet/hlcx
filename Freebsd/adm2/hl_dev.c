#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>

#include "pubsub.c"
#include "st102ic.c"
#include "msr.c"
#include "pinpad.c"
#define TERM_TYPE       1                   //�ն�����
                                //1:ʵ���ն� 2:�´�½�ն� 3:�����ն� 4:�����ն�
                                            
#define PINPAD_PORT     2                   //����������ӵ��ն˸���
#define MSR_PORT        1                   //�ſ��������ӵ��ն˸���
#define ICC_PORT        1                   //IC�����ӵ��ն˸���
                                       //1:����һ 2:���ڶ� 3:������ 4:������

write_msr(char *str,short kaz)
{
	int flag,qq;
	flag=1;
	while(flag!=0)
	{
		if(kaz==0)
		  mvprintw(22,22,"���ڶ�����ָʾ�����󻮴���д����!");
		else if(kaz==1)
		  mvprintw(22,22,"���ڶ�����ָʾ�����󻮿�д����!");
		refresh();
		flag=starMsrWrite(TERM_TYPE,MSR_PORT,str,str,TIMEOUT);
    if (flag!=0)
    {
 			mvprintw(22,22,"д�������Ƿ�����д��(y/n)��");
  		refresh();
  		qq=getch();
  		if (qq !='Y' && qq!='y')
				return -1;
 		}
 	}
	return(0);
}


read_msr(char *str2,short kaz)
{
	char str3[100];
	int flag,qq;
	flag=1;
	while(flag!=0)
	{
		if(kaz==0)
		  mvprintw(22,22,"���ڶ�����ָʾ�����󻮴��۶�����!");
		else if(kaz==1)
		  mvprintw(22,22,"���ڶ�����ָʾ�����󻮿�������!");
		refresh();
 		flag=starMsrRead(TERM_TYPE,MSR_PORT,2,str2,str3,TIMEOUT);
    if (flag!=0)
    {
 			mvprintw(22,22,"���������Ƿ����¶�ȡ(y/n)��");
  	  refresh();
  		qq=getch();
  		if (qq !='Y' && qq!='y')
			break;
 		}
 	}
	return(flag);
}


