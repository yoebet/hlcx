/*
	PUBSUB.C
	STAR DRIVERS PUBLIC PROGRAM FOR UNIX
	AUTHOR:		Wind
	VERSION:	V1.0
	DATE: 		1998.09.06
	Copyright (c) 1998-2000 START Group Co. Ltd.
   	All rights reserved.
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/termios.h>

#define SUC 0
#define ERR 1

#define DEBUG      0                       //日志调试标识!

#define TIMEOUT         180                 //超时设置

#define UNIXPC 		0                   //0: 外设连终端
					    //1: 外设直接连UNIX服务器
#define SET_KEYLOCK     0                   //0：键盘不锁定
                                            //1：键盘锁定    


#define SERIALCOM1_TTYNAME "/dev/ttyd0"     //UNIX服务器串口1串口名
#define SERIALCOM2_TTYNAME "/dev/ttyd1"     //UNIX服务器串口2串口名

#define ABORTCHAR 0x4                       //强行退出按键 Ctrl+D

#define MSR102_COMM_SET 	1           //磁卡命令集

#define ISO 0
#define IBM 1
#define MSR_DEFAULT_FORMAT	1           //0: 使用自定义磁卡格式
#define MSR_FORMAT 		ISO         //磁卡格式:ISO
#define MSR_CHARATER_SET 	IBM         //磁卡可读写字符集:IBM
/*
	终端辅助口控制命令
*/
#define KEYLOCK "\033[2h"
#define KEYUNLOCK "\033[2l"
/*实达终端辅口控制命令*/
#define STAR_OPEN_AUX1 "\033!0;0;0;0Y\033[/54l\033[/53h\033[/50h"
#define STAR_OPEN_AUX2 "\033!3;0;0;0Z\033[/54l\033[/53h\033[/50h"
#define STAR_OPEN_AUX3 "\033!0;0;0;0X\033[/54l\033[/53h\033[/50h"
#define STAR_OPEN_AUX4 "\033!0;0;0;0V\033[/54l\033[/53h\033[/50h"
#define STAR_CLOSE_AUX "\033[/50l"
#define STAR_AUXLEAD "\033[/51h"
#define STAR_AUXEND	"\033[/51l"

/*新大陆终端辅口控制命令*/
#define NEWLAND_OPEN_AUX1 "\033!0;0;0;0Y\033[/50;0h\033[2h\033[/54l"
#define NEWLAND_OPEN_AUX2 "\033!0;0;0;0Z\033[/50;1h\033[2h\033[/54l"
#define NEWLAND_OPEN_AUX3 "\033!0;0;0;0X\033[/50;2h\033[2h\033[/54l"
#define NEWLAND_OPEN_AUX4 "\033!0;0;0;0V\033[/50;2h\033[2h\033[/54l"
#define NEWLAND_CLOSE_AUX "\033[/50l"
#define NEWLAND_AUXLEAD "\033[/51h"
#define NEWLAND_AUXEND  "\033[/51l"

/*国光终端辅口控制命令*/
#define GG_OPEN_AUX1 "\033[/1;1;8;0;1;0M\033[/54l"
#define GG_OPEN_AUX2 "\033[/2;1;8;0;1;0M\033[/54l"
#define GG_OPEN_AUX3 "\033[/3;1;8;0;1;0M\033[/54l"
#define GG_OPEN_AUX4 "\033[/4;1;8;0;1;0M\033[/54l"
#define GG_CLOSE_AUX "\033[/M"
#define GG_AUXLEAD "\033[/|"
#define GG_AUXEND "|"

/*长城终端辅口控制命令*/
#define GW_OPEN_AUX1 "\033[>"
#define GW_OPEN_AUX2 "\033[1>"
#define GW_OPEN_AUX3 "\033[2>"
#define GW_OPEN_AUX4 "\033[3>"
#define GW_CLOSE_AUX "\033[<"
#define GW_AUXLEAD ""
#define GW_AUXEND ""

#define ALL_ON "\200"
#define RED_ON "\201"
#define GREEN_ON "\202"
#define ALL_OFF "\203"
#define CHECK_PINPAD "\211"

#define Open_Bp_Port_A "\033%A"
#define Open_Bp_Port_B "\033%B"
#define Open_Bp_Port_K "\033%K"
#define Open_Bp_Port_C "\033%C"


/*
	int setTerm(void);
	设置UNIX通讯口参数

		设成非正则方式

	返回: 	-1: 出错
		0:  正确返回
*/
void Star_WriteLog(char *Msg,int ret_code);
int Star_ttyname(char *name);
static int star_fd=1;
static struct termios oterm_attr,term_attr;
int Star_setTerm()
{

	if(star_fd<0) return -1;

	if(tcgetattr(star_fd, &oterm_attr)<0) return -1;
	//if(ioctl(star_fd,TCGETA,&oterm_attr)<0) return -1;
	if(tcgetattr(star_fd, &term_attr)<0) return -1;
	//if(ioctl(star_fd,TCGETA,&term_attr)<0) return -1;

	//term_attr.c_lflag &=~(ISIG|ECHO|ICANON|NOFLSH|XCLUDE);
  term_attr.c_lflag &=~(ISIG|ECHO|ICANON|NOFLSH);
	term_attr.c_iflag &=~(IXON|INLCR|IGNCR|ICRNL|ISTRIP);
	term_attr.c_oflag &=~OPOST;

	term_attr.c_cc[VMIN]=0;
	term_attr.c_cc[VTIME]=1;
	term_attr.c_cc[VQUIT]=0x7f;
#if UNIXPC
	term_attr.c_cflag&=~CBAUD;
	term_attr.c_cflag&=~CSIZE;
	term_attr.c_cflag|=B9600;
	term_attr.c_cflag|=CS8;
	term_attr.c_cflag|=(CLOCAL|CREAD);
	term_attr.c_cflag&=~(PARENB|CSTOPB);
#endif
	if(SET_TTY(star_fd,&term_attr)<0) return -1;
	//if(ioctl(star_fd,TCSETAW,&term_attr)<0) return -1;

	return star_fd;
}
/*
	int setNowait(void);

	设成非正则方式通讯方式
*/
int Star_setNowait()
{
	if(star_fd<0) return -1;
	
	if(tcgetattr(star_fd, &term_attr)<0) return -1;
	//if(ioctl(star_fd,TCGETA,&term_attr)<0) return -1;
	term_attr.c_cc[VMIN]=0;
	if(SET_TTY(star_fd,&term_attr)<0) return -1;
	//if(ioctl(star_fd,TCSETAW,&term_attr)<0) return -1;
	return 0;
}
/*
	int resetNowait(void);

	设成正则方式通讯方式
*/
int Star_resetNowait()
{
	if(star_fd<0) return -1;
	if(tcgetattr(star_fd, &term_attr)<0) return -1;
	//if(ioctl(star_fd,TCGETA,&term_attr)<0) return -1;
	term_attr.c_cc[VMIN]=1;
	term_attr.c_cc[VTIME]=0;
	if(SET_TTY(star_fd,&term_attr)<0) return -1;
	//if(ioctl(star_fd,TCSETAW,&term_attr)<0) return -1;
	return 0;
}
/*
	int resetTerm(void);
	恢复UNIX通讯口参数

	返回: 	-1: 出错
		0:  正确返回
*/
int Star_resetTerm()
{

	if(star_fd<0) return -1;
	if(SET_TTY(star_fd,&oterm_attr)<0) return -1;
	//if(ioctl(star_fd,TCSETAW,&oterm_attr)<0) return -1;
	if(star_fd!=0) close(star_fd);
	return 0;
}

/*
	int Star_openPort(int term,char port);
	打开外设通讯口
     term:终端类型
        1:实达 
        2:新大陆 
        3:国光 
        4:长城               
     port  : 终端接口
     	  1 辅口一
     	  .
     	  4 辅口四
    返回: 	-1: 出错
		         0:  正确返回

*/
int Star_openPort(int term,int port)
{
        int ret_code;
#if UNIXPC
	      switch(port)
	      {
		      case 1:  star_fd=open(SERIALCOM1_TTYNAME,O_RDWR);break;
		      case 2:  star_fd=open(SERIALCOM2_TTYNAME,O_RDWR);break;
		      default: return -1;
	      }
#else
        star_fd=open((char *)ttyname(1),O_RDWR);
#endif
	      if (star_fd<0) return -1;	
        ret_code=Star_setTerm();
        if(ret_code==-1)return -1;
#if UNIXPC
#else
        switch(term)
        {
           case 2:
                	switch(port)
        		      {
                                
                                case 2:  write(star_fd,NEWLAND_OPEN_AUX2,strlen(NEWLAND_OPEN_AUX2));break;
                                case 3:  write(star_fd,NEWLAND_OPEN_AUX3,strlen(NEWLAND_OPEN_AUX3));break;
                        	      case 4:  write(star_fd,NEWLAND_OPEN_AUX4,strlen(NEWLAND_OPEN_AUX4));break;
                        	      case 1: 
                        	      default: write(star_fd,NEWLAND_OPEN_AUX1,strlen(NEWLAND_OPEN_AUX1));break;
        		      }
                	break;
            case 3:	
                	switch(port)
        		      {
                                case 2:  write(star_fd,GG_OPEN_AUX2,strlen(GG_OPEN_AUX2));break;
                                case 3:  write(star_fd,GG_OPEN_AUX3,strlen(GG_OPEN_AUX3));break;
                        	      case 4:  write(star_fd,GG_OPEN_AUX4,strlen(GG_OPEN_AUX4));break;
                        	      case 1: 
                        	      default: write(star_fd,GG_OPEN_AUX1,strlen(GG_OPEN_AUX1));break;
        		      }
                	break;
           case 4:	
                	switch(port)
        		      {             
                                case 2:  write(star_fd,GW_OPEN_AUX1,strlen(GW_OPEN_AUX1));break;
                                case 3:  write(star_fd,GW_OPEN_AUX1,strlen(GW_OPEN_AUX1));break;
                        	      case 4:  write(star_fd,GW_OPEN_AUX4,strlen(GW_OPEN_AUX4));break;
                        	      case 1:  
                        	      default: write(star_fd,GW_OPEN_AUX1,strlen(GW_OPEN_AUX1));break;
        		      }
                	break;
            case 1:	
            default:
 
            	     switch(port)
        		       {
                                case 2:  write(star_fd,STAR_OPEN_AUX2,strlen(STAR_OPEN_AUX2));break;
                                case 3:  write(star_fd,STAR_OPEN_AUX3,strlen(STAR_OPEN_AUX3));break;
                        	      case 4:  write(star_fd,STAR_OPEN_AUX4,strlen(STAR_OPEN_AUX4));break;
                        	      case 1:  
                        	      default: write(star_fd,STAR_OPEN_AUX1,strlen(STAR_OPEN_AUX1));break;
        		       } 
                	break;
                 
        }
        fflush(stdin);
#endif
#if SET_KEYLOCK
        Star_dataToTerminal(KEYLOCK);
#endif

        fflush(stdout);
        return 0;
}
/*int Star_auxhead(int term)
   发送辅口数据头
     term:终端类型
        1:实达 
        2:新大陆 
        3:国光 
        4:长城               
*/
int Star_auxhead(int term)
{
        switch(term)
        {
                case 2:  write(star_fd,NEWLAND_AUXLEAD,strlen(NEWLAND_AUXLEAD));break;
                case 3:  write(star_fd,GG_AUXLEAD,strlen(GG_AUXLEAD));break;
                case 4:  write(star_fd,GW_AUXLEAD,strlen(GW_AUXLEAD));break;
                case 1: 
                default: write(star_fd,STAR_AUXLEAD,strlen(STAR_AUXLEAD));break;
        }
        fflush(stdin);
        fflush(stdout);
        return 0;
}
/*int Star_auxtail(int term)
  发送辅口数据尾
     term:终端类型
        1:实达 
        2:新大陆 
        3:国光 
        4:长城               
*/
int Star_auxtail(int term)
{
        switch(term)
        {
                case 2:  write(star_fd,NEWLAND_AUXEND,strlen(NEWLAND_AUXEND));break;
                case 3:  write(star_fd,GG_AUXEND,strlen(GG_AUXEND));break;
                case 4:  write(star_fd,GW_AUXEND,strlen(GW_AUXEND));break;
                case 1: 
                default: write(star_fd,STAR_AUXEND,strlen(STAR_AUXEND));break;
        }
        fflush(stdin);
        fflush(stdout);	
        return 0;
}
/*
	int Star_dataToDevice(int term,char *data);
	数据送往外设
	data: 要送往外设的数据
*/
int Star_dataToDevice(int term,char *str)
{
#if UNIXPC
 	      Star_dataToTerminal(str);
#else

        Star_auxhead(term);
        Star_dataToTerminal(str);
        Star_auxtail(term);
#endif
        return 0;
}
/*
	int Star_dataToTerminal(char *data);
	数据送往终端

	data: 要送往终端的数据
*/

int Star_dataToTerminal(data)
char *data;
{
	write(star_fd,data,strlen(data));
	return 0;
}
/*
	int closePort(char term);
	关闭外设通讯口
	 int term:终端类型
*/

int Star_closePort(int term)
{
        int ret_code;
        switch(term)
        	{
                        case 2:  write(star_fd,NEWLAND_CLOSE_AUX,strlen(NEWLAND_CLOSE_AUX));break;
                        case 3:  write(star_fd,GG_CLOSE_AUX,strlen(GG_CLOSE_AUX));break;
                        case 4:  write(star_fd,GW_CLOSE_AUX,strlen(GW_CLOSE_AUX));break;
                        case 1: 
                        default: write(star_fd,STAR_CLOSE_AUX,strlen(STAR_CLOSE_AUX));break;
        	}	
       
#if SET_KEYLOCK
        Star_dataToTerminal(KEYUNLOCK);
#endif
       
        ret_code=Star_resetTerm(); 
        return ret_code;
}
/*
	int charFromDevice(char *ch_point);
	从外设接收一个字符
	ch_point: 从外设返回的字符
	返回值: 0:接收到字符
		-1:未接收到字符
*/

int Star_charFromDevice(ch_point)
char *ch_point;
{
	int ret_code;

	ret_code=read(star_fd,ch_point,1);
	if (ret_code==1) return 0;
	return -1;
}

int Star_readChar(ch,timeout)
char *ch;
int timeout;
{
	int i,ret_code;

	for(i=0;i<timeout;i++)
	{
		ret_code=Star_charFromDevice(ch);
		if(ret_code==0) break;
	}
	if (ret_code==0) return 0;
	return -1;
}

/*
	readLenStr(char *string,int length,int timeout);

	读定长度字符串
	string:  返回字符串
	length:  要读的字符串长度
	timeout: 超时时间
*/
int Star_readLenStr(string,length,timeout)
char *string;
int length,timeout;
{
	int i,j,ret_code;
	char ch;

	fflush(stdin);

	for (i=0;i<length;i++)
	{
		for(j=0;j<timeout;j++)
		{
			ret_code=Star_charFromDevice(&ch);
			if (ret_code==0) break;
		}
		if (ret_code==-1) break;
		string[i]=ch;
	}
	return ret_code;
}
/*
	readLeadEndStr(char *string,char *lead_char,char *end_char,int timeout);

	读一个有起始与结束字符的字符串
	string:  返回字符串
	lead_char: 起始字符的字符串
	end_char: 结束字符的字符串
	timeout: 超时时间
        说明:加入了对101的支持
		if(strchr(lead_char,ch)!=NULL)
		{
                    if (string[i-2]!='?') //add by zkh
                    { 
			i=0;
			string[i++]=ch;
                    }
		}
*/

int Star_readLeadEndStr(string,lead_char,end_char,timeout)
char *string;
char *lead_char,*end_char;
int timeout;
{
	int i,j,ret_code;

	char ch;

	i=0;
	do
	{
		for(j=0;j<timeout;j++)
		{
			ret_code=Star_charFromDevice(&ch);
			if (ret_code==0) break;
		}
		string[i++]=ch;
		if (ret_code==-1) break;
		if(strchr(lead_char,ch)!=NULL)
		{
                    if (string[i-2]!='?') //add by zkh
                    { 
			i=0;
			string[i++]=ch;
                    }
		}
		if (ch==ABORTCHAR) break;
	} while(strchr(end_char,ch)==NULL);
	if ((ret_code==-1)||(ch==ABORTCHAR))
	{
		return -1;
	}
	string[i]=0;
	return i;
}
/*
	ReadPinPad(char *string,char *pin_length,int timeout);

	string:  返回字符串
	pin_length: 读到的密码长度
	timeout: 超时时间
	返回值:
		=0	ok
		=-1	error
*/
int Star_ReadPinPad(string,pin_length,timeout)
char *string;
char *pin_length;
int timeout;
{
	int nRetCode,j;
	char str[200];
	int nLength;
	
	nRetCode=Star_readLeadEndStr(str,"\002","\003",timeout);
	if(nRetCode == -1) return -1;
	nLength=(str[1]-0x30)<<4+str[2]-0x30;

	if(nLength!=strlen(str)-4) return -1;
	for(j=0;j<nLength/2;j++)
	{
		string[j]=((str[(j+1)*2+1]-0x30)<<4)|(str[(j+1)*2+2]-0x30);
	}
	string[nLength/2]=0;
	*pin_length=nLength/2;
	return 0;
}

/*
void Star_bcdToAsc(	unsigned char *psAsc,
	              unsigned char *psBcd,
	              unsigned int nLen,
	              unsigned char byMode)
	功能:bcd字符串 转换 为 ASC字符串
	参数:
		   psBcd : 输入的BCD字符串
		   nlen  : 输入的BCD字符串长度
		   psAsc : 输出转换后的ASC字符串
		   byMode: 转换格式
		   	       0 : 高4位和低4位的BCD码分别转化为ASCII码.
		   	       1 : 高4位和低4位的BCD码分别直接拆分到两个字节的低4位.
		   	       2 : 高4位和低4位的BCD码分别加上0X30成为两个ASC字符.
  返回: 无
  	   
*/
void Star_bcdToAsc(
	unsigned char *psAsc,
	unsigned char *psBcd,
	unsigned int nLen,
	unsigned char byMode)
{
	unsigned int i;
	unsigned char byTmp;

	for(i=0;i<nLen;i++)
	{
		switch(byMode)
		{
	    case 2:	
			     psAsc[i*2]=((psBcd[i]>>4)&0x0f)+0x30;
			     psAsc[i*2+1]=(psBcd[i]&0x0f)+0x30;
           break;   
			case 1:
				psAsc[2*i] = psBcd[i]>>4;
				psAsc[2*i+1] = psBcd[i]&0xf;
				break;
			case 0:
			default:
				byTmp = psBcd[i]>>4;
				if(byTmp<10)
					psAsc[2*i] = byTmp+0x30;
				else
					psAsc[2*i] = byTmp + 0x41 - 10;
				
				byTmp = psBcd[i]&0xf;
				if(byTmp<10)
					psAsc[2*i+1] = byTmp+0x30;
				else
					psAsc[2*i+1] = byTmp + 0x41 - 10;
				break;
		}
	}
}
/*unsigned char Star_ascToBcd(	unsigned char *psBcd,
	              unsigned char *psAsc,
	              unsigned int nLen,
	              unsigned char byMode)
	功能:ASC字符串 转换 为 BCD字符串
	参数:
		   psAsc : 输入的ASC字符串
		   nlen  : 输入的ASC字符串长度
		   psBcd : 输出转换后的BCD字符串
		   byMode: 转换格式
		   	       0 : 两个ASC字符组成一个BCD字节
		   	       1 : 两个ASC字符的低4位组成一个BCD字节的高低4位.
		   	       2 : 两个ASC字符减去0x30后的低4位组成一个BCD字节的高低4位.
  返回: 无
*/
unsigned char Star_ascToBcd(
	unsigned char *psBcd,
	unsigned char *psAsc,
	unsigned int nLen,
	unsigned char byMode)
{
	unsigned int i;
	unsigned int nRetCode;

	nRetCode=SUC;

	for(i=0;i<nLen/2;i++)
	{
		switch(byMode)
		{
			case 2:
				psBcd[i] = ((psAsc[2*i]-0x30)<<4)+((psAsc[2*i+1]-0x30)&0xf);
				break;
			case 1:
				psBcd[i] = (psAsc[2*i]<<4)+(psAsc[2*i+1]&0xf);
				break;
			case 0:
			default:
				if ((psAsc[2*i]<0x3a)&&(psAsc[2*i]>0x2f))
				{
					psBcd[i] = (psAsc[2*i]-0x30)<<4;
				}
				else
				{
					if((psAsc[2*i]>='A')&&(psAsc[2*i]<='F'))
					{
						psBcd[i] = (psAsc[2*i]-0x41+10)<<4;
					}
					else
					{
						psBcd[i] = psAsc[2*i]<<4;
						nRetCode=ERR;
					}	
				}
				if ((psAsc[2*i+1]<0x3a)&&(psAsc[2*i+1]>0x2f))
				{
					psBcd[i] += psAsc[2*i+1]-0x30;
				}
				else
				{
					if((psAsc[2*i+1]>='A')&&(psAsc[2*i+1]<='F'))
					{
						psBcd[i] += psAsc[2*i+1]-0x41+10;
					}
					else
					{
						psBcd[i] = psAsc[2*i]<<4;
						nRetCode=ERR;
					}	
				}
				break;
		}
	}
	return nRetCode;
}
int Star_ttyname(char *name)
{
    char *p;
    int i;
    for (i=0;i<strlen(name);name[i++]='\0');
    p=strrchr((char *)ttyname(1),'t');
    *name++='t';
    while (*p!='\0')
        *name++=*p++;
    return 0;
}
void Star_WriteLog(char *Msg,int ret_code)
{
  FILE *fp;
  char ttyname[15];
  memset(ttyname,'\0',15);
  Star_ttyname(ttyname);
  strcat(ttyname,".log");
#if DEBUG
  if((fp=fopen(ttyname,"a+"))==NULL) return;
  fprintf(fp,"%s ",Msg);
  fprintf(fp,"%d\n",ret_code);
  fclose(fp);
#endif	
}
