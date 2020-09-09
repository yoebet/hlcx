/*
	ST102IC.C
	STAR DRIVERS PUBLIC PROGRAM FOR UNIX
	AUTHOR:		Wind
	VERSION:	V1.0
	DATE: 		1998.09.06
	Copyright (c) 1998-2000 START Group Co. Ltd.
   	All rights reserved.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/fcntl.h>
#include <sys/termios.h>

#include "st102ic.h"


int HexToAsciibuf(number,asciibuf)
unsigned char number;
unsigned char *asciibuf;
{
	unsigned char num;

	num=number>>4;
	if (num<10) asciibuf[0] = num + 0x30;
	else	asciibuf[0] = num + 'A' - 10;

	num=number & 0xf;
	if (num<10) asciibuf[1] = num + 0x30;
	else	asciibuf[1] = num + 'A' - 10;

	return 0;
}
unsigned char TwoAsciiToHex(ascii0,ascii1)
unsigned char ascii0,ascii1;
{
	unsigned char number=0;

	if ((ascii0<0x3a)&&(ascii0>0x2f))
	number = ascii0 - 0x30;
	else if ((ascii0>0x40)&&(ascii0<='F'))
		number = ascii0 - 0x41 + 10;
//lbq 2003 08 20
	else if ((ascii0>0x60)&&(ascii0<='f'))
		number = ascii0 - 0x61 + 10;
	number<<=4;
	if ((ascii1<0x3a)&&(ascii1>0x2f))
	number += ascii1 - 0x30;
	else if ((ascii1>0x40)&&(ascii1<='F'))
		number += ascii1 - 0x41 + 10;
//lbq 2003 08 20
	else if ((ascii0>0x60)&&(ascii0<='f'))
		number += ascii1 - 0x61 + 10;
	return number;
}

unsigned char ChgHexToAscii(number)
unsigned char number;
{
	if (number<10)
		number += 0x30;
	else if ((number>9)&&(number<16))
		number = number + 0x41 - 10;
	return number;
}
unsigned char ChgAsciiToHex(unsigned char Byte)
{
	if ((Byte<0x3a)&&(Byte>0x2f))
	Byte-=0x30;
	else if ((Byte>0x40)&&(Byte<='F'))
	Byte=Byte-0x41+10;
	return Byte;
}

int iccReadData(lParam,lpData,lpnSize)
unsigned char *lParam;
unsigned char *lpData;
unsigned int *lpnSize;
{
	int i,k,stat,endReceive=0,data_num=0;
	unsigned char ch,lrc=0;
	unsigned char dataReceive[550];

	i=0;
	do{
		stat = Star_readChar(&ch,50);
		if (stat==-1) return ICCE_TIMEOUT;

		if ( ch == ESC_KEY )
			return ICCE_ESC_KEY_PRESS;
		if ((ch == ETX ) && ( i != 0 )) endReceive=1;
		dataReceive[i] = ch;
		if ((dataReceive[0]==0x36) || (dataReceive[0]==0x45)) i++;
		if ( i==2 )
			if (dataReceive[1]!=0x30) i=0;
	}while ( endReceive==0 );
	data_num=i;

	dataReceive[i]=0;
#if DEBUG
	printf("rec data: %s ;%d\n\r",dataReceive,i);
#endif
	for (i=0;i<(data_num-1)/2;i++)
	{
		dataReceive[i]=TwoAsciiToHex(dataReceive[2*i],dataReceive[2*i+1]);
		lrc ^= dataReceive[i];
	}
	dataReceive[i]=0;
	if (dataReceive[0] == NACK)    	return ICCE_NACK;
	if ((i-3) != dataReceive[1])	return ICCE_DATAPACK_ERR;
	if (lrc!=0)		return ICCE_LRC_ERR;
#if DEBUG
printf("rec over\n\r");
#endif
	for (i=0;i<dataReceive[1]-1;i++){
		lpData[i]=dataReceive[i+3];
	}
	lpData[i]=0;
	*lParam=dataReceive[2];
	*lpnSize=dataReceive[1]-1;
#if DEBUG
printf("iccReadData:lparam=%2X,%d\n\r",*lParam,*lpnSize);
#endif
	return ICCE_OK;
}

int iccDataWriteUsePack(term,lParam,lpData,nSize)
unsigned char lParam;
unsigned char *lpData;
int nSize,term;
{
	unsigned char lrc,byte;
	int dataLength,k;
	int stat,i;
	char dataBuffer[600];



	dataLength=nSize+1;
	lrc = ACK ^ dataLength ^ lParam;

	k=0;
	dataBuffer[k*2]  = ChgHexToAscii(ACK>>4);
	dataBuffer[k*2+1]= ChgHexToAscii(ACK&0x0F);
	k++;
	dataBuffer[k*2]  = ChgHexToAscii(dataLength>>4);
	dataBuffer[k*2+1]= ChgHexToAscii(dataLength&0x0F);
	k++;
	dataBuffer[k*2]  = ChgHexToAscii(lParam>>4);
	dataBuffer[k*2+1]= ChgHexToAscii(lParam&0x0F);
	k++;
	for(i=0;i<nSize;i++)
	{
		byte = lpData[i];
		dataBuffer[k*2]  = ChgHexToAscii(byte>>4);
		dataBuffer[k*2+1]= ChgHexToAscii(byte&0x0F);
		k++;
		lrc ^= byte ;
	}
	dataBuffer[k*2]  = ChgHexToAscii(lrc>>4);
	dataBuffer[k*2+1]= ChgHexToAscii(lrc&0x0F);
	k++;
	dataBuffer[k*2]=ETX;
	dataBuffer[k*2+1]=0x0;
#if DEBUG
	printf("sendData=");
	for(i=0;i<2*k+1;i++)
		printf("%2x",dataBuffer[i]);
	printf("\n");
#endif
	stat=Star_dataToDevice(term,dataBuffer);
	return stat;
}

int memIccSelectCard(int term,int port,unsigned char cardType)
{
	unsigned char comm[5],data[300];
	int stat;
	unsigned char lParam=1;
	unsigned int lData=256;

	Star_openPort(term,port);

	comm[0]=cardType;
	stat = iccDataWriteUsePack(term,MEM_ICC_SELECT_CARD,comm,1);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, data, &lData );
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}
int memIccPowerUp(int term,int port)
{
	unsigned char comm[5],data[300];
	int stat;
	unsigned char lParam=1;
	unsigned int lData=256;

	Star_openPort(term,port);

	stat = iccDataWriteUsePack(term,MEM_ICC_POWER_UP,comm,0);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, data, &lData);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}
int memIccPowerDown(int term,int port)
{
	unsigned char comm[5],data[300];
	int stat;
	unsigned char lParam=1;
	unsigned int lData=256;

	Star_openPort(term,port);

	stat = iccDataWriteUsePack(term,MEM_ICC_POWER_DOWN,comm,0);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, data, &lData);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}
int memIccChkSecCode(term,port,secCode,codeLen)
unsigned char *secCode;
unsigned char codeLen;
int term,port;
{
	unsigned char comm[300];
	int stat;
	unsigned char lParam=1;
	unsigned int lData=256;

	Star_openPort(term,port);

	memcpy(comm, secCode, codeLen);
	stat = iccDataWriteUsePack(term,MEM_ICC_CHECK_SECURITY,comm,codeLen);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, comm, &lData );
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}
int memIccReadCard(term,port,data,zone,addr,len)
unsigned char *data;
unsigned char zone;
unsigned char addr;
unsigned char len;
int term,port;
{
	unsigned char comm[5];
	int stat;
	unsigned char lParam=1;
	unsigned int lData=256;

	Star_openPort(term,port);

	comm[0]=zone;
	comm[1]=addr;
	comm[2]=len;
	stat = iccDataWriteUsePack(term,MEM_ICC_READ_CARD,comm,3);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, data, &lData );
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}
int memIccWriteCard(term,port,data,zone,addr,len)
unsigned char *data;
unsigned char zone;
unsigned char addr;
unsigned char len;
int term,port;
{
	unsigned char comm[300];
	int stat;
	unsigned char lParam=1;
	unsigned int lData=256;

	Star_openPort(term,port);

	comm[0]=zone;
	comm[1]=addr;
	comm[2]=len;
	memcpy(comm+3,data,len);
	stat = iccDataWriteUsePack(term,MEM_ICC_WRITE_CARD,comm,len+3);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, comm , &lData );
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}
int memIccEraseCard(term,port,zone,addr,len)
unsigned char zone;
unsigned char addr;
unsigned char len;
int term,port;
{
	unsigned char comm[5];
	int stat;
	unsigned char lParam=1;
	unsigned int lData=256;

	Star_openPort(term,port);

	comm[0]=zone;
	comm[1]=addr;
	comm[2]=len;
	stat = iccDataWriteUsePack(term,MEM_ICC_ERASE_CARD,comm,3);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, comm , &lData );
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}

int memIccChkErasePin(term,port,zone,secCode,codeLen)
unsigned char zone;
unsigned char *secCode;
unsigned char codeLen;
int term,port;
{
	unsigned char comm[300];
	int stat;
	unsigned int lData=256;
	int nRetCode;
	unsigned char lParam=1;

	Star_openPort(term,port);
	comm[0]=zone;
	memcpy(comm+1, secCode, codeLen);
	stat = iccDataWriteUsePack(term,MEM_ICC_CHECK_ERASEPIN,comm,codeLen+1);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}
	stat = iccReadData (&lParam, comm, &lData );
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}
/****************************************************

int memIccReadErrNum(data)
unsigned char data[30];
{
	unsigned char comm[200];
	int stat;
	int nRetCode;
	unsigned int lData=256;
	unsigned char lParam=1;

	openPort(ICCPORT);

	stat = iccDataWriteUsePack(MEM_ICC_READ_ERRNUM,comm,0);
	if (stat!=0)
	{
		closePort();
		return stat;
	}
	stat = iccReadData (&lParam,data,&lData );
	if (stat!=0)
	{
		closePort();
		return stat;
	}

	closePort();
	return lParam;
}

**************************************************************/

int cpuIccPowerUp(term,port,Icslot,respone,lRep,status)
unsigned int Icslot;
unsigned char respone[200];
unsigned char *lRep;
LPSTATUS *status;
int term,port;
{
	unsigned char comm[5],data[300],T0;
	int stat;
	unsigned char lParam=1,a;
	unsigned int lData=256,i,j,k,cmd;
	int nSendDataLen=0;
	int ncount=0;

	Star_openPort(term,port);

	switch(Icslot)
	{
		case 0:
			cmd=SAM_ICC1_POWER_UP;
			nSendDataLen=0;
			break;
		case 2:
		case 3:
		case 4:
			cmd=SAMCPU_ICC_POWER_UP;
			comm[0] = Icslot;
			nSendDataLen=1;
			break;
		case 1:
		default:
			cmd=CPU_ICC1_POWER_UP;
			nSendDataLen=0;
			break;
	}

//	stat = iccDataWriteUsePack(cmd,comm,0);
	stat = iccDataWriteUsePack(term,cmd,comm,nSendDataLen);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, respone, &lData);
	*lRep=lData;

	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}
	k=lData;
#if DEBUG
	printf("iccReadData1:lparam=%2X,%d\n\r",lParam,k);
	for(i=0;i<lData;i++)
		printf("%2X ",respone[i]);
	printf("\n\r");
#endif

//for protocol
	if ((respone[0]==0x18)&&(respone[1]=0x2))		//for 711上电返回值 18H,02H,LEN(卡上电返回数据长度减一)+卡上电返回数据
	{
		if((respone[4]&0x0f)==1)
			gsIcProtocol[Icslot] = T1_PROTOCOL;
		else
			gsIcProtocol[Icslot] = T0_PROTOCOL;
	}
	else
	{
//		gsIcProtocol[byIccSlot] = respone[1] & 0x0F;	//for 102
//lpData[0]--Ts,lpData[1]--T0,lpData[2]--TB1,lpData[3]--TC1,lpData[4]--TD1
		if((respone[1]&0x80)==0)
			gsIcProtocol[Icslot] = T0_PROTOCOL;
		else
		{
			ncount=2;
			if((respone[1]&0x10)!=0)	//TA1
				ncount=ncount+1;
			if((respone[1]&0x20)!=0)	//TB1
				ncount=ncount+1;
			if((respone[1]&0x40)!=0)	//TC1
				ncount=ncount+1;
			if((respone[ncount]&0x0f)==1)
				gsIcProtocol[Icslot] = T1_PROTOCOL;
			else
				gsIcProtocol[Icslot] = T0_PROTOCOL;

		}
	}

	status->me1 = respone[k-2];
	status->me2 = respone[k-1];
	status->MED = (unsigned int)lParam;

	Star_closePort(term);
	return lParam;
}

int cpuIccPowerDown(int term,int port,unsigned int Icslot)
{

	unsigned char comm[5];
	int stat;
	unsigned char lParam=1;
	unsigned int lData=256,cmd;
	int nSendDataLen=0;

	Star_openPort(term,port);

	switch(Icslot)
	{
		case 0:
			cmd=SAM_ICC1_POWER_DOWN;
			nSendDataLen=0;
			break;
		case 2:
		case 3:
		case 4:
			cmd=SAMCPU_ICC_POWER_DOWN;
			comm[0] = Icslot;
			nSendDataLen=1;
			break;
		case 1:
		default:
			cmd=CPU_ICC1_POWER_DOWN;
			nSendDataLen=0;
			break;
	}

//	stat = iccDataWriteUsePack(cmd,comm,0);
	stat = iccDataWriteUsePack(cmd,comm,nSendDataLen);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, comm, &lData);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	Star_closePort(term);
	return lParam;
}

int cpuIccIncoming(term,port,Icslot,Cla,INS,P1,P2,Lc,inData,status,pbyReceLen)
unsigned int Icslot,term,port;
unsigned char Cla;
unsigned char INS;
unsigned char P1;
unsigned char P2;
unsigned int Lc;
unsigned char *inData;
LPSTATUS *status;
unsigned char *pbyReceLen;
{
	unsigned char comm[256];
	int i,stat,cmd;
	unsigned int lData=256;
	unsigned char lParam;
	unsigned char tmp_stat;
	int nSendDataLen=0;

	if (Lc>=249)	return ICCE_IN;

	Star_openPort(term,port);
	switch(Icslot)
	{
		case 0:
			cmd=SAM_ICC1_INCOMING;
			comm[0] = Cla;
			comm[1] = INS;
			comm[2] = P1;
			comm[3] = P2;
			comm[4] = (unsigned char)Lc;
			nSendDataLen=Lc+5;
			for(i=0;i<comm[4];i++)
				comm[i+5]=inData[i];
			break;
		case 2:
		case 3:
		case 4:
			cmd=SAMCPU_ICC_INCOMING;
			comm[0] = Icslot;
			comm[1] = Cla;
			comm[2] = INS;
			comm[3] = P1;
			comm[4] = P2;
			comm[5] = (unsigned char)Lc;
			nSendDataLen=Lc+6;
			for(i=0;i<comm[5];i++)
				comm[i+6]=inData[i];
			break;
		case 1:
		default:
			cmd=CPU_ICC1_INCOMING;
			comm[0] = Cla;
			comm[1] = INS;
			comm[2] = P1;
			comm[3] = P2;
			comm[4] = (unsigned char)Lc;
			nSendDataLen=Lc+5;
			for(i=0;i<comm[4];i++)
				comm[i+5]=inData[i];
			break;
	}

//	stat = iccDataWriteUsePack(cmd,comm,Lc+5);
	stat = iccDataWriteUsePack(term,cmd,comm,nSendDataLen);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, comm, &lData);
	*pbyReceLen=lData;
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	status->me1 = comm[lData-2];
	status->me2 = comm[lData-1];
	status->MED = lParam;

	Star_closePort(term);
	return  lParam;
}

int cpuIccOutgoing(term,port,Icslot,Cla,INS,P1,P2,Le,outData,status,pbyReceLen)
unsigned int Icslot,term,port;
unsigned char Cla;
unsigned char INS;
unsigned char P1;
unsigned char P2;
unsigned int Le;
unsigned char *outData;
LPSTATUS *status;
unsigned char *pbyReceLen;
{
	unsigned char comm[256],tmp_stat;
	int stat;
	unsigned char lParam;
	unsigned int lData=256,cmd,i;
	int nSendDataLen=0;

	if (Le>=251)	return ICCE_IN;

	Star_openPort(term,port);
	switch(Icslot)
	{
		case 0:
			cmd=SAM_ICC1_OUTGOING;
			comm[0] = Cla;
			comm[1] = INS;
			comm[2] = P1;
			comm[3] = P2;
			comm[4] = (unsigned char)Le;
			nSendDataLen=5;
			break;
		case 2:
		case 3:
		case 4:
			cmd=SAMCPU_ICC_OUTGOING;
			comm[0] = Icslot;
			comm[1] = Cla;
			comm[2] = INS;
			comm[3] = P1;
			comm[4] = P2;
			comm[5] = (unsigned char)Le;
			nSendDataLen=6;
			break;
		case 1:
		default:
			cmd=CPU_ICC1_OUTGOING;
			comm[0] = Cla;
			comm[1] = INS;
			comm[2] = P1;
			comm[3] = P2;
			comm[4] = (unsigned char)Le;
			nSendDataLen=5;
			break;
	}

//	stat = iccDataWriteUsePack(cmd,comm,5);
	stat = iccDataWriteUsePack(term,cmd,comm,nSendDataLen);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, outData,  &lData);
	*pbyReceLen=lData;
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}
#if DEBUG
	printf("lParam=%d,lData=%d\n",lParam,lData);
	printf("outdata=");
	for(i=0;i<lData;i++)
		printf("%x",outData[i]);
	printf("\nrec over\n");
#endif

	status->me1 = outData[lData-2];
	status->me2 = outData[lData-1];
	status->MED = lParam;

	Star_closePort(term);
	return  lParam;
}

int cpuIccCommunication(term,port,IccSlot,CLA,INS,P1,P2,Lc,psSendData,Le,psReceData,status,pbyReceLen)
unsigned int IccSlot,term,port;
unsigned char CLA;
unsigned char INS;
unsigned char P1;
unsigned char P2;
unsigned int Lc;
unsigned char *psSendData;
unsigned int Le;
unsigned char *psReceData;
LPSTATUS *status;
unsigned char *pbyReceLen;
{
	unsigned char nRetCode;
	unsigned char nLen=0;
	unsigned char nIccProtocol;
	nIccProtocol = gsIcProtocol[IccSlot];

	if(nIccProtocol==T0_PROTOCOL)
	{
		if(Lc>0)
		{
			nRetCode=cpuIccIncoming(term,port,IccSlot,CLA,INS,P1,P2,Lc,psSendData,status,pbyReceLen);
			if((nRetCode!=0xe7)&&(nRetCode!=SUC)) return nRetCode;
			if( (nRetCode==0xe7)&&(status->me1==0x61) ) nLen=status->me2;
			if( nLen==0 && nRetCode==SUC ) nLen=Le;
			if( nLen!=0 )
			{
				nRetCode=cpuIccOutgoing(term,port,IccSlot,0x00,0xC0,0,0,nLen,psReceData,status,pbyReceLen);
			}
		}
		else
		{
			nRetCode=cpuIccOutgoing(term,port,IccSlot,CLA,INS,P1,P2,Le,psReceData,status,pbyReceLen);
			if((nRetCode!=0xe7)&&(nRetCode!=SUC)) return nRetCode;
			nLen=Le;
			if( (nRetCode==0xe7)&&(status->me1==0x61) )
			{
				nLen=status->me2;
				nRetCode=cpuIccOutgoing(term,port,IccSlot,0x00,0xC0,0,0,nLen,psReceData,status,pbyReceLen);
				if((nRetCode!=0xe7)&&(nRetCode!=SUC)) return nRetCode;
			}
			if( (nRetCode==0xe7)&&(status->me1==0x6c) )
			{
				nLen=status->me2;
				nRetCode=cpuIccOutgoing(term,port,IccSlot,CLA,INS,P1,P2,nLen,psReceData,status,pbyReceLen);
			}
		}
	}
	if(nIccProtocol==T1_PROTOCOL)
	{
		if(INS==0xB2 && Le==0)  // for VARLEN record file
		{
			nRetCode=cpuIccExchange(term,port,IccSlot,CLA,INS,P1,P2,Lc,psSendData,3,psReceData,status,pbyReceLen);
			if((nRetCode!=0)&&(nRetCode!=0xe7)) return nRetCode;
			Le=psReceData[1]+2;
		}
		nRetCode=cpuIccExchange(term,port,IccSlot,CLA,INS,P1,P2,Lc,psSendData,Le,psReceData,status,pbyReceLen);
	}
	return nRetCode;
}

int cpuIccExchange(term,port,Icslot,Cla,INS,P1,P2,Lc,inData,Le,outData,status,pbyReceLen)
unsigned int Icslot,term,port;
unsigned char Cla;
unsigned char INS;
unsigned char P1;
unsigned char P2;
unsigned int Lc;
unsigned char *inData;
unsigned int Le;
unsigned char *outData;
LPSTATUS *status;
unsigned char *pbyReceLen;
{
	unsigned char comm[256],tmp_stat;
	int stat;
	unsigned char lParam;
	unsigned int lData=256,cmd,i;
	int nSendDataLen=0;

	if (Lc>=249)	return ICCE_IN;
	if (Le>=251)	return ICCE_IN;

	Star_openPort(term,port);
	switch(Icslot)
	{
		case 0:
			cmd=SAM_ICC1_EXCHANGE;
			comm[0] = Cla;
			comm[1] = INS;
			comm[2] = P1;
			comm[3] = P2;
			comm[4] = (unsigned char)Lc;
			for(i=0;i<comm[4];i++)
				comm[i+5]=inData[i];
			comm[5+Lc] = (unsigned char)Le;
			nSendDataLen= Lc+6;
			break;
		case 2:
		case 3:
		case 4:
			cmd=SAMCPU_ICC_EXCHANGE;
			comm[0] = Icslot;
			comm[1] = Cla;
			comm[2] = INS;
			comm[3] = P1;
			comm[4] = P2;
			comm[5] = (unsigned char)Lc;
			for(i=0;i<comm[5];i++)
				comm[i+6]=inData[i];
			comm[6+Lc] = (unsigned char)Le;
			nSendDataLen= Lc+7;
			break;
		case 1:
		default:
			cmd=CPU_ICC1_EXCHANGE;
			comm[0] = Cla;
			comm[1] = INS;
			comm[2] = P1;
			comm[3] = P2;
			comm[4] = (unsigned char)Lc;
			for(i=0;i<comm[4];i++)
				comm[i+5]=inData[i];
			comm[5+Lc] = (unsigned char)Le;
			nSendDataLen= Lc+6;
			break;
	}

//	stat = iccDataWriteUsePack(cmd,comm,Lc+6);
	stat = iccDataWriteUsePack(term,cmd,comm,nSendDataLen);
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}

	stat = iccReadData (&lParam, outData,  &lData);
	*pbyReceLen=lData;
	if (stat!=0)
	{
		Star_closePort(term);
		return stat;
	}
#if DEBUG
	printf("lParam=%d,lData=%d\n",lParam,lData);
	printf("outData=");
	for(i=0;i<lData;i++)
		printf("%x",outData[i]);
	printf("\nrec over\n");
#endif

	status->me1 = outData[lData-2];
	status->me2 = outData[lData-1];
	status->MED = lParam;

	Star_closePort(term);
	return  lParam;
}
