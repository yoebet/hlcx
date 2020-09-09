/****************************************************************
������:c_close.ec
������:c_close()
��  ��:������
��  ��:1502
��  ��:  
      1.����
        �� (card_acc)
      2.ȡ�����ķֻ���
      3.�˶�
        ���� (customer)
        ��ַ (address)
      4.���ֻ��е���������
      5.������Ϣ,���˶�
      6.ȡ��ˮ��
      7.�޸ķֻ�
      8.д��־
      9.������ˮ��
      A.д��ϸ
           
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

c_close()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  char yy,qq;
  char str2[100];
  short s_day,ed[3];
  long interest;
  double int_d;
  static short in[3]={71,72,8};
  short num=3,flag;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.tr_br=sys_branch;
while(1)
{
  clear_scr(5,22,22,77);
  mvaddstr(5,30,"����������");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

  init_s(&ss);
  init_t(&tt);

  note("��鱾���Ƿ����!");
  
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
    dis_str(22,22,"���ʻ�������!",0,0);
    getch();
    continue;
  }
  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  if (ss.acc_flag[0]!='A')
  {
    dis_str(22,22,"���ʻ��ѷ�������!",0,0);
    getch();
    continue;
  }

  mvprintw(7+num,30,"��    ��:[%12s]",ss.customer);
  mvprintw(8+num,30,"��    ַ:[%30s]",ss.address);
  mvprintw(9+num,30,"��    ��:[%ld]",ss.balance/100);
   mvprintw(10+num,30,"֤������:[%s]",ss.zj_num);

  dis_str(22,22,"��˶Ի���/��ַ��,�� q �˳�,����������!",0,0);
  refresh();
  yy=getch();
  if (yy=='q')
    continue;
  dis_str(22,22,"                                       ",0,0);
  
  note("����������Ϣ!");
  rjulmdy(sys_work_d,ed);
	tt.amount=ss.balance;
   interest=s_int(ed,tt.amount);
   interest=ss.interest-interest;
   int_d=(double)(interest)*(double)(get_rate('H',0))/36000;
   interest=(long)(int_d);
  tt.s_int=interest;
  tt.interest=ss.interest;
  ss.tax_int=0;
  tt.tax_int=ss.tax_int;
  tt.close_d=sys_work_d;
  tt.operator=sys_operator;

  da.amount=ss.balance;
  note("Ϊ���ֻ��޸��ֵ!");
  ss.tax_int=da.tax_int;
  ss.acc_close_d=sys_work_d;	
  strcpy(ss.acc_flag,"C");
  ss.close_br=da.tr_br;
  ss.close_op=sys_operator;
  ss.det_num++;

  note("Ϊ��־��ֵ!");
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_code=1502;
  tt.amount=ss.balance;
  strcpy(tt.correct,"A");
  strcpy(tt.type,"H");
  tt.term=0;
  tt.open_br=da.open_br;
  strcpy(tt.card_acc,da.card_acc);
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,ss.customer);
  strcpy(tt.abstract,"0");
  tt.open_d=ss.acc_open_d;
  tt.close_d=sys_work_d;
  tt.balance=ss.balance;
  tt.item_num=ss.item_num;
  tt.det_num=ss.det_num;
  tt.account=0;
  tt.transfer=0;

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"������������");
    dis_err(sys_err_str);
    return(-1);
  }

  note("ȡ��ˮ��!");
  if ((tt.ser_num = get_ser(da.open_br)) < 0)
  {
    sprintf(sys_err_str,"ȡ��־�Ŵ�!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    continue;
  }
  note("�޸Ŀ��ֻ�!");
  
  set_ss_mast_u_sql(&ss,sql);
  char cond[40];
	sprintf(cond," open_br=%d and card_acc='%s'",ss.open_br,ss.card_acc);
	strcat(sql,cond);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"�޸Ŀ��ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  
  note("д��־!");
  
  set_s_trade_i_sql(&tt,sql);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"д��־�ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  
  note("д������ϸ!");
  da.tr_op=sys_operator;
  
  
  sprintf(sql,"insert into s_detail values(%d,0,'%s',%d,%ld,%ld,'%s','C',%d,%d,0)",
    da.open_br,da.card_acc,ss.det_num,ss.last_day,da.amount,da.abstract,da.tr_br,da.tr_op);
    
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"д����ϸ�ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  mysql_commit(conn);
  
  mvprintw(1,68,"��ˮ��:%d",tt.ser_num);
  refresh();
  mvprintw(22,22,"���ڶ�����ָʾ�����󻮿�д����!");
  refresh();
  //_CutSpace(ss.card_acc);
  sprintf(str2,"%19s'%09ld'%05d",ss.card_acc, ss.balance, ss.det_num);
  write_msr(str2,1);
  
  mvprintw(22,22,"�����ȡ��ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_card(&tt,1502,1);
  mvprintw(22,22,"���ٲ���ȡ��ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_card(&tt,1502,2);
	
  /*
  mvprintw(22,22,"�������Ϣƾ������һ��,q�˳�!          ");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
    prt_h_lx(&tt,1502,1);
  mvprintw(22,22,"���ٲ�����Ϣƾ������һ��,q�˳�!          ");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
    prt_h_lx(&tt,1502,2);
  */

  break;
}
  return(0);

}
