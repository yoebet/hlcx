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

#define DEBUG      0                       //��־���Ա�ʶ!

#define TIMEOUT         180                 //��ʱ����

#define UNIXPC 		0                   //0: �������ն�
					    //1: ����ֱ����UNIX������
#define SET_KEYLOCK     0                   //0�����̲�����
                                            //1����������    


#define SERIALCOM1_TTYNAME "/dev/ttyd0"     //UNIX����������1������
#define SERIALCOM2_TTYNAME "/dev/ttyd1"     //UNIX����������2������

#define ABORTCHAR 0x4                       //ǿ���˳����� Ctrl+D

#define MSR102_COMM_SET 	1           //�ſ����

#define ISO 0
#define IBM 1
#define MSR_DEFAULT_FORMAT	1           //0: ʹ���Զ���ſ���ʽ
#define MSR_FORMAT 		ISO         //�ſ���ʽ:ISO
#define MSR_CHARATER_SET 	IBM         //�ſ��ɶ�д�ַ���:IBM
/*
	�ն˸����ڿ�������
*/
#define KEYLOCK "\033[2h"
#define KEYUNLOCK "\033[2l"
/*ʵ���ն˸��ڿ�������*/
#define STAR_OPEN_AUX1 "\033!0;0;0;0Y\033[/54l\033[/53h\033[/50h"
#define STAR_OPEN_AUX2 "\033!3;0;0;0Z\033[/54l\033[/53h\033[/50h"
#define STAR_OPEN_AUX3 "\033!0;0;0;0X\033[/54l\033[/53h\033[/50h"
#define STAR_OPEN_AUX4 "\033!0;0;0;0V\033[/54l\033[/53h\033[/50h"
#define STAR_CLOSE_AUX "\033[/50l"
#define STAR_AUXLEAD "\033[/51h"
#define STAR_AUXEND	"\033[/51l"

/*�´�½�ն˸��ڿ�������*/
#define NEWLAND_OPEN_AUX1 "\033!0;0;0;0Y\033[/50;0h\033[2h\033[/54l"
#define NEWLAND_OPEN_AUX2 "\033!0;0;0;0Z\033[/50;1h\033[2h\033[/54l"
#define NEWLAND_OPEN_AUX3 "\033!0;0;0;0X\033[/50;2h\033[2h\033[/54l"
#define NEWLAND_OPEN_AUX4 "\033!0;0;0;0V\033[/50;2h\033[2h\033[/54l"
#define NEWLAND_CLOSE_AUX "\033[/50l"
#define NEWLAND_AUXLEAD "\033[/51h"
#define NEWLAND_AUXEND  "\033[/51l"

/*�����ն˸��ڿ�������*/
#define GG_OPEN_AUX1 "\033[/1;1;8;0;1;0M\033[/54l"
#define GG_OPEN_AUX2 "\033[/2;1;8;0;1;0M\033[/54l"
#define GG_OPEN_AUX3 "\033[/3;1;8;0;1;0M\033[/54l"
#define GG_OPEN_AUX4 "\033[/4;1;8;0;1;0M\033[/54l"
#define GG_CLOSE_AUX "\033[/M"
#define GG_AUXLEAD "\033[/|"
#define GG_AUXEND "|"

/*�����ն˸��ڿ�������*/
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
	����UNIXͨѶ�ڲ���

		��ɷ�����ʽ

	����: 	-1: ����
		0:  ��ȷ����
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

	��ɷ�����ʽͨѶ��ʽ
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

	�������ʽͨѶ��ʽ
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
	�ָ�UNIXͨѶ�ڲ���

	����: 	-1: ����
		0:  ��ȷ����
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
	������ͨѶ��
     term:�ն�����
        1:ʵ�� 
        2:�´�½ 
        3:���� 
        4:����               
     port  : �ն˽ӿ�
     	  1 ����һ
     	  .
     	  4 ������
    ����: 	-1: ����
		         0:  ��ȷ����

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
   ���͸�������ͷ
     term:�ն�����
        1:ʵ�� 
        2:�´�½ 
        3:���� 
        4:����               
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
  ���͸�������β
     term:�ն�����
        1:ʵ�� 
        2:�´�½ 
        3:���� 
        4:����               
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
	������������
	data: Ҫ�������������
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
	���������ն�

	data: Ҫ�����ն˵�����
*/

int Star_dataToTerminal(data)
char *data;
{
	write(star_fd,data,strlen(data));
	return 0;
}
/*
	int closePort(char term);
	�ر�����ͨѶ��
	 int term:�ն�����
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
	���������һ���ַ�
	ch_point: �����践�ص��ַ�
	����ֵ: 0:���յ��ַ�
		-1:δ���յ��ַ�
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

	���������ַ���
	string:  �����ַ���
	length:  Ҫ�����ַ�������
	timeout: ��ʱʱ��
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

	��һ������ʼ������ַ����ַ���
	string:  �����ַ���
	lead_char: ��ʼ�ַ����ַ���
	end_char: �����ַ����ַ���
	timeout: ��ʱʱ��
        ˵��:�����˶�101��֧��
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

	string:  �����ַ���
	pin_length: ���������볤��
	timeout: ��ʱʱ��
	����ֵ:
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
	����:bcd�ַ��� ת�� Ϊ ASC�ַ���
	����:
		   psBcd : �����BCD�ַ���
		   nlen  : �����BCD�ַ�������
		   psAsc : ���ת�����ASC�ַ���
		   byMode: ת����ʽ
		   	       0 : ��4λ�͵�4λ��BCD��ֱ�ת��ΪASCII��.
		   	       1 : ��4λ�͵�4λ��BCD��ֱ�ֱ�Ӳ�ֵ������ֽڵĵ�4λ.
		   	       2 : ��4λ�͵�4λ��BCD��ֱ����0X30��Ϊ����ASC�ַ�.
  ����: ��
  	   
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
	����:ASC�ַ��� ת�� Ϊ BCD�ַ���
	����:
		   psAsc : �����ASC�ַ���
		   nlen  : �����ASC�ַ�������
		   psBcd : ���ת�����BCD�ַ���
		   byMode: ת����ʽ
		   	       0 : ����ASC�ַ����һ��BCD�ֽ�
		   	       1 : ����ASC�ַ��ĵ�4λ���һ��BCD�ֽڵĸߵ�4λ.
		   	       2 : ����ASC�ַ���ȥ0x30��ĵ�4λ���һ��BCD�ֽڵĸߵ�4λ.
  ����: ��
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
