/****************************************************************
程序名:s_lost.ec
函数名:s_lost()
功  能:活期挂失/冻结
代  码:1511
流  程:  
      1.输入
        卡号 (card_acc)
        户名 (name)
      2.取指定卡号的分户
      3.检查分户状态及上述录入各项
      4.修改分户状态
      5.写特殊操作表
      6.写日志
           
 ****************************************************************/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "../include/sh_table.h"
#include "mysql.h"

extern char sys_err_str[96];
extern char sys_op_name[9];
extern short sys_branch,sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

c_lost()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  static short in[2]={66,8};
  short num=2,flag;
  short br,op,op_no;
  long day;
  char qq;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  da.tr_br=sys_branch;
  strcpy(da.acc_type,"2");
while(1)
{
  clear_scr(5,22,22,77);
  mvaddstr(5,22,"《卡帐户挂失》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  init_s(&ss);
  init_t(&tt);

  br=sys_branch;
  op=sys_operator;

  note("取对应分户记录!");
  
	sprintf(sql,"SELECT * FROM s_mast WHERE card_acc='%s' and open_br=%d",da.card_acc,da.open_br);
	mysql_query(conn,sql);
	
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    continue;
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  
  if(row==NULL){
    dis_str(22,22,"本卡号不存在!",0,0);
    getch();
    continue;
  }
  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  if (ss.acc_flag[0]!='A')
  {
    dis_str(22,22,"对应分户非正式户!",0,0);
    getch();
    continue;
  }

  mvprintw(7+num,30,"户    名:[%12s]",ss.customer);
  mvprintw(8+num,30,"地    址:[%30s]",ss.address);
   mvprintw(9+num,30,"证件号码:[%s]",ss.zj_num);

  dis_str(22,22,"请核对户名/地址后,按 q 退出,其它键继续!",0,0);
	refresh();
  qq=getch();
  if (qq=='q')
    continue;
  
  dis_str(22,22,"                                       ",0,0);

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"不能启动事务");
    dis_err(sys_err_str);
    return(-1);
  }
  
	sprintf(sql,"update s_mast set acc_flag='L' WHERE card_acc='%s' and open_br=%d",da.card_acc,da.open_br);

  if (mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"修改帐户错!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  
  note("为日志赋值!");
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_code=1511;
  tt.amount=da.amount;
  strcpy(tt.type,"H");
  tt.open_br=da.open_br;
  strcpy(tt.card_acc,da.card_acc);
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,ss.customer);
  //tt.open_d=ss.acc_open_d;
  tt.open_d=sys_work_d;
  tt.balance=da.amount;
  tt.interest=ss.interest;
  tt.item_num=ss.item_num;
  tt.det_num=ss.det_num;
  note("取流水号!");
  if ((tt.ser_num = get_ser(da.open_br)) < 0)
  {
    sprintf(sys_err_str,"取日志号错!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
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
  mysql_commit(conn);
  
  mvprintw(1,68,"流水号:%d",tt.ser_num);
  mvprintw(22,20,"请插入挂失凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_g(&tt,1,1);
  mvprintw(22,20,"请再插入挂失凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_g(&tt,1,2);

    break;
}
  return(0);
}
