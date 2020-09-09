/*
	MSR.C
	STAR DRIVERS PUBLIC PROGRAM FOR UNIX
	AUTHOR:		Wind
	VERSION:	V1.0
	DATE: 		1998.09.06
	Copyright (c) 1998-2005 START Group Co. Ltd.
   	All rights reserved.
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>

#define MSR_POWER_RESET 		"\033S"
#define MSR_RESET 			"\0330"

#define MSR_SET_ISO_FORMAT 		"\033B3"
#define MSR_SET_IBM_FORMAT 		"\033B4"
#define MSR_SET_ISO_CHARACTER 		"\033M"
#define MSR_SET_IBM_CHARACTER 		"\033O"
#define MSR_SET_EXTER_CHARACTER 	"\033P"
#define MSR_SET_NOEXTER_CHARACTER 	"\033C"
#define MSR_SET_HIGH_BPI 		"\033H"
#define MSR_SET_LOW_BPI 		"\033L"

#define MSR_STATE 			"\033j"

#if MSR102_COMM_SET
/* MSR102 COMMAND1*/

#define MSR_READ2 		"\033]"
#define MSR_READ3 		"\033T]"
#define MSR_READ23  		"\033B]"

#define MSR_WRITE_LEAD 		"\033t"
#define MSR_WRITE23_SEP_STR 	"A"
#define MSR_WRITE_END 		"\035\033\134"

#define MSR_RW_STATE_LEAD 	"\033"
#define MSR_RW_STATE_END 	"pq"
#define MSR_RW_OK 		'p'

#define MSR_RW2_OK 		"\033rp"
#define MSR_RW3_OK 		"\033Trp"
#define MSR_RW23_OK 		"\033Brp"
#define MSR_RW2_ERR 		"\033rq"
#define MSR_RW3_ERR 		"\033Trq"
#define MSR_RW23_ERR 		"\033Brq"

#else
/* MSR Command 2*/
#define MSR_READ2 		"\033r"
#define MSR_READ3 		"\033p"
#define MSR_READ23 		"\033q"

#define MSR_WRITE2_LEAD 	"\033s"
#define MSR_WRITE3_LEAD 	"\033u\033t"
#define MSR_WRITE23_LEAD 	"\033v\033s"
#define MSR_WRITE23_SEP_STR 	"?\033t"
#define MSR_WRITE_END 		"?\034"

#define MSR_RW_STATE_LEAD 	"\033"
#define MSR_RW_STATE_END 	"0123"
#define MSR_RW_OK 		'0'

#define MSR_RW_OK_STR 		"\0330"
#define MSR_RW2_ERR_STR 	"\0331"
#define MSR_RW3_ERR_STR 	"\0332"
#define MSR_RW23_ERR_STR 	"\0333"

#endif

#define MSR102_READ_DATA_LEAD 	0x1b
#define MSR102_READ_DATA_END 	0x1c
#define MSR102_READ_DATA_LEAD_LEN 2
#define MSR102_READ_DATA_END_LEN 2
#define MSR102_READ23_DATA_SEP_CHAR 'A'

#define MSR101_READ_DATA_LEAD ';'
#define MSR101_READ_DATA_END  0xd
#define MSR101_READ23_DATA_SEP_CHAR '?'
#define MSR101_READ_DATA_LEAD_LEN 2
#define MSR101_READ_DATA_END_LEN 2
#define MSR101_READ_DATA_HEAD ";+"  //add by tornado

#define MSR_READ_DATA_LEAD 	"\033\002;+"
#define MSR_READ_DATA_END 	"\x1c\003\x0d"
#define MSR_READ23_DATA_SEP_CHAR "A?+"


int starMsrRead(term,port,read_mode,track2_data,track3_data,timeout)
int read_mode,term,port;
char *track2_data;
char *track3_data;
int timeout;
{
	int i,j,k,k1,flag;
	char ch;
	char data[300],state[5];
	int ret_code;

        memset(data,0,sizeof(data));
        memset(state,0,sizeof(state));
	if (Star_openPort(term,port)== -1) return -1;
        Star_dataToDevice(term,MSR_SET_IBM_CHARACTER); //add by tornado

	switch(read_mode)
	{
		case 2:
			Star_dataToDevice(term,MSR_READ2);
			break;
		case 3:
			Star_dataToDevice(term,MSR_READ3);
			break;
		case 23:
		default:
			Star_dataToDevice(term,MSR_READ23);
			break;
	}

	ret_code=Star_readLeadEndStr(data,MSR_READ_DATA_LEAD,MSR_READ_DATA_END,timeout);
	if (ret_code==-1)
	{
		Star_dataToDevice(term,MSR_RESET);
    ret_code=Star_readChar(&ch,10); //加上这句是为了避免23mcr的错误返回.
    //ret_code=Star_readChar(ch,10); //加上这句是为了避免23mcr的错误返回.
		Star_closePort(term);
		return -1;
	}
	if ((data[0]==MSR102_READ_DATA_LEAD)&&(strchr(MSR101_READ_DATA_HEAD,data[0])==NULL)&&((read_mode==2)||(read_mode==3))) //对于23道读方式不进行状态判断
	{
#if MSR102_COMM_SET
		Star_dataToDevice(term,MSR_STATE);
#endif

		ret_code=Star_readLeadEndStr(state,MSR_RW_STATE_LEAD,MSR_RW_STATE_END,timeout);

		if (ret_code==-1)
		{
			Star_closePort(term);
			return -1;
		}
		if (state[strlen(state)-1]!=MSR_RW_OK)
		{
			Star_closePort(term);
			return -1;
		}
	}
	Star_closePort(term);

        //printf("readstr is %s",data);
	j=strlen(data);
	for (i=0;i<j;i++)
	{
		if (strchr(MSR_READ_DATA_END,data[i])!=NULL)
		{
			flag=1;
			break;
		}
	}
	if (flag==1)
	{
		data[i+1]=0;
		j=strlen(data);
	}
	else
	{
		return -1;
	}
	flag=0;
	k=k1=0;
	for (i=0;i<j-2;i++)
	{
		if (strchr(MSR_READ23_DATA_SEP_CHAR,data[i])!=NULL)
		{
			flag=1;
			continue;
		}
		if (strchr(MSR_READ_DATA_LEAD,data[i])!=NULL)
		{
			continue;
		}
		if ((data[i]<'A')&&(data[i]>0x20))
		{
			if (flag==0)
			{
                            if (read_mode != 3) //add by zkh,2track data;
			        track2_data[k++]=data[i];
			}
			else
			{
                            if (read_mode != 2) //add by zkh,3track data;
				track3_data[k1++]=data[i];
			}
		}
	}
	track2_data[k]=0;
	track3_data[k1]=0;
        if((k==0)&&(k1==0)) return -1;
        if((read_mode!=2)&&(read_mode!=3))//非2,3 单道读模式,
        {
             if((!strcmp(track2_data,"00000")&&!strcmp(track3_data,"00000"))||((track2_data[0]==0x7f)&&(track3_data[0]==0x7f))) 
             {
               track2_data[0]='\0';
               track3_data[0]='\0';
               return -1;//read 23 track err!
             } 
             if (!strcmp(track2_data,"00000")||(track2_data[0]==0x7f))
             {
               track2_data[0]='\0';
             }
             if (!strcmp(track3_data,"00000")||(track3_data[0]==0x7f))
             {
               track3_data[0]='\0';
             }
        }
	return 0;
}

int starMsrWrite(term,port,track2_data,track3_data,timeout)
char *track2_data;
char *track3_data;
int timeout,term,port;
{
	int i,ret_code;
	char ch;
	char state[5];

	if ((strlen(track2_data)==0)&&(strlen(track3_data)==0)) return -1;

	if (Star_openPort(term,port) == -1) return -1;
        Star_dataToDevice(term,MSR_SET_IBM_CHARACTER);

#if MSR102_COMM_SET
	Star_dataToDevice(term,MSR_WRITE_LEAD);
	Star_dataToDevice(term,track2_data);
	if (strlen(track3_data)!=0)
	{
		Star_dataToDevice(term,MSR_WRITE23_SEP_STR);
		Star_dataToDevice(term,track3_data);
	}
	Star_dataToDevice(term,MSR_WRITE_END);

	Star_dataToDevice(term,MSR_STATE);
#else
	if ((strlen(track2_data)==0)||(strlen(track3_data)==0))
	{
		if (strlen(track2_data)==0)
		{
			Star_dataToDevice(term,MSR_WRITE3_LEAD);
			Star_dataToDevice(term,track3_data);
			Star_dataToDevice(term,MSR_WRITE_END);
		}
		else
		{
			Star_dataToDevice(term,MSR_WRITE2_LEAD);
			Star_dataToDevice(term,track2_data);
			Star_dataToDevice(term,MSR_WRITE_END);
		}
	}
	else
	{
		Star_dataToDevice(term,MSR_WRITE23_LEAD);
		Star_dataToDevice(term,track2_data);
		Star_dataToDevice(term,MSR_WRITE23_SEP_STR);
		Star_dataToDevice(term,track3_data);
		Star_dataToDevice(term,MSR_WRITE_END);
	}
#endif

	ret_code=Star_readLeadEndStr(state,MSR_RW_STATE_LEAD,MSR_RW_STATE_END,timeout);

	if (ret_code==-1)
	{
		Star_dataToDevice(term,MSR_RESET);
		Star_closePort(term);
		return -1;
	}
	Star_closePort(term);
	if (state[strlen(state)-1]!=MSR_RW_OK)
	{
		return -1;
	}
	return 0;
}
