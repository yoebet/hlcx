/****************************************************************
函数名:c_open()
功  能:存折补卡
代  码:1591
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

char qq;

c_scard()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  short flag_k,ymd[3];
  char a_flag[2];
  static short in[9]={70,66,72,8};
  short num=4,flag;
  char str2[100];
  char yy;
  short mday[3];
  short chk;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;

  da.open_d=sys_work_d;
  da.tr_br=sys_branch;

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《存折补卡》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)return(0);

  init_s(&ss);
  init_t(&tt);

  da.open_d=sys_work_d;
  
  note("检查本户是否存在!");
  
	sprintf(sql,"SELECT * FROM s_mast WHERE account=%ld and open_br=%d",da.account,da.open_br);
	mysql_query(conn,sql);
	
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  
  if(row==NULL){
    dis_str(22,22,"本帐号不存在!",0,0);
    getch();
    return(-1);
  }
  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  note("检查有关项是否与分户一致!");
  if (ss.acc_flag[0]!='A')
  {
    dis_str(22,22,"本帐户已非正常户!",0,0);
    getch();
    return(-1);
  }

  cry_pwd(da.pwd_new1,da.pwd_new2,1); 
  da.pwd_new2[6]='\0';
  if(strcmp(ss.passwd,da.pwd_new2)!=0) 
  {
    dis_err("密码不符!");
    return(-1);
  }
  if(strcmp(ss.card_acc,"")!=0) 
  {
    dis_err("本折已办卡!");
    return(-1);
  }
  
  mvprintw(7+num,30,"户    名:[%12s]",ss.customer);
  mvprintw(8+num,30,"地    址:[%30s]",ss.address);
  mvprintw(9+num,30,"证件号码:[%s]",ss.zj_num);
  dis_str(22,22,"请核对户名/地址后,按 q 退出,其它键继续!",0,0);
	refresh();
  yy=getch();
  if (yy=='q')
    return(-1);
  dis_str(22,22,"                                       ",0,0);
  

  /***************************************/

  
  if(mysql_query(conn,"START TRANSACTION")){
    dis_err("不能启动事务");
    return(-1);
  }
  
  note("取流水号!");
  tt.ser_num = get_ser(da.open_br);
  if (tt.ser_num < 0)
  {
    sprintf(sys_err_str,"取日志号错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  sprintf(sql,"update s_mast set card_acc='%s',card_open_d=%ld where open_br=%d and account=%ld",
    da.card_acc,da.open_d,da.open_br,da.account);
    
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"写卡文件错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
  }
  
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_op=da.tr_op;
  tt.tr_code=1591;
  tt.account=ss.account;
  tt.amount=da.amount;
  strcpy(tt.correct,"A");
  strcpy(tt.type,"K");
  tt.open_br=da.open_br;
  strcpy(tt.card_acc,da.card_acc);
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,ss.customer);
  strcpy(tt.abstract,"0");
  tt.open_d=da.open_d;
  
  if (update_vou(da.open_br, "K0", 'O')<0)
  {
    sprintf(sys_err_str,"修改开户凭证错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  mvprintw(22,20,"请在读卡器指示灯亮后划卡写磁条!");
  refresh();
  sprintf(str2,"%19s'%09ld'%05d",da.card_acc, ss.balance, ss.det_num);
  
  if(write_msr(str2,1)!=0) {
    dis_str(22,22,"写磁条错!",60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  mysql_commit(conn);
  mvprintw(1,68,"流水号:%d",tt.ser_num);
  refresh();

  mvprintw(22,20,"请插入凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  	prt_h_card(&tt,1591,1);
  mvprintw(22,20,"请再插入凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  	prt_h_card(&tt,1591,2);

  return(0);
}
