/**************************************************************
程序名:pr_detail.ec           
函数名:pr_detail()           
功  能:打印本日各所各储种明细清单
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
  
pr_detail()
{
  static short in[3]={22,23,8};
  short num=3,flag;
  struct in_data da;
  FILE *fp;
  char tr_d[11],fname[5],file[50],cmd_str[100],yy;
  char sbr[180],br_name[11];
  short page,count,ii,ymd[3];
  long  account,amount,tr_date;
  short det_num,tr_br,tr_op;
  char  det_flag[2];
  char  det_type[3];
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  rjulmdy(sys_today,ymd);

  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"《打印明细清单》");
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);
    clear_line(22,22,77);
    dis_str(22,22,"正在处理,请稍侯......",0,0);  

	/****************************/
    da.open_br=100;
	
	sprintf(sql,"SELECT br_name FROM br_file WHERE  branch= %d ",da.open_br);
  	
    mysql_query(conn,sql);
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
	  strcpy(br_name,row[0]);
    }
    mysql_free_result(res);
  
    if(row==NULL)
    {
      sprintf(sys_err_str,"取[%d]的所记录错!%s.",da.open_br,mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
	
	/****************************/
	
    sprintf(file,"./tmp/tmp_det_%d",da.open_br);
    if((fp=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"不能生成明细文件!",60,0);
      getch();
      return(-1);
    }
    strcpy(da.ltype,"2");
    switch(da.ltype[0])
    {
      case '2':
        sprintf(sbr,"select account,det_num,tr_date,amount,flag,tr_br,tr_op from s_detail\
			where open_br = %d and account >= %d and account <= %d",da.open_br,da.account1,da.account2);
        strcpy(fname,"活期");
        break;
      default:
        dis_str(22,22,"储种选择错!",60,0);
        getch();
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
    fprintf(fp,"   明细清单  %20s第[%d]页\n\n"," ",page);
    fprintf(fp,"            起始帐号:%8ld  终止帐号:%8ld \n            打印日期:%4d年%2d月%2d日\n",da.account1,da.account2,ymd[2],ymd[0],ymd[1]);
    fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    fprintf(fp,"  帐号    序号    业务日         金额   状态 业务所号 业务柜员\n");
    fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    count=0;
	
	for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
	{
	  if(mysql_errno(conn)!=0)
	    break;
	  account=atol(row[0]);
	  det_num=atoi(row[1]);
	  tr_date=atol(row[2]);
	  amount=atol(row[3]);
	  strcpy(det_flag,row[4]);
	  tr_br=atoi(row[5]);
	  tr_op=atoi(row[6]);
	  
      if (det_flag[0]=='D')
        strcpy(det_type,"存款");
      else if (det_flag[0]=='W')
        strcpy(det_type,"取款");
      else if (det_flag[0]=='O')
        strcpy(det_type,"开户");
      else if (det_flag[0]=='C')
        strcpy(det_type,"销户");
      else if (det_flag[0]=='K')
        strcpy(det_type,"冲正");
      else if (det_flag[0]=='Q')
        strcpy(det_type,"扣税");
      else if (det_flag[0]=='I')
        strcpy(det_type,"利息");
      else 
        strcpy(det_type,det_flag);
   
      rfmtdate(tr_date,"yyyy/mm/dd",tr_d);
      fprintf(fp,"%8ld  %2d    %10s  %10.2lf  %4s     %3d      %2d\n",account,det_num,tr_d,amount/100.00,det_type,tr_br,tr_op);
      count++;
      if (count%50==0)
      {
        fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        page++;
        for(ii=1;ii<=3;ii++)
          fprintf(fp,"\n");
    fprintf(fp,"   明细清单  %20s第[%d]页\n\n"," ",page);
    fprintf(fp,"            起始帐号:%8ld  终止帐号:%8ld \n            打印日期:%4d年%2d月%2d日\n",da.account1,da.account2,ymd[2],ymd[0],ymd[1]);
        fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        fprintf(fp,"  帐号    序号    业务日         金额   状态 业务所号 业务柜员\n");
        fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
      }
    }
	mysql_free_result(res);

    fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    fprintf(fp," 累计笔数:[%d]             打印日期:%4d年%2d月%2d日\n",count,ymd[2],ymd[0],ymd[1]);
    fclose(fp);
    dis_str(22,22,"是否打印(y/n):[ ]",60,0);
    flag=yorn(22,44);
    if(flag=='y'||flag=='Y') 
    {
      dis_str(22,22,"请准备好打印机后按任一键开始打印",60,0);
      getch();       
      sprintf(cmd_str,"pr %s > "Printer, file);
      system(cmd_str);
    }
      break;
  }
  return(0);
}
