/**************************************************************
程序名:pr_account.ec           
函数名:pr_account()           
功  能:打印本日各所各储种分户清单
**************************************************************/
#include <stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern short sys_operator;    
extern long sys_today;
extern MYSQL *conn;
extern char sql[];

short item_count,vou_count;

struct s_trade tt;
FILE *fp;
extern short br_id[50];
short b_count;

pr_account()
{
  char op_d[11],clo_d[11],fname[5],file[50],cmd_str[100],yy;
  char sbr[200],br_name[11];
  short page,count,ii,ymd[3];
  long  account,amount,open_d,close_d,item_num,interest;
  short det_num;
  char  st_d[11],en_d[11],customer[13],acc_flag[2],address[31];

  static short in[4]={4,22,23,8};
  short num=4,flag;
  struct in_data da;
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  rjulmdy(sys_today,ymd);

    clear_scr(5,22,22,77);
    mvaddstr(5,30,"《打印分户账》");
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);
    dis_str(22,22,"正在处理,请稍侯......",0,0);  

	da.open_br=100;
    sprintf(file,"./tmp/tmp_acc_%d_%1s",da.open_br,da.ltype);
    if((fp=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"不能生成分户文件/usr/hlcx/tmp/tmp_acc*!",60,0);
      getch();
      return(-1);
    }
    switch(da.ltype[0])
    {
      case 'D':
      case '2':
        strcpy(fname,"双整");
        sprintf(sbr,"select account,customer,amount,open_d,close_d,item_num,term,interest,acc_flag,address\
			from f_mast where open_br = %d  and account>= %d and account <= %d   ",da.open_br,da.account1,da.account2);
        break;
      case 'H':
      case '1':
        strcpy(fname,"活期");
        sprintf(sbr,"select account,customer,balance,acc_open_d,acc_close_d,item_num,det_num,interest,acc_flag,address\
			from s_mast where open_br = %d and account >= %d and account <=%d ",da.open_br,da.account1,da.account2);
        break;
      default:
        dis_err("储种选择错!");
        return(-1);
        break;
    }
	
	
	
	/*******************************/
	
	mysql_query(conn,sbr);
	  
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"%s.",mysql_error(conn));
	  dis_str(22,22,sys_err_str,60,0);
	  getch();
	  return(-1);
	}

	res=mysql_use_result(conn);
	
    page=1;
    fprintf(fp,"  分户清单  %30s第[%d]页\n\n"," ",page);
    fprintf(fp,"储种:%s            起始帐号:%8ld  终止帐号:%8ld             打印日期:%4d年%2d月%2d日\n",fname,da.account1,da.account2,ymd[2],ymd[0],ymd[1]);
    fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    fprintf(fp,"  帐号       户名       金额    开户日     销户日   科目号 存期/明细   利息    状态          地址\n");
    fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    count=0;
	
	for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
	{
	  if(mysql_errno(conn)!=0)
	    break;
		
	  account=atol(row[0]);
	  strcpy(customer,row[1]);
	  amount=atol(row[2]);
	  open_d=atol(row[3]);
	  close_d=atol(row[4]);
	  item_num=atol(row[5]);
	  det_num=atoi(row[6]);
	  interest=atol(row[7]);
	  strcpy(acc_flag,row[8]);
	  strcpy(address,row[9]);
	  
      if (acc_flag[0] == 'C' || acc_flag[0] == 'K' )
        continue;
   
      rfmtdate(open_d,"yyyy/mm/dd",op_d);
      if (close_d>5)
        rfmtdate(close_d,"yyyy/mm/dd",clo_d);
      else
        strcpy(clo_d,"     ");
      fprintf(fp,"%8ld  %12s %10.2lf %10s %10s %6ld    %2d    %8.2lf    %1s         %30s\n", account,customer,amount/100.00,op_d,clo_d,item_num,det_num,interest/100.00,acc_flag,address);
      count++;
      if (count%50==0)
      {
        fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        page++;
        for(ii=1;ii<=3;ii++)
          fprintf(fp,"\n");
        fprintf(fp,"   分户清单  %30s第[%d]页\n\n"," ",page);
        fprintf(fp,"储种:%s            起始帐号:%8ld  终止帐号:%8ld             打印日期:%4d年%2d月%2d日\n",fname,da.account1,da.account2,ymd[2],ymd[0],ymd[1]);
        fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        fprintf(fp,"  帐号       户名       金额    开户日     销户日   科目号 存期/明细   利息    状态          地址\n");
        fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
      }
    }
	  mysql_free_result(res);
  
    fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    fprintf(fp,"        累计笔数:[%d]\n",count);
    fclose(fp);
    if (count==0)
    {
      dis_err("无分户账,按任一键返回 !");
      return(0);
    }

    dis_str(22,22,"是否打印(y/n):[ ]",60,0);
    flag=yorn(22,44);
    if(flag=='y'||flag=='Y') 
    {
      dis_str(22,22,"请准备好打印机后按任一键开始打印",60,0);
      getch();       
      sprintf(cmd_str,"pr %s > "Printer, file);
      system(cmd_str);
    }
   return(0);
}
