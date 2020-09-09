/****************************************************************
程序名:zh_pro_vou.ec
函数名:zh_pro_vou()
功  能:调整凭证
代  码:1908
流  程:  
      1.输入
        凭证类型 (vou_type)
        发生笔数(sum_que) 
        凭证方向(vou_flag) 
        操作员号(open_op)
        业务所号 (tr_br 回车取登录所号)
      2.检查凭证是否存在
      3.写日志
      4.写凭证文件
 ****************************************************************/

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

zh_pro_vou()
{
  struct f_vouch sum;
  struct s_trade tt;
  struct in_data da;
  char yy;
  static short in[4]={5,38,37,8};
  short num=4,flag;
  char vou_flag[2],vou_type[3];
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  strcpy(da.acc_type,"M");
  da.tr_br=sys_branch;
  da.tr_op=sys_operator;
  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"《凭证调整》");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    init_t(&tt);

    sprintf(tt.abstract,"%d",da.vou_flag+4);
    note("检查凭证是否存在!");
    switch (da.vou_type[0])
    {
       case '2':
         strcpy(vou_type,"H0");
         strcpy(tt.type,"H");
    strcpy(tt.correct,"H");
         break;
       case '3':
         strcpy(vou_type,"K0");
         strcpy(tt.type,"K");
    strcpy(tt.correct,"H");
         break;
       default:
         dis_str(22,22,"输入凭证类型错!",0,0); 
         break;
    }
    
	  sprintf(sql,"select flag from vouch where branch=%d and vou_type='%s' and kind=90",da.open_br,vou_type);
	  mysql_query(conn,sql);
	  if(mysql_errno(conn))
	  {
      sprintf(sys_err_str,"取凭证错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  if(row==NULL)
	  {
      sprintf(sys_err_str,"无此凭证!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  strcpy(vou_flag,row[0]);
    mysql_free_result(res);

    if (vou_flag[0]=='K')
    {
      dis_str(22,22,"本凭证已作废",60,0);
      getch();
      continue;
    }
    note("为日志赋值!");
    tt.branch=da.tr_br;
    tt.operator=sys_operator;
    tt.tr_op=da.tr_op;
    tt.tr_code=1908;
    strcpy(tt.type,"M");
    tt.open_br=da.open_br;
    tt.account=da.account;
    strcpy(tt.acc_type,da.acc_type);
    tt.open_d=da.open_d;
    note("调整方向");
    tt.det_num=da.vou_flag;
    note("发生笔数");
    tt.term=da.sum_que;

/***********************************/

    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"不能启动事务");
      dis_err(sys_err_str);
      return(-1);
    }
    
    note("取流水号!");
    if ((tt.ser_num = get_ser(da.open_br)) < 0)
    {
      sprintf(sys_err_str,"取日志号错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
    note("写日志!");
    set_s_trade_i_sql(&tt,sql);
    if(mysql_query(conn,sql)||(mysql_errno(conn)))
    {
      sprintf(sys_err_str,"写日志文件错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      continue;
    }
    note("修改凭证!");
    if (da.vou_flag==1)
    {
      note("领回");
	    sprintf(sql,"update vouch set rest_que=rest_que+%d,takeback=takeback+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (da.vou_flag==2)
    {
      note("使用");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d,use_que=use_que+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (da.vou_flag==3)
    {
      note("作废");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d,cancel=cancel+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (da.vou_flag==4)
    {
      note("上缴");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d,takeback=takeback+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (da.vou_flag==5)
    {
      note("换折");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d,chg_book=chg_book+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    if (mysql_query(conn,sql)||(mysql_errno(conn)))
    {
      sprintf(sys_err_str,"修改凭证文件错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      continue;
    }
    mysql_commit(conn);

    mvprintw(1,68,"流水号:%d",tt.ser_num);
    refresh();
    dis_err("处理完毕! ");  
      break;
  }
  return(0);
}
