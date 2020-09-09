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
   ����3DES�������3DESģʽ����
   ���أ�0 ��ȷ
         ��1 ����
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
    ���DES�������Һ����Ļ����ʾ��ҳ
  ���أ�0
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
     ����: ��λDES������̽�������̸�λ�ɳ���״̬��
     ��������Կ���ó�88888888���û���Կ���00000000,��������Կ��Ϊ0�û���Կ��Ϊ0���û���Կ��
   	  
     ������int timeouts :��ʱʱ��    
     ���أ�0  OK
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
 ����: ���DES�����������Կ��ΪMasterKeyNum �û���Կ��ΪWorkKeyNum���û���Կ?
 ����: char MasterKeyNum: ����Կ��
                          0=<MasterKeyNum<=1FH
       char WorkKeyNum: �û���Կ��
			  0=<WorkKeyNum<=07H
 ���أ�0  OK
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
   ����: ��DES�������Һ��������ʾ�ַ���
   ����: int mode : �к���
   	               0 (��һ��)
   	               1 (�ڶ���)
         char *string :��ʾ���ַ������ַ����ĳ���<16��
         int timeout  :��ʱʱ��
   ���أ�0  OK
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
  ����:DES�����������������뵱ǰ�Ĺ�����Կ����DES���ܺ󣬽���͵�string��
  ������  
         int modex������������ʾ��ʽ
   		0     ���������ʾ"����������"��������ʾ
   		1     ���������ʾ"��������һ��"��������ʾ
   		2     ������������������ʾ�����������ʾ"����������"
         int modey���������������ʽ
		0     ��"ȷ��"���򵽴�ָ����PIN����
   		1     ��"ȷ��"��
   		2     ����ָ����PIN����
   		3     ����ָ����PIN���Ȳ���"ȷ��"��
   	char *string: ���ص���������
   	int strlength:���ݳ���
   	int timeout :��ʱʱ��
   ���أ�0  OK
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
  ����: �޸ĵ�DES������̵��û���Կ,
        �û���Կ������Կ���ܣ���DES��������յ��û���Կ����ָ������Կ�ŵ�
        ������Կ���ܺ��������Կ��ΪM���û���Կ��ΪN���û���Կ���䣬
        �÷���ֻ��ָ������Կ�ż��ɣ��Ա�������Կ�����ϴ���ϲ���ȫ.
	    
  ������int MasterKeyNum: ����Կ��	(0=<mindex<=1FH)
   	    int WorkKeyNum:   �û���Կ�� (0=<nindex<=07H)
        char *WorkKey  : �û���Կ
  ���أ�0  OK
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
   ����: �޸�3DES������̵��û���Կ,
         �û���Կ������Կ���ܣ�3DES��������յ��û���Կ����ָ��������Կ��
         ������Կ���ܺ��������Կ��ΪM���û���Կ��ΪN���û���Կ���䣬
         �÷���ֻ��ָ������Կ�ż��ɣ��Ա�������Կ�����ϴ���ϲ���ȫ.
	    
   ������int MasterKeyNum: ����Կ��	(0=<MasterKeyNum<=1DH)
    	   int WorkKeyNum:    �û���Կ�� (0=<WorkKeyNum<=05H)
         char *WorkKey  : �û���Կ
                    64  bit ��ʽ: char WorkKey[8]; //�������Կ���鳤��.
                    128 bit ��ʽ: char WorkKey[16];
		    192 bit ��ʽ: char WorkKey[24];
         int WorkKeyType:�û���Կ����
                    WorkKeyType = 1 :64  bit DES��Կ
		    WorkKeyType = 2 :128 bit DES��Կ 
		    WorkKeyType = 3 :192 bit DES��Կ

   ����:?  OK
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
	  MasterKeyNum *= 3;  //3DES������Կ����Ϊ3���ֽڳ���.
  
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
  ���ܣ� �޸ĵ�DES������̵�����Կ

  ����: char *Old_MasterKey: ������Կ(8���ֽ�)
		    
        char *New_MasterKey: ������Կ(8���ֽ�)
		                      
        int MasterKeyNum: ����Կ��     0=<MasterKeyNum<=1DH
  ���أ�0  OK
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
	//��λ�������
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
    ����: �޸�3DES�����������Կ
    ����: char *Old_MasterKey: ������Կ
	  int Old_Key_Type: ������Կ��������
	            Old_Key_Type = 1 :64  bit DES��Կ(8 ���ֽ�)
	            Old_Key_Type = 2 :128 bit DES��Կ(16���ֽ�)
	            Old_Key_Type = 3 :192 bit DES��Կ(32���ֽ�)
	  char *New_MasterKey: ������Կ
	  int New_Key_Type: ������Կ��������
 		    New_Key_Type = 1 :64  bit DES��Կ(8 ���ֽ�)
		    New_Key_Type = 2 :128 bit DES��Կ(16���ֽ�) 
		    New_Key_Type = 3 :192 bit DES��Կ(32���ֽ�)
          int MasterKeyNum: ����Կ��     0=<MasterKeyNum<=1DH
    ���أ�0  OK
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
	MasterKeyNum *= 3;  //3DES������Կ����Ϊ3���ֽڳ���.
	//if(strlen(Old_MasterKey)%8 != 0 || strlen(New_MasterKey)%8 != 0)
	//	return FALSE;
	//��λ�������
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
		case 1:  //64 bit ��Կ
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
		case 1:  //64 bit ��Կ
			memcpydata(stringtemp6,0,New_MasterKey,0,8);
			memcpydata(stringtemp6,8,New_MasterKey,0,8);
			memcpydata(stringtemp6,16,New_MasterKey,0,8);
			break;
		case 2:  //128 bit ��Կ
			memcpydata(stringtemp6,0,New_MasterKey,0,8);
			memcpydata(stringtemp6,8,New_MasterKey,8,8);
			memcpydata(stringtemp6,16,New_MasterKey,0,8);
			break;
		case 3:  //192 bit ��Կ
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
����: �����û��������볤��
  		����: int pinlen : ����
  	     			            6
  		����:  0: OK
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
	���ܣ�����DES������������������������ɵĹ�����Կ��DES���ܺ󣬽���������ܺ��͵�key��
	������
              int modex������������ʾ��ʽ
   		      0     ���������ʾ"����������"��������ʾ
   	        1     ���������ʾ"��������һ��"��������ʾ
   		       2     ������������������ʾ�����������ʾ"����������"
	      int modey���������������ʽ
		       0     ��"ȷ��"���򵽴�ָ����PIN����
   		     1     ��"ȷ��"��
   		     2     ����ָ����PIN����
   		     3     ����ָ����PIN���Ȳ���"ȷ��"��
	      int pinlength:	Ҫ��������볤��,������Ϊ4λ,����������벻��4λ                    ,��ȷ�ϼ�,�������
                   ���Ὣ�����ϴ�,�����볬��4λ,������������Ҳһ������ȫ���ϴ�
        int term: �ն�����
        	       1 ʵ�� 2�´�½ 3���� 4����
        int port :�ն˶˿�
        	       1 ����һ 2 ���ڶ� 3 ������ 4 ������
	���أ�
	    ret=0		�ɹ�
	    ret=-1		��ʱʧ��,��ʱ������term.h��,TIMEOUT 
*/
int STAR_Getkey(term,port,modex,modey,pinlength,key,timeout)
char *key;
int modex,modey,pinlength,timeout,term,port;
{
	char current_work_key[9],WorkKey[9],current_master_key[9],old_master_key[9],card_pin[9];
	  //��������Կ���͹�����Կ��
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
  //  ��λ�������.
  if( STAR_Reset_PinPad(term,port,20) == -1 ) return -1;
    //�޸�����Կ
  if (STAR_Change_MasterKey(term,port,MasterKeyNum,old_master_key,current_master_key)== -1) return -1;
    //DES���ܹ�����Կ
  //DES(current_master_key,current_work_key,WorkKey);
  TripleDes(current_master_key,8,current_work_key,8,WorkKey);
    //�޸Ĺ�����Կ
	if (STAR_Change_WorkKey(term,port,MasterKeyNum,WorkKeyNum,WorkKey) == -1) return -1;
	  //�������Կ��
	if (STAR_Select_WorkKey(term,port,MasterKeyNum,WorkKeyNum) == -1) return -1;
	  //�������볤��
	if (pinlength != 0)
	{
			if (STAR_Set_PinLength(term,port,pinlength) == -1) return -1;
  }
 	else
	{
			if (STAR_Set_PinLength(term,port,13) == -1) return -1; //����Ϊ13λ�ĳ���!
	}	
	//��ȡ���룬�����ַ�ֵ�뵱ǰ�Ĺ�����Կ���ܺ����͵�����ϵͳ
	if (STAR_GetPin_Des(term,port,modex,modey,card_pin,&strlength,timeout) == -1) return -1;
			  //���Һ����Ļ����ʾ��ҳ

	STAR_ClearLED(term,port);
	if(strlength==0) return 0;
	  //DES���������ַ�ֵ
	//_DES(current_work_key,card_pin,key);
	//Star_WriteLog(card_pin,strlength);
	ret=_TripleDes(current_work_key,8,card_pin,strlength,key);
  
	return ret;
}
/*
	ret=STAR_3DESGetkey(modex,modey,pinlength,key);
	���ܣ���3DES������������������������ɵĹ�����Կ��3DES���ܺ󣬽���������ܺ��͵�key��
	������
	      int modex������������ʾ��ʽ
   		0     ���������ʾ"����������"��������ʾ
   		1     ���������ʾ"��������һ��"��������ʾ
   		2     ������������������ʾ�����������ʾ"����������"
	      int modey���������������ʽ
		0     ��"ȷ��"���򵽴�ָ����PIN����
   		1     ��"ȷ��"��
   		2     ����ָ����PIN����
   		3     ����ָ����PIN���Ȳ���"ȷ��"��
	      int pinlength	Ҫ��������볤��,������Ϊ4λ,����������벻��4λ                     ,��ȷ�ϼ�,�������
	           ���Ὣ�����ϴ�,�����볬��4λ,������������Ҳһ������ȫ���ϴ�
        int term: �ն�����
        	       1 ʵ�� 2�´�½ 3���� 4����
        int port :�ն˶˿�
        	       1 ����һ 2 ���ڶ� 3 ������ 4 ������
        ���أ�
       	   ret=0		�ɹ�
	   ret=-1		��ʱʧ��,��ʱ������term.h��,TIMEOUT 
*/

int STAR_3DESGetkey(term,port,modex,modey,pinlength,key,timeout)
char *key;
int modex,modey,pinlength,timeout,term,port;
{
	char current_work_key[100],WorkKey[100],current_master_key[100],card_pin[20],Old_master_key[100];
	  //��������Կ���͹�����Կ��
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
  //  ��λ�������.
  if( STAR_Reset_PinPad(term,port,20) == -1 ) return -1;
     //����3DES�������3DESģʽ
  if( STAR_Set3DesMode(term,port) == -1) return -1;
     //�޸�3DES������̵�����Կ
  
  if( STAR_Change_MasterKey_3Des(term,port,MasterKeyNum,Old_master_key,2,current_master_key,2) == -1) return -1;
     //����24�ֽڵ����������Կ��
	srand((unsigned) time(&t));
	for(i=0; i<24; i++) current_work_key[i]=rand(); 
	current_work_key[24]='\0';
  	   //3DES������ܹ�����Կ
	if (TripleDes(current_master_key,16,current_work_key,24,WorkKey)== -1) return -1;
	   //�޸Ĺ�����Կ
	if (STAR_Change_WorkKey_3Des(term,port,MasterKeyNum,WorkKeyNum,WorkKey,3) == -1) return -1;
	   //�������Կ��
	//Star_WriteLog("after change workkey",1);
	if (STAR_Select_WorkKey(term,port,MasterKeyNum*3,WorkKeyNum) == -1) return -1;
     //�������볤��
  if(pinlength != 0)
  {
		if (STAR_Set_PinLength(term,port,pinlength) == -1) return -1;
	}
	else
	{
		if (STAR_Set_PinLength(term,port,13) == -1) return -1;
	}
      //��ȡ���룬�����ַ�ֵ�뵱ǰ�Ĺ���������ܺ����͵�����ϵͳ
	if (STAR_GetPin_Des(term,port,modex,modey,card_pin,&strlength,timeout) == -1) return -1;
      //����������Һ������ʾ��ҳ��
	STAR_ClearLED(term,port);
	if(strlength==0)		return 0;
           //ʹ�ù�����Կ�����͵����Ľ���3DES���ܣ��������ǰ�����롣
	//Star_WriteLog(card_pin,strlength);
           
	ret=_TripleDes(current_work_key,24,card_pin,strlength,key);
	return (ret);
	
}

/*
int STAR_GetPin_Ansi98(modex,modey,CARDNO,CARDNOlen,string,pstrlen,timeout)
���ܣ�
      �������������ʾ�ڵڶ��У�����ȷ�ϡ��󣬽����������PIN�Ϳ���CARDNO����¼A      ���ṩ���㷨����PINBLOCK.
      �õ�ǰ��Կ��DES���㣬��������,��ֵ��string.���ĳ��ȸ�ֵ��pstrlen?
      ����CARDNO��13λ������������û�����������ֱ�Ӱ���ȷ�ϡ����˳���
      �����м��������ֱ����8��0X0��ȥ���ɣ���Ȼ��Ҫ����ϴ���
����: char* string: ���������PIN�Ϳ���CARDNO����¼A���ṩ���㷨����PINBLOCK
      int pstrlen : PINBLOCK �ĳ��ȡ�
      char* CARDNO   : ��������
      int CARDNOlen   : ���ų���
                        ����>=13;
      int timeout :��ʱʱ�䡣
 ���أ�
      ret=0		�ɹ�
      ret=-1		��ʱʧ��,��ʱ������term.h��,TIMEOUT 
		        
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

���ܣ���echostr(�����)��ʾ���������Һ����ʾ����һ�е����,ȡ�����������ʾ"����     ������",�������룬
      �������������ʾ�ڵڶ��У�����ȷ�ϡ��󣬽����������PIN�Ϳ���CARDNO����¼A      ���ṩ���㷨����PINBLOCK��
      �õ�ǰ��Կ��DES���㣬��������,��ֵ��string.���ĳ��ȸ�ֵ��pstrlen��
      ����CARDNO��13λ������������û�����������ֱ�Ӱ���ȷ�ϡ����˳���
      �����м��������ֱ����8��0X0��ȥ���ɣ���Ȼ��Ҫ����ϴ���
����: char* string: ���������PIN�Ϳ���CARDNO����¼A���ṩ���㷨����PINBLOCK
      int pstrlen : PINBLOCK �ĳ��ȡ�
      char * echostr: ���������ʾ���ַ�����
      char* CARDNO   : ��������
      int CARDNOlen   : ���ų���
	                   ����>=13;
      int timeout :��ʱʱ�䡣
���أ�
      ret=0		�ɹ�
      ret=-1		��ʱʧ��,��ʱ������term.h��,TIMEOUT 
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
���� ��ANSI98������
������ char *CARDNO:		�����˻���(PAN).
       int  *CARDNOlen:	�����˻��ŵĳ�ʼ����.
       char *string:		���ص�PIN-BLOCK.
       char *key		��ȡ�ĳֿ�������(PIN).
���أ� -1 :��������
	     >0 :���볤�ȡ�
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
   ����: ���ַ�������ϵͳ������������ڣ��͵�ǰ������Կһͬ��DES�����㣬��������         ��������ϵͳ
   ����: char *string: С����16���ֽڵ��ַ���
         int slen    : �ַ�������
	               ����Ϊ8��16,����8��16λ������ʹ��0x00���.
         char *retstring: ���ܺ���ַ���
	 int   timeout  :��ʱʱ��
	          	
   ����:  0: OK
  	 -1: ��ʱ����������.

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
   ����:��ȡ����������̺���
   ����:   
   	    int term: �ն�����
        	       1 ʵ�� 2�´�½ 3���� 4����
        int port :�ն˶˿�
        	       1 ����һ 2 ���ڶ� 3 ������ 4 ������
        int read_prompt: ���������ʾ��ʽ
        	               1 �̵���������
        	               2 �����������
        char *pin : ��ȡ������
        int timeout :��ʱʱ��
   ����:
   	    0  ��ȷ
   	    -1 ����	               
        
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
 ** ����������ANSI9.8��ʽ��ȡ����,��������������뿨�Ž���ANSI9.8�����,�뵫ǰ�Ĺ�����Կ���ܺ�
              �γ��������͵�����ϵͳ.
  	          ��DES�����������Կ����(0-31)32��,������Կ����(0-7)8��
  	          3DES�����������Կ����(0-9)10��,������Կ����(0-5)6��
              (�˺��������ڼ��ܻ�ͨѶ)
 ** ���������
              (1) int term:  �ն�����   1 ʵ��,2 �´�½,3 ����,4 ���  
              (2) int port:  �ն˸��ڣ� 1:����1 .......	4:����4
              (3) int destype: ���������������. 
         	                     1��ʾ��DES�������,
         	                     3��ʾ 3DES�������.
             
              (4) int in_length: pin��������  !0 ��ʾ���Ƶĳ���,0 ��ʾ4-12λ֮��. 		
              (5) int mkeyid: ����Կ��   
              (6) int wkeyid: ������Կ�� 
              (7) char *workkey: ������Կ����(��ֺ����Կ��ʽ,BCDTOASC) 
              	                 ֧��ʶ��16BYTE,32BYTE,48BYTEģʽ.  
              (8) char *account: ����          
              (9) int timeout: ��ʱʱ�� 
              
 ** ���������(10) char *encryptedpin : �������������(16���ֽ�,BCDTOASC,���ϼ��ܻ��ӿڹ淶)
 ** �� �� ֵ��
               0  �ɹ�;
              -1  ʧ��;
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
     
      //�޸�ָ��������Կ����Ĺ�����Կ
   	if (destype == 1) //��DES  
   	     ret_code = STAR_Change_WorkKey(term,port,mkeyid,wkeyid,tempin2);
   	else     //3DES  
   	{
        if ( STAR_Set3DesMode(term,port) == -1) return -1;
        
        ret_code = STAR_Change_WorkKey_3Des(term,port,mkeyid,wkeyid,tempin2,strlen(workkey)/16);
    }
    if (ret_code == -1)  return -1;
    
      //�������Կ����
    if (destype == 1)  ret_code = STAR_Select_WorkKey(term,port,mkeyid,wkeyid);
    else 
    {
    	  if (mkeyid >= 10 || wkeyid >= 6) return -2;
    	  else ret_code = STAR_Select_WorkKey(term,port,mkeyid*3,wkeyid);
    }
    if (ret_code == -1)  return -1;
    
    if (in_length != 0)  
    {  //�������볤��
       ret_code = STAR_Set_PinLength(term,port,in_length); 
       if (ret_code == -1) return -1;  
         //ANSI98 ��ȡ����;
       ret_code = STAR_GetPin_Ansi98(term,port,0,3,account,strlen(account),tempin,&len_star,timeout);           
    }
    else  
    {
       ret_code = STAR_Set_PinLength(term,port,13); 
       if (ret_code == -1) return -1;  
         //ANSI98 ��ȡ����;
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
  ����: ��������Կ(�����ڼ��ܻ�ͨѶ)
  	     ��DES�����������Կ����(0-31)32��,������Կ����(0-7)8��
  	     3DES�����������Կ����(0-9)10��,������Կ����(0-5)6��
  ����:
  	     (1)int term: �ն����� 1 ʵ�� 2�´�½ 3���� 4����
         (2)int port: �ն˶˿�  1 ����һ 2 ���ڶ� 3 ������ 4 ������
         (3)int destype : ���������������.
         	                1��ʾ��DES�������,
         	                3��ʾ  3DES�������.
         (4)int mkeyid :����Կ��
         (5)char *meky   :����Կ(��ֺ����Կ��ʽ,BCDTOASC)
         	                ֧��ʶ��16BYTE,32BYTE,48BYTEģʽ
  ����    
          0���ɹ�  
          -1��ʧ��       
*/
int STAR_DownMasterDesKey(int term,int port,int destype,int mkeyid,char *mkey)
{
        
        int ret_code;
        char old_master_key[9],tempin[50];        
        memset(old_master_key,0,sizeof(old_master_key));
        memset(tempin,0,sizeof(tempin));
        strcpy(old_master_key,"88888888");
        
    	  Star_ascToBcd((unsigned char *)tempin,(unsigned char *)mkey,strlen(mkey),0);        
    	  if (destype == 1) //��DES 
            ret_code = STAR_Change_MasterKey(term,port,mkeyid,old_master_key,tempin);
        else     //3DES
        {
        	       	 
             ret_code = STAR_Change_MasterKey_3Des(term,port,mkeyid,old_master_key,1,tempin,strlen(mkey)/16);
        }
        return ret_code;
       
}

