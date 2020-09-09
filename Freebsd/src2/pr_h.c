#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "../include/sh_table.h"
#include "mysql.h"

extern char sys_err_str[];
extern char sys_op_name[9];
extern short sys_branch,sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];


#define GO_LINE1 16
#define GO_LINE2 0
#define GO_LINE3 3

short ymd[3];
short chk;
char amt[18];

/*-----------------------
    ��ӡ���۴��ƾ��
-----------------------*/
prt_h_t(tt,code,lc)
struct s_trade *tt;
short code;
short lc;
{
	char  parr[24], parr_int[24];
	long atol();
	int chk;

	openlp();
	newline(2);
	if(lc==1){
	  printf("%10s�ͻ�����"," ");
	}else if(lc==2){
	  printf("%10s��������"," ");
	}
	newline(2);
	if(code==1501) 
	  printf("%20s�����������д��ۿ���ƾ��"," ");
	else if(code==1502) 
	  printf("%20s�����������д�������ƾ��"," ");
	else if(code==1503) 
	  printf("%20s�����������д��۴��ƾ��"," ");
	else if(code==1504) 
	  printf("%20s�����������д���ȡ��ƾ��"," ");
	else if(code==1591) 
	  printf("%20s�����������д��۲���ƾ��"," ");
	else if(code==1592) 
	  printf("%20s�����������л���ƾ��"," ");
	newline(1);
	 printf("%15s==================================="," ");
	newline(2);
	//printf("123-12345678-11212345678122001081212341231234567P12345123456  123");
  chk=get_chk_no(tt->open_br,tt->account);
	if (tt->acc_type[0]=='P')
		printf("%10s�����ʺ�:%ld_%d"," ",tt->account,chk);
	printf("%10s ����:%s"," ",tt->customer);
	newline(1);
	sprintf(amt,"%d",tt->amount);
	pramt(amt,parr,15);
	printf("%10s������:���ֱ�ֵ%s "," ",parr);
	newline(1);
	printf("%10s����Ա��:%d"," ",tt->operator);
	printf("%5s��ˮ��:%d"," ",tt->ser_num);
	newline(1);
  rjulmdy(tt->open_d,ymd);
	printf("%10s������:%d��%d��%d��"," ",ymd[2],ymd[0],ymd[1]);
	newline(1);
	if(code!=1501) 
	{
    rjulmdy(sys_work_d,ymd);
	  printf("%10sҵ����:%d��%d��%d��"," ",ymd[2],ymd[0],ymd[1]);
	  newline(1);
  }
	
	backpage();
	closelp();
}

/*-----------------------
    ��ӡ�����ƾ��
-----------------------*/
prt_h_card(tt,code,lc)
struct s_trade *tt;
short code;
short lc;
{
	char  parr[24], parr_int[24];
	long atol();
	int chk;

	openlp();
	newline(2);
	if(lc==1){
	  printf("%10s�ͻ�����"," ");
	}else if(lc==2){
	  printf("%10s��������"," ");
	}
	newline(2);
	if(code==1501) 
	  printf("%20s�����������п�����ƾ��"," ");
	else if(code==1502) 
	  printf("%20s�����������п�����ƾ��"," ");
	else if(code==1503) 
	  printf("%20s�����������п����ƾ��"," ");
	else if(code==1504) 
	  printf("%20s�����������п�ȡ��ƾ��"," ");
	else if(code==1591) 
	  printf("%20s�����������д��۲���ƾ��"," ");
	newline(1);
	 printf("%15s==================================="," ");
	newline(2);
	//printf("123-12345678-11212345678122001081212341231234567P12345123456  123");
	if(code==1591) {
    chk=get_chk_no(tt->open_br,tt->account);
		printf("%10s�����ʺ�:%ld_%d"," ",tt->account,chk);
	}
	printf("%10s����:%s"," ",tt->card_acc);
	printf("%10s ����:%s"," ",tt->customer);
	newline(1);
	sprintf(amt,"%d",tt->amount);
	pramt(amt,parr,15);
	printf("%10s������:���ֱ�ֵ%s "," ",parr);
	newline(1);
	printf("%10s����Ա��:%d"," ",tt->operator);
	printf("%5s��ˮ��:%d"," ",tt->ser_num);
	newline(1);
  rjulmdy(tt->open_d,ymd);
  printf("%10s��������:%d��%d��%d��"," ",ymd[2],ymd[0],ymd[1]);
	newline(1);
	if(code!=1501&&code!=1591) 
	{
    rjulmdy(sys_work_d,ymd);
	  printf("%10sҵ����:%d��%d��%d��"," ",ymd[2],ymd[0],ymd[1]);
	  newline(1);
  }
	
	backpage();
	closelp();
}

/*--------------------------
  ��ӡ���ۿ�������
--------------------------*/
prt_h_z(tt1)
struct s_trade *tt1;
{
	float rate;
	char qq;
	char  parr[24];

	openlp();

	//set_right();
  chk=get_chk_no(tt1->open_br,tt1->account);
  printf("%10s%ld_%d   "," ",tt1->account,chk);
  printf("%20s%s"," ",tt1->customer);
	//un_right();
	newline(10);
  rjulmdy(tt1->open_d,ymd);
	printf("%12s%d��%d��%d��"," ",ymd[2],ymd[0],ymd[1]);
	newline(6);
	printf("%1s%d%d%d"," ",ymd[2],ymd[0],ymd[1]);
  printf("  ��  ");
	sprintf(amt,"%d",tt1->amount);
	pramt(amt,parr,15);
	printf("+%15s",parr);
	printf("%16s",parr);
	printf("   %2d",tt1->operator);
	newline(1);
	backpage();
	closelp();
}

/*--------------------------
  ����
--------------------------*/
prt_h_nz(tt1)
struct s_trade *tt1;
{
	float rate;
	char qq;
	char  parr[24];

	openlp();

	//set_right();
  chk=get_chk_no(tt1->open_br,tt1->account);
  printf("%10s%ld_%d   "," ",tt1->account,chk);
  printf("%20s%s"," ",tt1->customer);
	//un_right();
	newline(10);
  rjulmdy(tt1->open_d,ymd);
	printf("%12s%d��%d��%d��"," ",ymd[2],ymd[0],ymd[1]);
	newline(6);
	printf("%1s%d%d%d"," ",ymd[2],ymd[0],ymd[1]);
  printf(" ���� ");
	sprintf(amt,"%d",tt1->amount);
	pramt(amt,parr,15);
	printf("+%15s",parr);
	sprintf(amt,"%d",tt1->balance);
	pramt(amt,parr,15);
	printf("%16s",parr);
	printf("   %2d",tt1->operator);
	newline(1);
	backpage();
	closelp();
}

/*-------------------------------------------
  ��ӡ���������桢ȡ�Ĵ���
-------------------------------------------*/
prt_h_cq(tt2,code)
struct s_trade *tt2;
short code;
{
	long camt,bookbal;
	short prev_line,prt_line,unp_line;
	char  tmp_bal1[24],parr[24]; 
 	long  bal_amt,atol();
	char  sbal_amt[12];

	if(tt2->det_num==91)dis_err("�뻻���ۺ󲹴�!");
	prev_line=tt2->det_num%90;	 /**�ϱ��к�**/
  sprintf(sbal_amt,"%ld",tt2->balance);
  sbal_amt[11]='\0';
  pramt(sbal_amt,parr,12);
  
	if(prev_line==11||prev_line==31||prev_line==51 || prev_line==71 )
		dis_err("�뷭ҳ��������!");
/**
		else
			dis_err("��������!");
**/
		openlp();
		prev_line=prev_line%20;
	  if(prev_line<=10)
		{
    		  newline(GO_LINE1);
		}
		else
		{
    		  newline(GO_LINE2);
		}
		if(prev_line>10)prev_line-=10;
		newline(prev_line);
  	rjulmdy(tt2->open_d,ymd);
	  printf("%1s%d%d%d"," ",ymd[2],ymd[0],ymd[1]);
    printf("  ");
		sprintf(amt,"%d",tt2->amount);
   	pramt(amt,parr,15);
		switch(code) 
		{
			case 1503:
				printf("��  +%15s",parr);
				break;
			case 1504:
				printf("ȡ  -%15s",parr);
				break;
			case 1502:
				printf("��  -%15s",parr);
				break;
		}
		sprintf(amt,"%d",tt2->balance);
   	pramt(amt,parr,15);
		printf("%16s",parr);
		printf("   %2d",tt2->operator);
		newline(1);
		backpage();
		closelp();
}

prt_h_g(tt3,code,lc)
struct s_trade *tt3;
short code;
short lc;
{
	openlp();
	newline(1);
	if(lc==1){
	  printf("%10s�ͻ�����"," ");
	}else if(lc==2){
	  printf("%10s��������"," ");
	}
	newline(2);
	if(code==1) 
	  printf("%20s�����������й�ʧ��"," ");
	else if(code==2) 
	  printf("%20s�����������н�ҵ�"," ");
	newline(1);
	 printf("%15s==================================="," ");
	printf("  %10s%ld%17s"," ",tt3->item_num," ");
	newline(1);
        chk=get_chk_no(tt3->open_br,tt3->account);
	newline(1);
	if (tt3->acc_type[0]=='P')
		printf("%10s�����ʺ�:%ld_%d"," ",tt3->account,chk);
	else
		printf("%10s����:%s"," ",tt3->card_acc);
	printf("%10s ����:%s"," ",tt3->customer);
	newline(1);
	printf("%10s����Ա��:%d"," ",tt3->operator);
	printf("%5s������ˮ��:%d"," ",tt3->ser_num);
	newline(1);
  //rjulmdy(tt3->open_d,ymd);
  rjulmdy(sys_work_d,ymd);
	printf("%10sҵ����:%d��%d��%d��"," ",ymd[2],ymd[0],ymd[1]);
	newline(1);
	backpage();
	closelp();
}

prt_h_ct()
{
	openlp();
	newline(5);
	printf("             ********    ��ҵ��������!      ********");
	backpage();
	closelp();
}

/*---------------------------------------------------------
  ��ӡ���ۿ��������桢ȡ�����롢�ǡ��������Ĵ��۳���
---------------------------------------------------------*/
prt_h_cz(tt4,code)
struct s_trade *tt4;
short code;
{
		short prev_line;

		if(tt4->det_num==91)dis_err("�뻻���ۺ󲹴�!");
		prev_line=(tt4->det_num)%90;	 /**�ϱ��к�**/

		if(prev_line==11||prev_line==31||prev_line==51 || prev_line==71 )
			dis_err("�뷭ҳ��������!");
/**
		else
			dis_err("��������!");
**/
		openlp();
		prev_line=prev_line%20;
	  if(prev_line<10)
	  {
    		  newline(GO_LINE1);
		}
		else
		{
    		  newline(GO_LINE2);
		}
		if(prev_line>10)prev_line-=10;
		newline(prev_line);
	 printf("                   ******�ѳ���******");
	 backpage();
	 closelp();
}

prt_h_lx(tt5,code,lc)
struct s_trade *tt5;
short code;
short lc;
{
	char  parr[24], parr_int[24];
	long atol();
	int chk;

	openlp();
	newline(2);
	if(lc==1){
	  printf("%10s�ͻ�����"," ");
	}else if(lc==2){
	  printf("%10s��������"," ");
	}
	newline(2);
	if(code==1502) 
	  printf("%20s�����������л���������Ϣ��"," ");
	newline(1);
	 printf("%15s==================================="," ");
	newline(2);
	//intf("123-12345678-11212345678122001081212341231234567P12345123456  123");
        chk=get_chk_no(tt5->open_br,tt5->account);
	newline(1);
	if (tt5->acc_type[0]=='P')
		printf("%10s�����ʺ�:%ld_%d"," ",tt5->account,chk);
	else
		printf("%10s����:%s"," ",tt5->card_acc);
	printf("%10s ����:%s"," ",tt5->customer);
	newline(1);
	sprintf(amt,"%d",tt5->amount);
	pramt(amt,parr,15);
	printf("%10s���:���ֱ�ֵ%s "," ",parr);
	newline(1);
	sprintf(amt,"%d",tt5->s_int);
	pramt(amt,parr,15);
	printf("%10s��Ϣ:���ֱ�ֵ%s "," ",parr);
	newline(1);
	sprintf(amt,"%d",tt5->s_int+tt5->balance);
	pramt(amt,parr,15);
	printf("%10s�ϼ�:���ֱ�ֵ%s "," ",parr);
	newline(1);
	printf("%10s����Ա��:%d"," ",tt5->operator);
	printf("%5s������ˮ��:%d"," ",tt5->ser_num);
	newline(1);
  	rjulmdy(tt5->open_d,ymd);
	printf("%10s������:%d��%d��%d��"," ",ymd[2],ymd[0],ymd[1]);
	newline(1);
	backpage();
	closelp();
}


/** ���۲���*/
prt_bdcz(char *card_acc,short br) {
	
  struct ss_detail sd;
  short code;
	long camt,bookbal;
	short prev_line,prt_line,unp_line;
	short i=0,det_num,lpage,tpage,upc;
	char  tmp_bal1[24],parr[24]; 
 	long  bal_amt,atol();
	char  sbal_amt[12];
	char qq;
  
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  sprintf(sql,"SELECT count(*) FROM s_detail WHERE card_acc='%s' AND open_br=%d AND abstract='u' order by det_num",card_acc,br);
	if(mysql_query(conn,sql))
	{
	  sprintf(sys_err_str,"ȡ��ϸ��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
	}
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL)return(0);
	upc=atoi(row[0]);
    mysql_free_result(res);
	
  sprintf(sql,"SELECT * FROM s_detail WHERE card_acc='%s' AND open_br=%d AND abstract='u' order by det_num",card_acc,br);
  
	if(mysql_query(conn,sql))
	{
	  sprintf(sys_err_str,"ȡ��ϸ��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
	}
	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	
	if(row==NULL)return(0);
		
  mvprintw(22,16,"������ۣ��������ۺ���һ��,q�˳�!     ");
  refresh();
  qq=getch();
  if (qq=='Q' || qq=='q') {
    mysql_free_result(res);
    return(0);
  }
	
	openlp();
  
  for(;row!=NULL;row=mysql_fetch_row(res))
  {
    set_ss_detail(&sd,row);
    i++;
    code=0;
    if (sd.flag[0]=='O')
      code=1501;
    else if (sd.flag[0]=='D')
      code=1503;
    else if (sd.flag[0]=='W')
      code=1504;
      
		  det_num=sd.det_num-(--upc);
			if(i==1){
        tpage=lpage=det_num/20;
      }else{
      	tpage=det_num/20;
      }
		  
			if(det_num==91)dis_err("�뻻���ۺ󲹴�!");
			prev_line=det_num%90;	 /**�ϱ��к�**/
		  sprintf(sbal_amt,"%ld",sd.balance);
		  sbal_amt[11]='\0';
		  pramt(sbal_amt,parr,12);
		  
			if(tpage!=lpage||prev_line==11||prev_line==31||prev_line==51 || prev_line==71 ) {
	      if(i>1){
	      	backpage();
	      }
				dis_err("�뷭ҳ��������!");
	      if(i>1){
		      newline(GO_LINE2);
	      }
	    }
	    lpage=tpage;
			if(i==1){
				prev_line=prev_line%20;
			  if(prev_line<=10)
				{
		    		  newline(GO_LINE1);
				}
				else
				{
		    		  newline(GO_LINE2);
				}
			  if(prev_line>10)prev_line-=10;
		   	newline(prev_line);
		  }
	  	rjulmdy(sd.tr_date,ymd);
		  printf("%1s%d%d%d"," ",ymd[2],ymd[0],ymd[1]);
	    printf("  ");
			sprintf(amt,"%d",sd.amount);
	   	pramt(amt,parr,15);
			switch(code) 
			{
				case 1503:
					printf("����  +%13s",parr);
					break;
				case 1504:
					printf("��ȡ  -%13s",parr);
					break;
				case 1502:
					printf("��  -%15s",parr);
					break;
			}
			sprintf(amt,"%ld",sd.balance);
	   	pramt(amt,parr,15);
			printf("%16s",parr);
			printf("   %2d",sd.tr_op);
			newline(1);    
  }
  mysql_free_result(res);
  
	backpage();
	closelp();
  
  
  sprintf(sql,"UPDATE s_detail set abstract='0' WHERE card_acc='%s' AND open_br=%d AND abstract='u'",
    card_acc,br);
   
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
  }
  
}
