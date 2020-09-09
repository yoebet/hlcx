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

main()
{
	unsigned char sdata[500];
	unsigned int ndata;
	unsigned int star1604=0;
	unsigned char sPIN[10];
	unsigned char sNum[10];
	unsigned char sTmp[500];
	char pin[10];
	unsigned int nRetCode;
	int nChoice,IcChoice,CChoice,PChoice,MainChoice;
	int Icslot = 0;
	unsigned int nCardType;
	LPSTATUS tstatus;
	unsigned char Cretdata[500];
	char Cretlen,nretlen;
	unsigned int i;
	unsigned char sdatain[500];

	char tr2[38],tr3[108];
	int q,mode;
       
	MainChoice=select_();
	if(MainChoice==0) return 1;
	else if(MainChoice==1)
	{
	   do
	   {
	       nChoice=selectMenuMem();
	       switch (nChoice)
	       {
	           case 1:
		         fflush(stdin);
			 fflush(stdout);
			 printf("Please input card type(0---9):");
			 scanf("%d",&nCardType);
			 if(nCardType==8)  star1604=1;
			 nRetCode=memIccSelectCard(TERM_TYPE,ICC_PORT,nCardType);
			 if(nRetCode!=SUC)
			     printf("Select Card Err\n");
			 else
			     printf("Select Card Suc\n");
			 break;
		   case 2:
			 nRetCode=memIccPowerUp(TERM_TYPE,ICC_PORT);
			 if(nRetCode!=SUC) printf("Power Up Err\n");
			 else
			     printf("Power Up Suc\n ");
			 break;
		   case 3:
			 fflush(stdin);
			 fflush(stdout);
			 printf("Please input password:");
			 fflush(stdin);
			 scanf("%s",sPIN);
			 if(star1604)
			 {
			     sNum[0]=TwoAsciiToHex(0,0);
			     sNum[1]=TwoAsciiToHex(sPIN[0],sPIN[1]);
			     sNum[2]=TwoAsciiToHex(sPIN[2],sPIN[3]);
			 }
			 else
			 {
			     sNum[0]=TwoAsciiToHex(sPIN[0],sPIN[1]);
			     sNum[1]=TwoAsciiToHex(sPIN[2],sPIN[3]);
			     sNum[2]=TwoAsciiToHex(sPIN[4],sPIN[5]);
			 }
			 nRetCode=memIccChkSecCode(TERM_TYPE,ICC_PORT,sNum,sizeof(sNum));
			 if(nRetCode!=SUC)
			     printf("Password Check Err retcode=%d\n",nRetCode);
			 else
			     printf("Password Check Suc\n");
			 break;
		   case 4:
			 nRetCode=memIccReadCard(TERM_TYPE,ICC_PORT,sdata,00,30,10);
			 if(nRetCode!=SUC)
			     printf("Read Card Fail retcode=%d\n",nRetCode);
			 else
			 {
			     printf("Read Card Suc\n");
			     Star_bcdToAsc(sTmp,sdata,10,0);
			     sTmp[20]=0;
			     printf("return data = %s \n" , sTmp );
			 }
			 break;
		   case 5:
			 fflush(stdin);
			 fflush(stdout);
			 printf("Please i：");
			 fflush(stdin);
			 scanf("%s",sdata);
			 nRetCode=memIccWriteCard(TERM_TYPE,ICC_PORT,sdata,00,30,10);
			 if(nRetCode!=SUC)
			     printf("Write Card Fail retcode=%d\n",nRetCode);
			 else
			     printf("Write Card Suc\n");
			 break;
		   case 6:
			 nRetCode=memIccEraseCard(TERM_TYPE,ICC_PORT,00,30,10);
			 if(nRetCode!=SUC)
			     printf("Card Erase Fail retcode=%d\n",nRetCode);
			 else
			     printf("Card Erase Suc\n");
			 break;
		   case 7:
			 fflush(stdin);
			 fflush(stdout);
			 printf("请输入区一擦除密码（6字节）：");
			 fflush(stdin);
			 scanf("%s",sPIN);
			 sNum[0]=TwoAsciiToHex(sPIN[0],sPIN[1]);
			 sNum[1]=TwoAsciiToHex(sPIN[2],sPIN[3]);
			 sNum[2]=TwoAsciiToHex(sPIN[4],sPIN[5]);
			 sNum[3]=TwoAsciiToHex(sPIN[6],sPIN[7]);
			 sNum[4]=TwoAsciiToHex(sPIN[8],sPIN[9]);
			 sNum[5]=TwoAsciiToHex(sPIN[10],sPIN[11]);
  			 nRetCode=memIccChkErasePin(1,sNum,6);
			 if(nRetCode!=SUC)
			     printf("区一擦除失败 retcode=%d\n",nRetCode);
			 else
			     printf("区一已正确擦除\n");
			 break;
		   case 8:
			 nRetCode=memIccPowerDown(TERM_TYPE,ICC_PORT);
			 if(nRetCode!=SUC)
			     printf("Power Down Fail\n");
			 else
			     printf("Power Down Suc\n");
			 break;
		   case 0:
		 	 break;
	       }
	       printf("press enter key ...");
	       fflush(stdin);
	       getchar();
	   }while(nChoice!=0);
	}
	else if(MainChoice==2)
	{
	   do
	   {
	       CChoice=selectMenuCpu();
	       switch(CChoice)
	       {
		   case 1:
		         printf("please input ic num(0--4):");
			 fflush(stdout);
			 fflush(stdin);
			 scanf("%d",&Icslot);
			 break;
		   case 2:
			 printf("ic slot is:%d\n",Icslot);
		         nRetCode=cpuIccPowerUp(TERM_TYPE,ICC_PORT,Icslot,Cretdata,&Cretlen,&tstatus);
			 if(nRetCode!=SUC)
			 {
			     printf("Cpu Card Power Up Fail！");
			     printf("nRetCode=%x\n",nRetCode);
			 }
			 else
			 {
//			     if(Cretdata[1]&0x80)
//			         gProtocol=T1_PROTROCOL;
//			     else
//			         gProtocol=T0_PROTROCOL;

			     printf("Cpu Card Power Up Suc,Return=");
			     for(i=0;i<Cretlen;i++)
			     printf("%x ",Cretdata[i]);
			     printf("\n");
			     printf("sw1=%x,sw2=%x\n",tstatus.me1,tstatus.me2);
			     printf("\n");

			 }
			 fflush(stdout);
			 break;

		   case 3:
//			 if(gProtocol=T1_PROTOCOL)
//			     exchaang()
//			 else
//			     communicatasio()
//			 nRetCode=cpuIccOutgoing(Icslot,00,0x84,00,00,4,Cretdata,&tstatus,&nretlen);
			 nRetCode=cpuIccCommunication(TERM_TYPE,ICC_PORT,Icslot,0x00,0x84,0x00,0x00,0x00,sdatain,4,Cretdata,&tstatus,&nretlen);
			 if(nRetCode!=SUC)
			 {
			     printf("get random number fail！\n");
			     printf("nRetCode=%x,sw1=%x,sw2=%x\n",nRetCode,tstatus.me1,tstatus.me2);
			 }
			 else
			 {
			     printf("get random number OK, number is:");
			     for(i=0;i<8;i++)
			         printf("%x ",Cretdata[i]);
			     printf("\nsw1=%x, sw2=%x\n",tstatus.me1,tstatus.me2);
			 }
			 fflush(stdout);
			 break;
		   case 4:
			 nRetCode=cpuIccPowerDown(TERM_TYPE,ICC_PORT,Icslot);
			 if(nRetCode!=SUC)
			 {
			     printf("Cpu Card Power Down Fail！\n");
			 }
			 else
			 {
			     printf("Cpu Card Power Down Suc\n");
			 }
			 fflush(stdout);
			 break;

		   case 0:
			 default:
			 break;
	       }
	       if(Icslot>4 || Icslot<0 )
	       {
	           Icslot=0;
		   printf("\nWrong Choice ...");
		   fflush(stdout);
		   fflush(stdin);
		   getchar();
	       }
	       else
	       {	
	       	   printf("\npress enter key ...");
		   fflush(stdout);
		   fflush(stdin);
		   getchar();
	       }
	   }while(CChoice!=0);
	}
	else if(MainChoice==3)  //PINPAD12 ;
	{
           do
	   {
	       PChoice=SelectMenuPad12();
	       switch(PChoice)
	       {
		   case 1:
		   case 2:
			 printf("Please input PIN:\n");
			 nRetCode=STAR_pinpadReadPin(TERM_TYPE,PINPAD_PORT,PChoice,pin,TIMEOUT);
			 if (nRetCode==0) printf("\nYour Pin is %s\n",pin);
			 else printf("read PIN timeout\n");
		   break;

		   case 0:
		   default:
			 break;
	       }
	       if(PChoice>2 || PChoice<0 )
	       {
		   printf("\nWrong Choice ...");
		   fflush(stdout);
		   fflush(stdin);
		   getchar();
	       }
	       else
	       {   printf("\npress enter key ...");
		   fflush(stdout);
		   fflush(stdin);
		   getchar();
	       }
           }while(PChoice!=0);
	}
	else if(MainChoice==4)  //DesPINPAD ;
	{
	   do
	   {
	       PChoice=SelectMenuDesPad();
	       switch(PChoice)
	       {
		   case 1:
		   case 2:
			 printf("Please input PIN:\n");
			 nRetCode=STAR_Getkey(TERM_TYPE,PINPAD_PORT,PChoice-1,0,6,pin,TIMEOUT);
			 if (nRetCode==0) printf("\nYour Pin is %s\n",pin);
			 else printf("read PIN timeout\n");
			 break;
		   			case 3:
		   			case 4:
			 					printf("Please input PIN:\n");
			 					nRetCode=STAR_3DESGetkey(TERM_TYPE,PINPAD_PORT,PChoice-3,0,6,pin,TIMEOUT);
			 					if (nRetCode==0) printf("\nYour Pin is[%d] %s\n",strlen(pin),pin);
			 					else printf("read PIN timeout\n");
			 					break;
		   case 0:
		   default:
			break;
	       }
	       if(PChoice>4 || PChoice<0 )
	       {
		   printf("\nWrong Choice ...");
		   fflush(stdout);
		   fflush(stdin);
		   getchar();
	       }
	       else
	       {   printf("\npress enter key ...");
		   fflush(stdout);
		   fflush(stdin);
		   getchar();
	       }
	   }while(PChoice!=0);
	}
	else if(MainChoice==5)  //Msr 
	{
	do
	   {
	       PChoice=SelectMenuMsr();
	       switch(PChoice)
	       {
	           case 1:
			 printf("please input read_mode:(23:23道,2:2道,3:3道)");
			 fflush(stdin);
			 scanf("%d",&mode);
			 fflush(stdin);
			 printf("please  read card !\n");
			 nRetCode=starMsrRead(TERM_TYPE,MSR_PORT,mode,tr2,tr3,TIMEOUT);
			 printf("\ntr2[Len=%d]=",strlen(tr2));
			 puts(tr2);
			 printf("tr3[Len=%d]=",strlen(tr3));
			 puts(tr3);
			 printf("\nRetCode=%d\n",nRetCode);
			 break;
		   case 2:
			 fflush(stdin);
			 printf("\ninput track2 data(no data return)\n");
			 gets(tr2);
			 fflush(stdin);
			 printf("input track3 data(no data return)\n");
			 gets(tr3);
			 nRetCode=starMsrWrite(TERM_TYPE,MSR_PORT,tr2,tr3,TIMEOUT);
			 printf("\nRetCode=%d\n",nRetCode);
			 break;
		   case 0:
		   default:
			 break;
	       }
	       if(PChoice>2 || PChoice<0 )
	       {
		   printf("\nWrong Choice ...");
		   fflush(stdout);
		   fflush(stdin);
		   getchar();
	       }
	       else
	       {   printf("\npress enter key ...");
		   fflush(stdout);
		   fflush(stdin);
		   getchar();
	       }
	   }while(PChoice!=0);
	}
	return SUC;
}


int select_()
{
    int IcChoice;
    do
    {
        printf("\n");
	printf( "          ============================================\n");
	printf( "          =           START  API  DEMO               =\n");
	printf( "          ============================================\n");
	printf( "          =    [ 1] IC Memory    Test    存储卡      =\n");
	printf( "          =    [ 2] IC  CPU      Test    CPU 卡      =\n");
	printf( "          =    [ 3] NorPinpad    Test    明码密键    =\n");
	printf( "          =    [ 4] DesPinpad    Test    DES 密键    =\n");
	printf( "          =    [ 5] Msr          Test    磁卡机具    =\n");
	printf( "          =    [ 0] Exit                 退出        =\n");
	printf( "          ============================================\n");
	printf("please select:");
	fflush(stdout);
	fflush(stdin);
	scanf("%d",&IcChoice);
    }while(IcChoice< 0 || IcChoice>5);
return IcChoice;
}


int selectMenuMem(void)
{
    int nChoice;
    do
    {
        printf("\n");
	printf("          ============================================\n");
	printf("          =         IC Memory  DEMO                  =\n");
	printf("          ============================================\n");
	printf("          =    [ 1] select card [选卡]               =\n");
	printf("          =    [ 2] power up [上电]                  =\n");
	printf("          =    [ 3] check pin [密码校验              =\n");
	printf("          =    [ 4] read card [读卡]                 =\n");
	printf("          =    [ 5] write card [写卡]                =\n");
	printf("          =    [ 6] erase card [擦卡]                =\n");
	printf("          =    [ 7] check erase pin                  =\n");
	printf("          =    [ 8] power down [下电]                =\n");
	printf("          =    [ 0] exit [退出]                      =\n");
	printf("          ============================================\n");
	printf("please select:");
	fflush(stdout);
	fflush(stdin);
	scanf("%d",&nChoice);
    }while(nChoice< 0 || nChoice>8);
return nChoice;
}

int selectMenuCpu(void)
{
    int CChoice;
    do
    {
	printf("\n");
	printf( "          ==========================================\n");
	printf( "          =            IC CPU DEMO                 =\n");
	printf( "          ==========================================\n");
	printf( "          =    [ 1] select Ic slot     选择卡座    =\n");
	printf( "          =    [ 2] power up           卡上电      =\n");
	printf( "          =    [ 3] get random number  读随机数    =\n");
	printf( "          =    [ 4] power down         卡下电      =\n");
	printf( "          =    [ 0] Exit               退出        =\n");
	printf( "          ==========================================\n");
	printf("please select:");
	fflush(stdout);
	fflush(stdin);
	scanf("%d",&CChoice);
    }while(CChoice< 0 || CChoice>4);
return CChoice;
}
int SelectMenuPad12(void)
{
    int CChoice;
    do 
    {
	printf("\n");
	printf( "          ============================================\n");
	printf( "          =           NorPinpad  Test                =\n");
	printf( "          ============================================\n");
	printf( "          =  [1] Pinpad Green Read  明码密键绿灯亮读 =\n");
	printf( "          =  [2] Pinpad Red   Read  密码密键红灯亮读 =\n");
	printf( "          =  [0] Exit               退出             =\n");
	printf( "          ============================================\n");
	printf("please select:");
	fflush(stdout);
	fflush(stdin);
	scanf("%d",&CChoice);
    }while(CChoice< 0 || CChoice>2);
return CChoice;
}
int SelectMenuMsr(void)
{
    int CChoice;
    do 
    {
	printf("\n");
	printf( "          ============================================\n");
	printf( "          =            Msr  Test     磁卡机具        =\n");
	printf( "          ============================================\n");
	printf( "          =           [1] Msr Read Card    磁卡读    =\n");
	printf( "          =           [2] Msr Write Card   磁卡写    =\n");
	printf( "          =           [0] Exit             退出      =\n");
	printf( "          ============================================\n");
	printf("please select:");
	fflush(stdout);
	fflush(stdin);
	scanf("%d",&CChoice);
    }while(CChoice< 0 || CChoice>2);
return CChoice;
}
int SelectMenuDesPad(void)
{
    int CChoice;
    do 
    {
	printf("\n");
	printf( "          ============================================\n");
	printf( "          =            DesPinPad  Test   DES密键     =\n");
	printf( "          ============================================\n");
	printf( "          =  [1] DES Green on  Read  DES密键绿灯亮读 =\n");
	printf( "          =  [2] DES Red   on  Read  DES密键红灯亮读 =\n");
	printf( "          =  [3] 3DES Green   read 3DES 密键绿灯亮读 =\n");
	printf( "          =  [4] 3DES Red     read 3DES 密键红灯亮读 =\n");	
	printf( "          =  [0] Exit                退出            =\n");
	printf( "          ============================================\n");
	printf("please select:");
	fflush(stdout);
	fflush(stdin);
	scanf("%d",&CChoice);
    }while(CChoice< 0 || CChoice>4);
return CChoice;
}
