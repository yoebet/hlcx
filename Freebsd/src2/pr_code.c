#include <stdio.h>
#include <string.h>
#include <signal.h>
extern short print;

/*-------------------
     打开打印机
-------------------*/
openlp()
{
  /**ESC[5i****/
  printf("\x1b\x5b\x35\x69");
  newline(3);
  return(0);
}

/*-------------------
     关闭打印机
-------------------*/
closelp()
{
  /**ESC[4i****/
  sleep(2);
  printf("\x1b\x5b\x34\x69");
  sleep(2);
	return(0);
}
/*-------------------
     前进line行
-------------------*/
newline(line)
int line;
{
    int i;
	if (line==0) return(0);
    for(i=1;i<=line;i++)
    printf("\n");
    return(0);
}
/*-------------------
     退    纸
-------------------*/
backpage()
{
  printf("\x0c");
	sleep(2);
	return(0);
}

pramt(src,obj,len)
char *src,*obj;
int len;
{
	int  i,j;
	char *sp,*tp;

	while(*src==' ') src++;
	i=strlen(src);
	if(i>len)
		return(-1);
	sp=src;
	sp+=(i-1);
	tp=obj;
	tp+=(len-1);
	*tp=0;
	tp--;
	j=0;
	while(j<i||len>1)
	{
		if(isdigit(*sp)==0)
			break;
		switch(j)
		{
			case 2:
				*tp='.';
				tp--;
				break;
			case 6:
			case 10:
			case 14:
			case 18:
			case 22:
			case 26:
				*tp=',';
				tp--;
				break;
			default:
				*tp=*sp;
				tp--;
				sp--;
				break;
		}
		j++;
		len--;
	}
	for(;j<4;j++)
	{
		switch(j)
		{
			case 2:
				*tp='.';
				break;
			default:
				*tp='0';
				break;
		}
		tp--;
		len--;
	}
	if(len>1)
	{
		//  *tp='$';
		*tp=' ';
		len--;
		tp--;
	}
	for(;len>1;len--)
	{
		*tp=' ';
		tp--;
	}
}

prtymd(space_num,_ymd)
int space_num;              /**年月日之间几个空格**/
char _ymd[];
{
	int i,j;
	printf("%c%c",_ymd[0],_ymd[1]);
	printf("%c%c",_ymd[2],_ymd[3]);
	for(i=0;i<space_num;i++)
		printf(" ");
	printf("%c%c",_ymd[5],_ymd[6]);
	for(i=0;i<space_num;i++)
		printf(" ");
	printf("%c%c",_ymd[8],_ymd[9]);
}
/*-------------------
设置水平扩展打印方式
-------------------*/
set_right()
{
	printf("\x1b\x57\x01");
	return(0);
}
/*-------------------
取消水平扩展打印方式
-------------------*/
un_right()
{
	printf("\x1b\x57\x02");
	return(0);
}
