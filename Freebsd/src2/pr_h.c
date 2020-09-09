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
    打印存折存款凭条
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
	  printf("%10s客户留存"," ");
	}else if(lc==2){
	  printf("%10s银行留存"," ");
	}
	newline(2);
	if(code==1501) 
	  printf("%20s快乐星期银行存折开户凭条"," ");
	else if(code==1502) 
	  printf("%20s快乐星期银行存折销户凭条"," ");
	else if(code==1503) 
	  printf("%20s快乐星期银行存折存款凭条"," ");
	else if(code==1504) 
	  printf("%20s快乐星期银行存折取款凭条"," ");
	else if(code==1591) 
	  printf("%20s快乐星期银行存折补卡凭条"," ");
	else if(code==1592) 
	  printf("%20s快乐星期银行换折凭条"," ");
	newline(1);
	 printf("%15s==================================="," ");
	newline(2);
	//printf("123-12345678-11212345678122001081212341231234567P12345123456  123");
  chk=get_chk_no(tt->open_br,tt->account);
	if (tt->acc_type[0]=='P')
		printf("%10s存折帐号:%ld_%d"," ",tt->account,chk);
	printf("%10s 户名:%s"," ",tt->customer);
	newline(1);
	sprintf(amt,"%d",tt->amount);
	pramt(amt,parr,15);
	printf("%10s发生额:快乐币值%s "," ",parr);
	newline(1);
	printf("%10s操作员号:%d"," ",tt->operator);
	printf("%5s流水号:%d"," ",tt->ser_num);
	newline(1);
  rjulmdy(tt->open_d,ymd);
	printf("%10s开户日:%d年%d月%d日"," ",ymd[2],ymd[0],ymd[1]);
	newline(1);
	if(code!=1501) 
	{
    rjulmdy(sys_work_d,ymd);
	  printf("%10s业务日:%d年%d月%d日"," ",ymd[2],ymd[0],ymd[1]);
	  newline(1);
  }
	
	backpage();
	closelp();
}

/*-----------------------
    打印卡存款凭条
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
	  printf("%10s客户留存"," ");
	}else if(lc==2){
	  printf("%10s银行留存"," ");
	}
	newline(2);
	if(code==1501) 
	  printf("%20s快乐星期银行卡开户凭条"," ");
	else if(code==1502) 
	  printf("%20s快乐星期银行卡销户凭条"," ");
	else if(code==1503) 
	  printf("%20s快乐星期银行卡存款凭条"," ");
	else if(code==1504) 
	  printf("%20s快乐星期银行卡取款凭条"," ");
	else if(code==1591) 
	  printf("%20s快乐星期银行存折补卡凭条"," ");
	newline(1);
	 printf("%15s==================================="," ");
	newline(2);
	//printf("123-12345678-11212345678122001081212341231234567P12345123456  123");
	if(code==1591) {
    chk=get_chk_no(tt->open_br,tt->account);
		printf("%10s存折帐号:%ld_%d"," ",tt->account,chk);
	}
	printf("%10s卡号:%s"," ",tt->card_acc);
	printf("%10s 户名:%s"," ",tt->customer);
	newline(1);
	sprintf(amt,"%d",tt->amount);
	pramt(amt,parr,15);
	printf("%10s发生额:快乐币值%s "," ",parr);
	newline(1);
	printf("%10s操作员号:%d"," ",tt->operator);
	printf("%5s流水号:%d"," ",tt->ser_num);
	newline(1);
  rjulmdy(tt->open_d,ymd);
  printf("%10s卡开户日:%d年%d月%d日"," ",ymd[2],ymd[0],ymd[1]);
	newline(1);
	if(code!=1501&&code!=1591) 
	{
    rjulmdy(sys_work_d,ymd);
	  printf("%10s业务日:%d年%d月%d日"," ",ymd[2],ymd[0],ymd[1]);
	  newline(1);
  }
	
	backpage();
	closelp();
}

/*--------------------------
  打印存折开户存折
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
	printf("%12s%d年%d月%d日"," ",ymd[2],ymd[0],ymd[1]);
	newline(6);
	printf("%1s%d%d%d"," ",ymd[2],ymd[0],ymd[1]);
  printf("  开  ");
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
  换折
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
	printf("%12s%d年%d月%d日"," ",ymd[2],ymd[0],ymd[1]);
	newline(6);
	printf("%1s%d%d%d"," ",ymd[2],ymd[0],ymd[1]);
  printf(" 换折 ");
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
  打印存折销、存、取的存折
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

	if(tt2->det_num==91)dis_err("请换存折后补打!");
	prev_line=tt2->det_num%90;	 /**上笔行号**/
  sprintf(sbal_amt,"%ld",tt2->balance);
  sbal_amt[11]='\0';
  pramt(sbal_amt,parr,12);
  
	if(prev_line==11||prev_line==31||prev_line==51 || prev_line==71 )
		dis_err("请翻页后插入存折!");
/**
		else
			dis_err("请插入存折!");
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
				printf("存  +%15s",parr);
				break;
			case 1504:
				printf("取  -%15s",parr);
				break;
			case 1502:
				printf("销  -%15s",parr);
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
	  printf("%10s客户留存"," ");
	}else if(lc==2){
	  printf("%10s银行留存"," ");
	}
	newline(2);
	if(code==1) 
	  printf("%20s快乐星期银行挂失单"," ");
	else if(code==2) 
	  printf("%20s快乐星期银行解挂单"," ");
	newline(1);
	 printf("%15s==================================="," ");
	printf("  %10s%ld%17s"," ",tt3->item_num," ");
	newline(1);
        chk=get_chk_no(tt3->open_br,tt3->account);
	newline(1);
	if (tt3->acc_type[0]=='P')
		printf("%10s存折帐号:%ld_%d"," ",tt3->account,chk);
	else
		printf("%10s卡号:%s"," ",tt3->card_acc);
	printf("%10s 户名:%s"," ",tt3->customer);
	newline(1);
	printf("%10s操作员号:%d"," ",tt3->operator);
	printf("%5s交易流水号:%d"," ",tt3->ser_num);
	newline(1);
  //rjulmdy(tt3->open_d,ymd);
  rjulmdy(sys_work_d,ymd);
	printf("%10s业务日:%d年%d月%d日"," ",ymd[2],ymd[0],ymd[1]);
	newline(1);
	backpage();
	closelp();
}

prt_h_ct()
{
	openlp();
	newline(5);
	printf("             ********    本业务已作废!      ********");
	backpage();
	closelp();
}

/*---------------------------------------------------------
  打印存折开、销、存、取、插入、登、补开户的存折冲正
---------------------------------------------------------*/
prt_h_cz(tt4,code)
struct s_trade *tt4;
short code;
{
		short prev_line;

		if(tt4->det_num==91)dis_err("请换存折后补打!");
		prev_line=(tt4->det_num)%90;	 /**上笔行号**/

		if(prev_line==11||prev_line==31||prev_line==51 || prev_line==71 )
			dis_err("请翻页后插入存折!");
/**
		else
			dis_err("请插入存折!");
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
	 printf("                   ******已冲正******");
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
	  printf("%10s客户留存"," ");
	}else if(lc==2){
	  printf("%10s银行留存"," ");
	}
	newline(2);
	if(code==1502) 
	  printf("%20s快乐星期银行活期销户利息单"," ");
	newline(1);
	 printf("%15s==================================="," ");
	newline(2);
	//intf("123-12345678-11212345678122001081212341231234567P12345123456  123");
        chk=get_chk_no(tt5->open_br,tt5->account);
	newline(1);
	if (tt5->acc_type[0]=='P')
		printf("%10s存折帐号:%ld_%d"," ",tt5->account,chk);
	else
		printf("%10s卡号:%s"," ",tt5->card_acc);
	printf("%10s 户名:%s"," ",tt5->customer);
	newline(1);
	sprintf(amt,"%d",tt5->amount);
	pramt(amt,parr,15);
	printf("%10s余额:快乐币值%s "," ",parr);
	newline(1);
	sprintf(amt,"%d",tt5->s_int);
	pramt(amt,parr,15);
	printf("%10s利息:快乐币值%s "," ",parr);
	newline(1);
	sprintf(amt,"%d",tt5->s_int+tt5->balance);
	pramt(amt,parr,15);
	printf("%10s合计:快乐币值%s "," ",parr);
	newline(1);
	printf("%10s操作员号:%d"," ",tt5->operator);
	printf("%5s交易流水号:%d"," ",tt5->ser_num);
	newline(1);
  	rjulmdy(tt5->open_d,ymd);
	printf("%10s开户日:%d年%d月%d日"," ",ymd[2],ymd[0],ymd[1]);
	newline(1);
	backpage();
	closelp();
}


/** 存折补打*/
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
	  sprintf(sys_err_str,"取明细错!%s",mysql_error(conn));
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
	  sprintf(sys_err_str,"取明细错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
	}
	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	
	if(row==NULL)return(0);
		
  mvprintw(22,16,"补打存折，请插入存折后按任一键,q退出!     ");
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
		  
			if(det_num==91)dis_err("请换存折后补打!");
			prev_line=det_num%90;	 /**上笔行号**/
		  sprintf(sbal_amt,"%ld",sd.balance);
		  sbal_amt[11]='\0';
		  pramt(sbal_amt,parr,12);
		  
			if(tpage!=lpage||prev_line==11||prev_line==31||prev_line==51 || prev_line==71 ) {
	      if(i>1){
	      	backpage();
	      }
				dis_err("请翻页后插入存折!");
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
					printf("卡存  +%13s",parr);
					break;
				case 1504:
					printf("卡取  -%13s",parr);
					break;
				case 1502:
					printf("销  -%15s",parr);
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
