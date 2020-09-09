/*
	PINPAD.C
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
#include <sys/fcntl.h>

#include "mdes.c"
#define FALSE -1
#define TRUE 0
//static int Star_ResetFlag = 0;
//static int Star_ResetFlag_3DES = 0;
/*
int STAR_Set3DesMode()
   设置3DES密码键盘3DES模式函数
   返回：0 正确
         －1 错误
*/
int STAR_Set3DesMode(int term,int port)
{
	char stringtmp[10];
        int ret;
        unsigned char retchar;
  memset(stringtmp,0,sizeof(stringtmp));
	sprintf(stringtmp,"\033V\x31\x0d\x0a");
	stringtmp[5]='\0';
	if(Star_openPort(term,port) == -1)
	{
		 //Star_closePort(term);
		return FALSE;
	}
	fflush(stdout);
	Star_dataToDevice(term,stringtmp);
	fflush(stdout);
	fflush(stdout);
	ret=Star_readLenStr(&retchar,1,20);
	Star_closePort(term);
	if(ret == -1 || retchar != 0xaa) 
		return FALSE;
	return TRUE;
}
/*int STAR_ClearLED()
    清除DES密码键盘液晶屏幕，显示首页
  返回：0
*/
int STAR_ClearLED(int term,int port)
{
	char stringtmp[5];
   memset(stringtmp,0,sizeof(stringtmp));

	sprintf(stringtmp,"\033C\x0d\x0a");
	stringtmp[4]='\0';
	if(Star_openPort(term,port) == -1)
		return FALSE;
	fflush(stdout);
	Star_dataToDevice(term,stringtmp);
	fflush(stdout);
	Star_closePort(term);
	return TRUE;
}
/*
int STAR_Reset_PinPad(int term,int port,int timeouts)
     功能: 复位DES密码键盘将密码键盘复位成出厂状态。
     即：主密钥设置成88888888，用户密钥设成00000000,激活主密钥号为0用户密钥号为0的用户密钥。
   	  
     参数：int timeouts :超时时间    
     返回：0  OK
            -1 ERR
*/
int STAR_Reset_PinPad(int term, int port,int timeouts)
{
	char stringtmp[10],stringtmp1[10];
  
	unsigned char retchar;
	int ret;
  memset(stringtmp1,0,sizeof(stringtmp1));
  memset(stringtmp,0,sizeof(stringtmp));
	
	sprintf(stringtmp,"\033R\x0d\x0a");
	stringtmp[4]='\0';
	if(Star_openPort(term,port) == -1)
		return FALSE;
	fflush(stdout);
	Star_dataToDevice(term,stringtmp);
	fflush(stdout);
	ret=Star_readLenStr(&retchar,1,timeouts);
	Star_closePort(term);
	if(ret == -1 || retchar != 0xaa)
		return FALSE;
	return TRUE;
}
/*	
int STAR_Select_WorkKey(term,port,MasterKeyNum,WorkKeyNum)
 功能: 激活单DES密码键盘主密钥号为MasterKeyNum 用户密钥号为WorkKeyNum的用户密钥?
 参数: char MasterKeyNum: 主密钥号
                          0=<MasterKeyNum<=1FH
       char WorkKeyNum: 用户密钥号
			  0=<WorkKeyNum<=07H
 返回：0  OK
      -1 ERR
*/
int STAR_Select_WorkKey(term,port,MasterKeyNum,WorkKeyNum)
char MasterKeyNum,WorkKeyNum;
int term,port;
{
	char stringtmp[10],tmp[4];
	unsigned char retchar;
	int ret;
   memset(stringtmp,0,sizeof(stringtmp));
  memset(tmp,0,sizeof(tmp));

	if(MasterKeyNum < 0 || MasterKeyNum >31)
		return FALSE;
	if(WorkKeyNum < 0 || WorkKeyNum > 7)
		return FALSE;
	if(Star_openPort(term,port) == -1)
		return FALSE;
	tmp[0]=(char)((MasterKeyNum>>4)&0x0f)+0x30;
	tmp[1]=(char)(MasterKeyNum&0x0f)+0x30;
	tmp[2]=(char)((WorkKeyNum>>4)&0x0f)+0x30;
	tmp[3]=(char)(WorkKeyNum&0x0f)+0x30;
	sprintf(stringtmp,"\033A%c%c%c%c\x0d\x0a",tmp[0],tmp[1],tmp[2],tmp[3]);
	stringtmp[8]='\0';
	fflush(stdout);
	Star_dataToDevice(term,stringtmp);
	fflush(stdout);
	ret=Star_readLenStr(&retchar,1,100);
	Star_closePort(term);
	if(ret == -1 || retchar != 0xaa)
		return FALSE;
	else
		return TRUE;
}

/*
//int STAR_DispString(mode,string,timeout)
   功能: 在DES密码键盘液晶上面显示字符串
   参数: int mode : 行号码
   	               0 (第一行)
   	               1 (第二行)
         char *string :显示的字符串，字符串的长度<16。
         int timeout  :超时时间
   返回：0  OK
         -1 ERR
*/
int STAR_DispString(term,port,mode,string,timeout)
int mode,timeout,term,port;
char *string;
{
	char stringtmp[25];
	int len;
  memset(stringtmp,0,sizeof(stringtmp));

	len=strlen(string);
	if(len > 16 || mode < 0 || mode > 1)
		return FALSE;
	if(Star_openPort(term,port) == -1)
		return FALSE;
	sprintf(stringtmp,"\033L%c%d%s\x0d\x0a",mode+0x31,timeout,string);
	len+=5;
	stringtmp[len]='\0';
	fflush(stdout);
	Star_dataToDevice(term,stringtmp);
	fflush(stdout);
	Star_closePort(term);
	return TRUE;
}
/*
int STAR_GetPin_Des(modex,modey,string,strlength,timeout)
  功能:DES密码键盘输入的密码与当前的工作密钥做单DES加密后，结果送到string中
  参数：  
         int modex：密码输入提示方式
   		0     密码键盘提示"请输入密码"，语音提示
   		1     密码键盘提示"请再输入一次"，语音提示
   		2     带语音键盘无语音提示，密码键盘提示"请输入密码"
         int modey：密码输入结束方式
		0     按"确认"键或到达指定的PIN长度
   		1     按"确认"键
   		2     到达指定的PIN长度
   		3     到达指定的PIN长度并按"确认"键
   	char *string: 返回的密文数据
   	int strlength:数据长度
   	int timeout :超时时间
   返回：0  OK
         -1 ERR
   	

*/
int STAR_GetPin_Des(term,port,modex,modey,string,strlength,timeout)
int modex,modey,timeout,*strlength;
char *string;
{
	char stringtmp[10];
	int ret_code;
	memset(stringtmp,0,sizeof(stringtmp));

	if(modex < 0 || modex > 5)
		return FALSE;
	if(modey < 0 || modey > 3)
		return FALSE;
	if(Star_openPort(term,port) == -1)
		return FALSE;
	sprintf(stringtmp,"\033F%c%c\x0d\x0a",modex+0x30,modey+0x30);
	stringtmp[6]='\0';
	fflush(stdout);
	Star_dataToDevice(term,stringtmp);
	fflush(stdout);
	
	ret_code=Star_ReadPinPad(string,strlength,timeout);
	Star_closePort(term);
	if(ret_code == -1)
		return FALSE;
	else
		return TRUE;
}
/*
int STAR_Change_WorkKey(MasterKeyNum,WorkKeyNum,WorkKey)
  功能: 修改单DES密码键盘的用户密钥,
        用户密钥经主密钥加密，单DES密码键盘收到用户密钥后，用指定主密钥号的
        的主密钥解密后存入主密钥号为M，用户密钥号为N的用户密钥区间，
        该方法只须指定主密钥号即可，以避免主密钥在线上传输较不安全.
	    
  参数：int MasterKeyNum: 主密钥号	(0=<mindex<=1FH)
   	    int WorkKeyNum:   用户密钥号 (0=<nindex<=07H)
        char *WorkKey  : 用户密钥
  返回：0  OK
        -1 ERR
          
*/
int STAR_Change_WorkKey(term,port,MasterKeyNum,WorkKeyNum,WorkKey)
char MasterKeyNum,WorkKeyNum;
char *WorkKey;
int term,port;
//int WorkKeylen;
{
    char stringtmp1[100],stringtmp2[100],tmp[4];
	unsigned char retchar;
	int i,ret,keylen;
  memset(stringtmp1,0,sizeof(stringtmp1));
  memset(stringtmp2,0,sizeof(stringtmp2));
  memset(tmp,0,sizeof(tmp));

	if(MasterKeyNum < 0 || MasterKeyNum >31)
		return FALSE;
	if(WorkKeyNum < 0 || WorkKeyNum > 7)
		return FALSE;
	if(Star_openPort(term,port) == -1)
		return FALSE;
	memset(stringtmp1,0,sizeof(stringtmp1));
	memset(stringtmp2,0,sizeof(stringtmp2));
	
	for(i=0;i<8;i++)
	{
		stringtmp2[i*2]=(char)((WorkKey[i]>>4)&0x0f)+0x30;
		stringtmp2[i*2+1]=(char)(WorkKey[i]&0x0f)+0x30;
	}
	stringtmp2[8*2]='\0';
	tmp[0]=(char)((MasterKeyNum>>4)&0x0f)+0x30;
	tmp[1]=(char)(MasterKeyNum&0x0f)+0x30;
	tmp[2]=(char)((WorkKeyNum>>4)&0x0f)+0x30;
	tmp[3]=(char)(WorkKeyNum&0x0f)+0x30;
	sprintf(stringtmp1,"\033S%c%c%c%c%s\x0d\x0a",tmp[0],tmp[1],tmp[2],tmp[3],stringtmp2);
	stringtmp1[6+8*2+2]='\0';
	fflush(stdout);
	Star_dataToDevice(term,stringtmp1);
	fflush(stdout);
	ret=Star_readLenStr(&retchar,1,20);
	Star_closePort(term);
	if(ret == -1 || retchar != 0xaa)
		return FALSE;
	else
		return TRUE;
}
/*
int STAR_Change_WorkKey_3Des(term,port,MasterKeyNum,WorkKeyNum,WorkKey,WorkKeyType)
   功能: 修改3DES密码键盘的用户密钥,
         用户密钥经主密钥加密，3DES密码键盘收到用户密钥后，用指定的主密钥号
         的主密钥解密后存入主密钥号为M，用户密钥号为N的用户密钥区间，
         该方法只须指定主密钥号即可，以避免主密钥在线上传输较不安全.
	    
   参数：int MasterKeyNum: 主密钥号	(0=<MasterKeyNum<=1DH)
    	   int WorkKeyNum:    用户密钥号 (0=<WorkKeyNum<=05H)
         char *WorkKey  : 用户密钥
                    64  bit 方式: char WorkKey[8]; //输入的密钥数组长度.
                    128 bit 方式: char WorkKey[16];
		    192 bit 方式: char WorkKey[24];
         int WorkKeyType:用户密钥类型
                    WorkKeyType = 1 :64  bit DES密钥
		    WorkKeyType = 2 :128 bit DES密钥 
		    WorkKeyType = 3 :192 bit DES密钥

   返回:?  OK
         -1 ERR

*/
int STAR_Change_WorkKey_3Des(term,port,MasterKeyNum,WorkKeyNum,WorkKey,WorkKeyType)
char MasterKeyNum,WorkKeyNum;
char *WorkKey;
int WorkKeyType,term,port;
{
    char stringtmp1[150],stringtmp2[100],tmp[4],stringtmp3[100];
	unsigned char retchar;
	int i,ret,keylen;
  memset(stringtmp1,0,sizeof(stringtmp1));
  memset(stringtmp2,0,sizeof(stringtmp2));
  memset(stringtmp3,0,sizeof(stringtmp3));
  memset(tmp,0,sizeof(tmp));

	if(MasterKeyNum < 0 || MasterKeyNum >9) return FALSE;
	  MasterKeyNum *= 3;  //3DES的主密钥区间为3个字节长度.
  
  if(WorkKeyNum < 0 || WorkKeyNum > 5)
		return FALSE;
	if(Star_openPort(term,port) == -1)
		return FALSE;
	memset(stringtmp1,0,sizeof(stringtmp1));
	memset(stringtmp2,0,sizeof(stringtmp2));
	memset(stringtmp3,0,sizeof(stringtmp3));
	switch(WorkKeyType)
	{
		case 1: //64 bit
			memcpydata(stringtmp3,0,WorkKey,0,8);
			memcpydata(stringtmp3,8,WorkKey,0,8);
			memcpydata(stringtmp3,16,WorkKey,0,8);
			break;
		case 2: //128 bit
			memcpydata(stringtmp3,0,WorkKey,0,8);
			memcpydata(stringtmp3,8,WorkKey,8,8);
			memcpydata(stringtmp3,16,WorkKey,0,8);
			break;
		case 3: //192 bit
			memcpydata(stringtmp3,0,WorkKey,0,24);
			break;
		default:
			return -1;
	}
	for(i=0;i<24;i++)
	{
		stringtmp2[i*2]=(char)((stringtmp3[i]>>4)&0x0f)+0x30;
		stringtmp2[i*2+1]=(char)(stringtmp3[i]&0x0f)+0x30;
	}
	stringtmp2[48]='\0';
	tmp[0]=(char)((MasterKeyNum>>4)&0x0f)+0x30;
	tmp[1]=(char)(MasterKeyNum&0x0f)+0x30;
	tmp[2]=(char)((WorkKeyNum>>4)&0x0f)+0x30;
	tmp[3]=(char)(WorkKeyNum&0x0f)+0x30;
	sprintf(stringtmp1,"\033S%c%c%c%c%s\x0d\x0a",tmp[0],tmp[1],tmp[2],tmp[3],stringtmp2);
	stringtmp1[6+48+2]='\0';
	fflush(stdout);
	Star_dataToDevice(term,stringtmp1);
	fflush(stdout);
	ret=Star_readLenStr(&retchar,1,100);
	Star_closePort(term);
	if(ret == -1 || retchar != 0xaa)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
/*
int STAR_Change_MasterKey(MasterKeyNum,Old_MasterKey,New_MasterKey)
  功能： 修改单DES密码键盘的主密钥

  参数: char *Old_MasterKey: 旧主密钥(8个字节)
		    
        char *New_MasterKey: 新主密钥(8个字节)
		                      
        int MasterKeyNum: 主密钥区     0=<MasterKeyNum<=1DH
  返回：0  OK
        -1 ERR
		    
*/
int STAR_Change_MasterKey(term,port,MasterKeyNum,Old_MasterKey,New_MasterKey)
char MasterKeyNum;
char *Old_MasterKey,*New_MasterKey;
int term,port;
{
	char stringtmp1[45],stringtmp2[17],stringtmp3[17],tmp[4];
	unsigned char retchar;
	int i,ret;
  memset(stringtmp1,0,sizeof(stringtmp1));
  memset(stringtmp2,0,sizeof(stringtmp2));
  memset(stringtmp3,0,sizeof(stringtmp3));
  memset(tmp,0,sizeof(tmp));
	if(MasterKeyNum < 0 || MasterKeyNum >31)
		return FALSE;
	//if(strlen(Old_MasterKey) != 8 || strlen(New_MasterKey) != 8)
	//	return FALSE;
	//复位密码键盘
/*	if(Star_ResetFlag == 0)
	{
	   ret =  STAR_Reset_PinPad(term,port,20);
	   if (ret == -1 ) 		   	 return -1;
	   else   	   	 Star_ResetFlag = 1;	
  }
*/
	if(Star_openPort(term,port) == -1)
		return FALSE;

	for(i=0;i<8;i++)
	{
		stringtmp2[i*2]=(char)((Old_MasterKey[i]>>4)&0x0f)+0x30;
		stringtmp2[i*2+1]=(char)(Old_MasterKey[i]&0x0f)+0x30;
	}
	stringtmp2[16]='\0';
	for(i=0;i<8;i++)
	{
		stringtmp3[i*2]=(char)((New_MasterKey[i]>>4)&0x0f)+0x30;
		stringtmp3[i*2+1]=(char)(New_MasterKey[i]&0x0f)+0x30;
	}
	stringtmp3[16]='\0';
	tmp[0]=(char)((MasterKeyNum>>4)&0x0f)+0x30;
	tmp[1]=(char)(MasterKeyNum&0x0f)+0x30;
	sprintf(stringtmp1,"\033M%c%c%s%s\x0d\x0a",tmp[0],tmp[1],stringtmp2,stringtmp3);
	stringtmp1[38]='\0';
	fflush(stdout);
	Star_dataToDevice(term,stringtmp1);
	fflush(stdout);
	ret=Star_readLenStr(&retchar,1,20);
	Star_closePort(term);
	if(ret == -1 || retchar != 0xaa)
		return FALSE;
	else
		return TRUE;
}
/*
int STAR_Change_MasterKey_3Des(MasterKeyNum,Old_MasterKey,Old_Key_Type,New_MasterKey,New_Key_Type)
    功能: 修改3DES密码键盘主密钥
    参数: char *Old_MasterKey: 旧主密钥
	  int Old_Key_Type: 旧主密钥加密类型
	            Old_Key_Type = 1 :64  bit DES密钥(8 个字节)
	            Old_Key_Type = 2 :128 bit DES密钥(16个字节)
	            Old_Key_Type = 3 :192 bit DES密钥(32个字节)
	  char *New_MasterKey: 新主密钥
	  int New_Key_Type: 新主密钥加密类型
 		    New_Key_Type = 1 :64  bit DES密钥(8 个字节)
		    New_Key_Type = 2 :128 bit DES密钥(16个字节) 
		    New_Key_Type = 3 :192 bit DES密钥(32个字节)
          int MasterKeyNum: 主密钥区     0=<MasterKeyNum<=1DH
    返回：0  OK
          -1 ERR
		  

*/
int STAR_Change_MasterKey_3Des(term,port,MasterKeyNum,Old_MasterKey,Old_Key_Type,New_MasterKey,New_Key_Type)
char MasterKeyNum;
char *Old_MasterKey,*New_MasterKey;
int Old_Key_Type,New_Key_Type,term,port;
{
	char stringtmp1[200],stringtmp2[200],stringtmp3[200],tmp[4],stringtemp4[200],stringtemp6[200];
	unsigned char retchar;
	int i,ret;
  memset(stringtmp1,0,sizeof(stringtmp1));
  memset(stringtmp2,0,sizeof(stringtmp2));
  memset(stringtmp3,0,sizeof(stringtmp3));
  memset(stringtemp4,0,sizeof(stringtemp4));
  memset(stringtemp6,0,sizeof(stringtemp6));
  memset(tmp,0,sizeof(tmp));
	if(MasterKeyNum < 0 || MasterKeyNum >9) return FALSE;
	MasterKeyNum *= 3;  //3DES的主密钥区间为3个字节长度.
	//if(strlen(Old_MasterKey)%8 != 0 || strlen(New_MasterKey)%8 != 0)
	//	return FALSE;
	//复位密码键盘
/*	if(Star_ResetFlag_3DES == 0)
	{
	   ret =  STAR_Reset_PinPad(term,port,200);
	   if (ret == -1 ) 	return -1;
	   else  Star_ResetFlag_3DES = 1;			
     if( STAR_Set3DesMode(term,port) == -1) return -1;
	   	 
  }
*/  
	if(Star_openPort(term,port) == -1)
		return FALSE;
	switch(Old_Key_Type)
	{
		case 1:  //64 bit 密钥
			memcpydata(stringtemp4,0,Old_MasterKey,0,8);
			memcpydata(stringtemp4,8,Old_MasterKey,0,8);
			memcpydata(stringtemp4,16,Old_MasterKey,0,8);
			break;
		case 2: //128 bit
			memcpydata(stringtemp4,0,Old_MasterKey,0,8);
			memcpydata(stringtemp4,8,Old_MasterKey,8,8);
			memcpydata(stringtemp4,16,Old_MasterKey,0,8);
			break;//192 bit
		case 3:
			memcpydata(stringtemp4,0,Old_MasterKey,0,24);
			break;
		default:
			return -1;
	}
	stringtemp4[24]='\0';
	switch(New_Key_Type)
	{
		case 1:  //64 bit 密钥
			memcpydata(stringtemp6,0,New_MasterKey,0,8);
			memcpydata(stringtemp6,8,New_MasterKey,0,8);
			memcpydata(stringtemp6,16,New_MasterKey,0,8);
			break;
		case 2:  //128 bit 密钥
			memcpydata(stringtemp6,0,New_MasterKey,0,8);
			memcpydata(stringtemp6,8,New_MasterKey,8,8);
			memcpydata(stringtemp6,16,New_MasterKey,0,8);
			break;
		case 3:  //192 bit 密钥
			memcpydata(stringtemp6,0,New_MasterKey,0,24);
			break;
		default:
			return -1;

	}
	stringtemp6[24]='\0';
	
	for(i=0;i<24;i++)
	{
		stringtmp2[i*2]=(char)((stringtemp4[i]>>4)&0x0f)+0x30;
		stringtmp2[i*2+1]=(char)(stringtemp4[i]&0x0f)+0x30;
	}
	stringtmp2[48]='\0';

	for(i=0;i<24;i++)
	{
		stringtmp3[i*2]=(char)((stringtemp6[i]>>4)&0x0f)+0x30;
		stringtmp3[i*2+1]=(char)(stringtemp6[i]&0x0f)+0x30;
	}
	stringtmp3[48]='\0';
	tmp[0]=(char)((MasterKeyNum>>4)&0x0f)+0x30;
	tmp[1]=(char)(MasterKeyNum&0x0f)+0x30;
	sprintf(stringtmp1,"\033M%c%c%s%s\x0d\x0a",tmp[0],tmp[1],stringtmp2,stringtmp3);
	stringtmp1[4+48+48+2]='\0';
	fflush(stdout);
	Star_dataToDevice(term,stringtmp1);
	fflush(stdout);
	ret=Star_readLenStr(&retchar,1,20);
	
	Star_closePort(term);
	if(ret == -1 || retchar != 0xaa)
		return FALSE;
	else
		return TRUE;
}
/*  		
int STAR_Set_PinLength(pinLength)
功能: 设置用户输入密码长度
  		参数: int pinlen : 长度
  	     			            6
  		返回:  0: OK
      		  -1: ERR
*/
int STAR_Set_PinLength(term,port,pinLength)
int pinLength,term,port;
{
	char stringtmp[10],tmp[2];
	unsigned char retchar;
	int ret;
  memset(stringtmp,0,sizeof(stringtmp));
  memset(tmp,0,sizeof(tmp));

	tmp[0]=(char)((pinLength>>4)&0x0f)+0x30;
	tmp[1]=(char)(pinLength&0x0f)+0x30;
	sprintf(stringtmp,"\033N%c%c\x0d\x0a",tmp[0],tmp[1]);
	stringtmp[6]='\0';
	if(Star_openPort(term,port) == -1)
		return FALSE;
	fflush(stdout);
	Star_dataToDevice(term,stringtmp);
	fflush(stdout);
	ret=Star_readLenStr(&retchar,1,20);
	Star_closePort(term);
	if(ret == -1 || retchar != 0xaa)
		return FALSE;
	else
		return TRUE;
}

/*
	ret=STAR_Getkey(modex,modey,pinlength,key);
	功能：将单DES密码键盘输入的密码与随机生成的工作密钥做DES加密后，结果经过解密后送到key中
	参数：
              int modex：密码输入提示方式
   		      0     密码键盘提示"请输入密码"，语音提示
   	        1     密码键盘提示"请再输入一次"，语音提示
   		       2     带语音键盘无语音提示，密码键盘提示"请输入密码"
	      int modey：密码输入结束方式
		       0     按"确认"键或到达指定的PIN长度
   		     1     按"确认"键
   		     2     到达指定的PIN长度
   		     3     到达指定的PIN长度并按"确认"键
	      int pinlength:	要输入的密码长度,比如设为4位,如输入的密码不到4位                    ,按确认键,密码键盘
                   不会将数据上传,如输入超过4位,超过部分数据也一起将数据全部上传
        int term: 终端类型
        	       1 实达 2新大陆 3国光 4长城
        int port :终端端口
        	       1 辅口一 2 辅口二 3 辅口三 4 辅口四
	返回：
	    ret=0		成功
	    ret=-1		超时失败,超时定义在term.h中,TIMEOUT 
*/
int STAR_Getkey(term,port,modex,modey,pinlength,key,timeout)
char *key;
int modex,modey,pinlength,timeout,term,port;
{
	char current_work_key[9],WorkKey[9],current_master_key[9],old_master_key[9],card_pin[9];
	  //定义主密钥区和工作密钥区
	char MasterKeyNum=1,WorkKeyNum=3;
	int i,ret;
	time_t t;
	int strlength=0;
  memset(current_work_key,0,sizeof(current_work_key));
  memset(WorkKey,0,sizeof(WorkKey));
  memset(current_master_key,0,sizeof(current_master_key));
  memset(old_master_key,0,sizeof(old_master_key));
  memset(card_pin,0,sizeof(card_pin));
	strcpy(current_master_key,"77777777");
	strcpy(old_master_key,"88888888");
	srand((unsigned) time(&t));
	for(i=0; i<8; i++) current_work_key[i]=rand();
	current_work_key[8]='\0';
  //  复位密码键盘.
  if( STAR_Reset_PinPad(term,port,20) == -1 ) return -1;
    //修改主密钥
  if (STAR_Change_MasterKey(term,port,MasterKeyNum,old_master_key,current_master_key)== -1) return -1;
    //DES加密工作密钥
  //DES(current_master_key,current_work_key,WorkKey);
  TripleDes(current_master_key,8,current_work_key,8,WorkKey);
    //修改工作密钥
	if (STAR_Change_WorkKey(term,port,MasterKeyNum,WorkKeyNum,WorkKey) == -1) return -1;
	  //激活工作密钥区
	if (STAR_Select_WorkKey(term,port,MasterKeyNum,WorkKeyNum) == -1) return -1;
	  //设置密码长度
	if (pinlength != 0)
	{
			if (STAR_Set_PinLength(term,port,pinlength) == -1) return -1;
  }
 	else
	{
			if (STAR_Set_PinLength(term,port,13) == -1) return -1; //设置为13位的长度!
	}	
	//读取密码，密码字符值与当前的工作密钥加密后上送到主机系统
	if (STAR_GetPin_Des(term,port,modex,modey,card_pin,&strlength,timeout) == -1) return -1;
			  //清除液晶屏幕，显示首页

	STAR_ClearLED(term,port);
	if(strlength==0) return 0;
	  //DES解密密码字符值
	//_DES(current_work_key,card_pin,key);
	//Star_WriteLog(card_pin,strlength);
	ret=_TripleDes(current_work_key,8,card_pin,strlength,key);
  
	return ret;
}
/*
	ret=STAR_3DESGetkey(modex,modey,pinlength,key);
	功能：将3DES密码键盘输入的密码与随机生成的工作密钥做3DES加密后，结果经过解密后送到key中
	参数：
	      int modex：密码输入提示方式
   		0     密码键盘提示"请输入密码"，语音提示
   		1     密码键盘提示"请再输入一次"，语音提示
   		2     带语音键盘无语音提示，密码键盘提示"请输入密码"
	      int modey：密码输入结束方式
		0     按"确认"键或到达指定的PIN长度
   		1     按"确认"键
   		2     到达指定的PIN长度
   		3     到达指定的PIN长度并按"确认"键
	      int pinlength	要输入的密码长度,比如设为4位,如输入的密码不到4位                     ,按确认键,密码键盘
	           不会将数据上传,如输入超过4位,超过部分数据也一起将数据全部上传
        int term: 终端类型
        	       1 实达 2新大陆 3国光 4长城
        int port :终端端口
        	       1 辅口一 2 辅口二 3 辅口三 4 辅口四
        返回：
       	   ret=0		成功
	   ret=-1		超时失败,超时定义在term.h中,TIMEOUT 
*/

int STAR_3DESGetkey(term,port,modex,modey,pinlength,key,timeout)
char *key;
int modex,modey,pinlength,timeout,term,port;
{
	char current_work_key[100],WorkKey[100],current_master_key[100],card_pin[20],Old_master_key[100];
	  //定义主密钥区和工作密钥区
	char MasterKeyNum=1,WorkKeyNum=3;
	int i,ret;
	time_t t;
	int strlength=0;
  memset(current_work_key,0,sizeof(current_work_key));
  memset(WorkKey,0,sizeof(WorkKey));
  memset(current_master_key,0,sizeof(current_master_key));
  memset(Old_master_key,0,sizeof(Old_master_key));
  memset(card_pin,0,sizeof(card_pin));
	strcpy(current_master_key,"STAR_3DES_PINPAD");
	current_master_key[16]='\0';
  strcpy(Old_master_key,"8888888888888888");
  Old_master_key[16]='\0';
  //  复位密码键盘.
  if( STAR_Reset_PinPad(term,port,20) == -1 ) return -1;
     //设置3DES密码键盘3DES模式
  if( STAR_Set3DesMode(term,port) == -1) return -1;
     //修改3DES密码键盘的主密钥
  
  if( STAR_Change_MasterKey_3Des(term,port,MasterKeyNum,Old_master_key,2,current_master_key,2) == -1) return -1;
     //生成24字节的随机工作密钥。
	srand((unsigned) time(&t));
	for(i=0; i<24; i++) current_work_key[i]=rand(); 
	current_work_key[24]='\0';
  	   //3DES运算加密工作密钥
	if (TripleDes(current_master_key,16,current_work_key,24,WorkKey)== -1) return -1;
	   //修改工作密钥
	if (STAR_Change_WorkKey_3Des(term,port,MasterKeyNum,WorkKeyNum,WorkKey,3) == -1) return -1;
	   //激活工作密钥区
	//Star_WriteLog("after change workkey",1);
	if (STAR_Select_WorkKey(term,port,MasterKeyNum*3,WorkKeyNum) == -1) return -1;
     //设置密码长度
  if(pinlength != 0)
  {
		if (STAR_Set_PinLength(term,port,pinlength) == -1) return -1;
	}
	else
	{
		if (STAR_Set_PinLength(term,port,13) == -1) return -1;
	}
      //读取密码，密码字符值与当前的工作密码加密后上送到主机系统
	if (STAR_GetPin_Des(term,port,modex,modey,card_pin,&strlength,timeout) == -1) return -1;
      //清除密码键盘液晶，显示首页。
	STAR_ClearLED(term,port);
	if(strlength==0)		return 0;
           //使用工作密钥对上送的密文进行3DES解密，运算出当前的密码。
	//Star_WriteLog(card_pin,strlength);
           
	ret=_TripleDes(current_work_key,24,card_pin,strlength,key);
	return (ret);
	
}

/*
int STAR_GetPin_Ansi98(modex,modey,CARDNO,CARDNOlen,string,pstrlen,timeout)
功能：
      将键入的密码显示在第二行，按“确认”后，将输入的密码PIN和卡号CARDNO按附录A      所提供的算法生成PINBLOCK.
      用当前密钥作DES运算，产生密文,赋值给string.密文长度赋值给pstrlen?
      卡号CARDNO长13位。如果密码键盘没有输入密码而直接按“确认”键退出，
      不进行加密运算而直接送8个0X0上去即可（当然需要拆分上传）
参数: char* string: 输入的密码PIN和卡号CARDNO按附录A所提供的算法生成PINBLOCK
      int pstrlen : PINBLOCK 的长度。
      char* CARDNO   : 卡号数据
      int CARDNOlen   : 卡号长度
                        长度>=13;
      int timeout :超时时间。
 返回：
      ret=0		成功
      ret=-1		超时失败,超时定义在term.h中,TIMEOUT 
		        
*/

int STAR_GetPin_Ansi98(term,port,modex,modey,CARDNO,CARDNOlen,string,pstrlen,timeout)
int modex,modey,CARDNOlen,timeout,*pstrlen,term,port;
char *string,*CARDNO;
{
	char stringtmp[29],CARDNOtmp[49],CARDNO12[13];
	int ret_code,stringlen,i=0,j=0;

	memset(stringtmp,0,sizeof(stringtmp));
	memset(CARDNO12,0,sizeof(CARDNO12));
	memset(CARDNOtmp,0,sizeof(CARDNOtmp));
	if(modex < 0 || modex > 5)
		return FALSE;
	if(modey < 0 || modey > 3)
		return FALSE;
	if(CARDNOlen<=12)
	{
			
		return FALSE;
	}
	else
	{
		for(j=0,i=CARDNOlen-13;i<=CARDNOlen-2;i++)
		{
			CARDNO12[j]=CARDNO[i];
	j++;
		}

//printf("\ntest1:%s",CARDNO12);
	sprintf(stringtmp,"\033I%c%c%s\x0d\x0a",modex+0x30,modey+0x30,CARDNO12);
	}
  if(Star_openPort(term,port) == -1)
	return FALSE;
	Star_dataToDevice(term,stringtmp);
	ret_code=Star_ReadPinPad(string,pstrlen,timeout);
	Star_closePort(term);
	if(ret_code == -1)
		return FALSE;
	else
		return TRUE;
}
/*
int STAR_GetPin_Ansi98_withecho(modex,modey,CARDNO,CARDNOlen,echostr,string,pstrlen,timeout)

功能：将echostr(不拆分)显示在密码键盘液晶显示器第一行的左侧,取消密码键盘提示"请输     入密码",允许输入，
      将键入的密码显示在第二行，按“确认”后，将输入的密码PIN和卡号CARDNO按附录A      所提供的算法生成PINBLOCK，
      用当前密钥作DES运算，产生密文,赋值给string.密文长度赋值给pstrlen。
      卡号CARDNO长13位。如果密码键盘没有输入密码而直接按“确认”键退出，
      不进行加密运算而直接送8个0X0上去即可（当然需要拆分上传）
参数: char* string: 输入的密码PIN和卡号CARDNO按附录A所提供的算法生成PINBLOCK
      int pstrlen : PINBLOCK 的长度。
      char * echostr: 密码键盘显示的字符串。
      char* CARDNO   : 卡号数据
      int CARDNOlen   : 卡号长度
	                   长度>=13;
      int timeout :超时时间。
返回：
      ret=0		成功
      ret=-1		超时失败,超时定义在term.h中,TIMEOUT 
*/
int STAR_GetPin_Ansi98_withecho(term,port,modex,modey,CARDNO,CARDNOlen,echostr,string,pstrlen,timeout)
int modex,modey,CARDNOlen,timeout,*pstrlen,term,port;
char *string,*CARDNO,*echostr;
{
	char stringtmp[50],CARDNOtmp[49],CARDNO12[11];
	int ret_code,stringlen,i=0,j=0;

	memset(stringtmp,0,sizeof(stringtmp));
	memset(CARDNOtmp,0,sizeof(CARDNOtmp));
	memset(CARDNO12,0,sizeof(CARDNO12));
	if(modex < 0 || modex > 5)
		return FALSE;
	if(modey < 0 || modey > 3)
		return FALSE;
	if(CARDNOlen<=12)
	{
		sprintf(stringtmp,"\033J%c%c%s%s\x0d\x0a",modex+0x30,modey+0x30,CARDNO,echostr);
	}
	else
	{
		for(j=0,i=CARDNOlen-13;i<=CARDNOlen-2;i++)
		{
			CARDNO12[j]=CARDNO[i];
	j++;
		}
	sprintf(stringtmp,"\033J%c%c%s%s\x0d\x0a",modex+0x30,modey+0x30,CARDNO12,echostr);
	}
  if(Star_openPort(term,port) == -1)
	return FALSE;
	Star_dataToDevice(term,stringtmp);
	ret_code=Star_ReadPinPad(string,pstrlen,timeout);
	Star_closePort(term);
	if(ret_code == -1)
		return FALSE;
	else
		return TRUE;
}

/*
int _ANSI98(CARDNO,CARDNOlen,string,key)
功能 ：ANSI98逆运算
参数： char *CARDNO:		基本账户号(PAN).
       int  *CARDNOlen:	基本账户号的初始长度.
       char *string:		返回的PIN-BLOCK.
       char *key		获取的持卡人密码(PIN).
返回： -1 :参数错误。
	     >0 :密码长度。
*/
int _ANSI98(CARDNO,CARDNOlen,string,key)
char *CARDNO,*string,*key;
{
	unsigned char ch,CARDNOresult[15],keyresult[15],CARDNOcomp[7],keycomp[7];
	int  i,j=0,keylen;
	CARDNOresult[0]=CARDNOresult[1]=CARDNOresult[2]=CARDNOresult[3]=0x30;
	if(CARDNOlen<=12)
	{
		return FALSE;
	}
	else
	{
		for(j=0,i=CARDNOlen-13;i<=CARDNOlen-2;i++)
		{
			CARDNOresult[j+4]=CARDNO[i];
			j++;
		}
	}
	for(i=0;i<16;i++)
	{
		CARDNOresult[i]=CARDNOresult[i]-0x30;
	}
	for(i=0;i<8;i++)
	{
	CARDNOcomp[i]=(CARDNOresult[2*i]<<4)+(CARDNOresult[2*i+1]&0x0f);
	}
	for(i=0;i<8;i++)
	{
		keycomp[i]=CARDNOcomp[i]^string[i];
	}
	for(i=0;i<8;i++)
	{
	keyresult[2*i]=(keycomp[i]>>4)+0x30;
		
	keyresult[2*i+1]=(keycomp[i]&0x0f)+0x30;
	
		if(keyresult[2*i]==0x3f||keyresult[2*i+1]==0x3f)
			break;
}
keylen=keyresult[1]-48;
	if(keyresult[0]!=0x30)
		return FALSE;
for(j=0,i=2;j<keylen;j++)
	{
		key[j]=keyresult[i];
	i++;	
	}
	key[j]=0x0;
	return keylen;
}
/*
int STAR_Get_Cryptograph(string,ls,retstring,timeout)
   功能: 将字符串由主系统送入密码键盘内，和当前工作密钥一同作DES的运算，产生密文         ，送入主系统
   参数: char *string: 小等于16个字节的字符串
         int slen    : 字符串长度
	               长度为8和16,不足8和16位的数据使用0x00填充.
         char *retstring: 加密后的字符串
	 int   timeout  :超时时间
	          	
   返回:  0: OK
  	 -1: 超时或读密码错误.

*/
int STAR_Get_Cryptograph(term,port,string,ls,retstring,timeout)
int ls,timeout,term,port;
char *string,*retstring;
{

	int *pstrlen,ret_code;	
	char strtmp[50];
	if(ls>31) return FALSE;
	memset(strtmp,0,sizeof(strtmp));
	sprintf(strtmp,"\033H%s\x0d\x0a",string);
  if(Star_openPort(term,port) == -1)
	return FALSE;
	Star_dataToDevice(term,strtmp);
	ret_code=Star_ReadPinPad(retstring,pstrlen,timeout);
	Star_closePort(term);
	if(ret_code == -1)
		return FALSE;
	else
		return TRUE;
}
/*
int STAR_pinpadReadPin(term,port,read_prompt,pin,timeout)
   功能:读取明码密码键盘函数
   参数:   
   	    int term: 终端类型
        	       1 实达 2新大陆 3国光 4长城
        int port :终端端口
        	       1 辅口一 2 辅口二 3 辅口三 4 辅口四
        int read_prompt: 密码键盘提示方式
        	               1 绿灯亮读密码
        	               2 红灯亮读密码
        char *pin : 读取的密码
        int timeout :超时时间
   返回:
   	    0  正确
   	    -1 错误	               
        
*/
int STAR_pinpadReadPin(term,port,read_prompt,pin,timeout)
int read_prompt,term,port;
char *pin;
int timeout;
{
	int i,j,ret_code;
	char ch;
	ret_code=Star_openPort(term,port);
	if (ret_code==-1) return -1;

	switch(read_prompt)
	{
	  case 1:
			Star_dataToDevice(term,GREEN_ON);
			break;
		case 2:
			Star_dataToDevice(term,RED_ON);
			break;
		default:
			Star_dataToDevice(term,GREEN_ON);
			break;
	}
	i=0;
	do
	{
		ret_code=Star_readChar(&ch,timeout);
		if (ch==ABORTCHAR) 
		{
			ret_code==-1;
		}
		if (ret_code!=0) break;
		if (ch==',') i=0;
		else
                {
                        if((ch>='0')&&(ch<='9'))
                        pin[i++]=ch;
                }
	}while(ch!=0xd);
	pin[i]=0;
	Star_dataToDevice(term,ALL_OFF);
	Star_closePort(term);
	return ret_code;
}
/***************************************************
int STAR_GetDesKey(term,port,destype,in_length,mkeyid,wkeyid,workkey,account,encryptedpin,timeout)
 ** 功能描述：ANSI9.8方式读取密码,键盘输入的密码与卡号进行ANSI9.8运算后,与但前的工作密钥加密后
              形成密文上送到主机系统.
  	          单DES密码键盘主密钥区间(0-31)32个,工作密钥区间(0-7)8个
  	          3DES密码键盘主密钥区间(0-9)10个,工作密钥区间(0-5)6个
              (此函数适用于加密机通讯)
 ** 输入参数：
              (1) int term:  终端类型   1 实达,2 新大陆,3 国光,4 湘计  
              (2) int port:  终端辅口： 1:辅口1 .......	4:辅口4
              (3) int destype: 加密密码键盘类型. 
         	                     1表示单DES密码键盘,
         	                     3表示 3DES密码键盘.
             
              (4) int in_length: pin长度限制  !0 表示限制的长度,0 表示4-12位之间. 		
              (5) int mkeyid: 主密钥号   
              (6) int wkeyid: 工作密钥号 
              (7) char *workkey: 工作密钥密文(拆分后的密钥格式,BCDTOASC) 
              	                 支持识别16BYTE,32BYTE,48BYTE模式.  
              (8) char *account: 卡号          
              (9) int timeout: 超时时间 
              
 ** 输出参数：(10) char *encryptedpin : 输出的密码密文(16个字节,BCDTOASC,符合加密机接口规范)
 ** 返 回 值：
               0  成功;
              -1  失败;
 ***************************************************/
int STAR_GetDesKey(term,port,destype,in_length,mkeyid,wkeyid,workkey,account,encryptedpin,timeout)
char *workkey, *account, *encryptedpin;
int in_length,timeout,term,port,mkeyid,wkeyid,destype;
{
    int ret_code,i,len_star=0;
    char tempin[100],tempin2[100],wkeyzone = 0;
    memset(tempin,0,sizeof(tempin));
    memset(tempin2,0,sizeof(tempin2));
 	  Star_ascToBcd((unsigned char *)tempin2,(unsigned char *)workkey,strlen(workkey),0);        
     
      //修改指定工作密钥区间的工作密钥
   	if (destype == 1) //单DES  
   	     ret_code = STAR_Change_WorkKey(term,port,mkeyid,wkeyid,tempin2);
   	else     //3DES  
   	{
        if ( STAR_Set3DesMode(term,port) == -1) return -1;
        
        ret_code = STAR_Change_WorkKey_3Des(term,port,mkeyid,wkeyid,tempin2,strlen(workkey)/16);
    }
    if (ret_code == -1)  return -1;
    
      //激活工作密钥区间
    if (destype == 1)  ret_code = STAR_Select_WorkKey(term,port,mkeyid,wkeyid);
    else 
    {
    	  if (mkeyid >= 10 || wkeyid >= 6) return -2;
    	  else ret_code = STAR_Select_WorkKey(term,port,mkeyid*3,wkeyid);
    }
    if (ret_code == -1)  return -1;
    
    if (in_length != 0)  
    {  //设置密码长度
       ret_code = STAR_Set_PinLength(term,port,in_length); 
       if (ret_code == -1) return -1;  
         //ANSI98 读取密码;
       ret_code = STAR_GetPin_Ansi98(term,port,0,3,account,strlen(account),tempin,&len_star,timeout);           
    }
    else  
    {
       ret_code = STAR_Set_PinLength(term,port,13); 
       if (ret_code == -1) return -1;  
         //ANSI98 读取密码;
    	ret_code = STAR_GetPin_Ansi98(term,port,0,1,account,strlen(account),tempin,&len_star,timeout);    
    }
    if (ret_code == -1) 	
    { 
     	 STAR_ClearLED(term,port);
       return -1;
    }
    Star_bcdToAsc((unsigned char *)encryptedpin,(unsigned char *)tempin,len_star,0);
    encryptedpin[len_star*2]='\0';
    return 0;    
}

/*int Star_DownMasterDesKey(int term,int port,int mkeyid,char *mkey)
  功能: 下载主密钥(适用于加密机通讯)
  	     单DES密码键盘主密钥区间(0-31)32个,工作密钥区间(0-7)8个
  	     3DES密码键盘主密钥区间(0-9)10个,工作密钥区间(0-5)6个
  参数:
  	     (1)int term: 终端类型 1 实达 2新大陆 3国光 4长城
         (2)int port: 终端端口  1 辅口一 2 辅口二 3 辅口三 4 辅口四
         (3)int destype : 加密密码键盘类型.
         	                1表示单DES密码键盘,
         	                3表示  3DES密码键盘.
         (4)int mkeyid :主密钥号
         (5)char *meky   :主密钥(拆分后的密钥格式,BCDTOASC)
         	                支持识别16BYTE,32BYTE,48BYTE模式
  返回    
          0：成功  
          -1：失败       
*/
int STAR_DownMasterDesKey(int term,int port,int destype,int mkeyid,char *mkey)
{
        
        int ret_code;
        char old_master_key[9],tempin[50];        
        memset(old_master_key,0,sizeof(old_master_key));
        memset(tempin,0,sizeof(tempin));
        strcpy(old_master_key,"88888888");
        
    	  Star_ascToBcd((unsigned char *)tempin,(unsigned char *)mkey,strlen(mkey),0);        
    	  if (destype == 1) //单DES 
            ret_code = STAR_Change_MasterKey(term,port,mkeyid,old_master_key,tempin);
        else     //3DES
        {
        	       	 
             ret_code = STAR_Change_MasterKey_3Des(term,port,mkeyid,old_master_key,1,tempin,strlen(mkey)/16);
        }
        return ret_code;
       
}

