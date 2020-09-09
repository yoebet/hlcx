#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>

#include "pubsub.c"
#include "st102ic.c"
#include "msr.c"
#include "pinpad.c"
#define TERM_TYPE       1                   //终端类型
                                //1:实达终端 2:新大陆终端 3:国光终端 4:长城终端
                                            
#define PINPAD_PORT     2                   //密码键盘所接的终端辅口
#define MSR_PORT        1                   //磁卡机具所接的终端辅口
#define ICC_PORT        1                   //IC卡所接的终端辅口
                                       //1:辅口一 2:辅口二 3:辅口三 4:辅口四

write_msr(char *str,short kaz)
{
	int flag,qq;
	flag=1;
	while(flag!=0)
	{
		if(kaz==0)
		  mvprintw(22,22,"请在读卡器指示灯亮后划存折写磁条!");
		else if(kaz==1)
		  mvprintw(22,22,"请在读卡器指示灯亮后划卡写磁条!");
		refresh();
		flag=starMsrWrite(TERM_TYPE,MSR_PORT,str,str,TIMEOUT);
    if (flag!=0)
    {
 			mvprintw(22,22,"写磁条错，是否重新写入(y/n)？");
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
		  mvprintw(22,22,"请在读卡器指示灯亮后划存折读磁条!");
		else if(kaz==1)
		  mvprintw(22,22,"请在读卡器指示灯亮后划卡读磁条!");
		refresh();
 		flag=starMsrRead(TERM_TYPE,MSR_PORT,2,str2,str3,TIMEOUT);
    if (flag!=0)
    {
 			mvprintw(22,22,"读磁条错，是否重新读取(y/n)？");
  	  refresh();
  		qq=getch();
  		if (qq !='Y' && qq!='y')
			break;
 		}
 	}
	return(flag);
}


