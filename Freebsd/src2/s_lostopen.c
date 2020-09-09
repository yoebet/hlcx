/****************************************************************
������:s_lostopenopen.ec
������:s_lostopen()
��  ��:�۹�ʧ����
��  ��:1511
��  ��:  
      1.����
        ���� (card_acc)
        
      2.ȡָ�����ŵķֻ�
      3.���ֻ�״̬������¼�����
      4.�޸ķֻ�״̬
      5.д���������
      6.д��־
           
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

s_lostopen()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  static short in[2]={1,8};
  short num=2,flag;
  short br,op,op_no;
  long day;
  char qq;
  char t_str[7];
   
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
  mvaddstr(5,22,"�������ʻ���ʧ������");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  init_s(&ss);
  init_t(&tt);

  br=sys_branch;
  op=sys_operator;

  note("ȡ��Ӧ�ֻ���¼!");
  
	sprintf(sql,"SELECT * FROM s_mast WHERE card_acc='%s' and open_br=%d",da.account,da.open_br);
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
    dis_str(22,22,"ԭ�����ʻ�������!",0,0);
    getch();
    continue;
  }

  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  if (ss.acc_flag[0]!='A')
  {
    dis_str(22,22,"��Ӧ�ֻ�����ʽ��!",0,0);
    getch();
    continue;
  }

  mvprintw(7+num,30,"��    ��:[%12s]",ss.customer);
  mvprintw(8+num,30,"��    ַ:[%30s]",ss.address);
  mvprintw(9+num,30,"֤������:[%s]",ss.zj_num);

  dis_str(22,22,"��˶Ի���/��ַ��,�� q �˳�,����������!",0,0);
	refresh();
  qq=getch();
  if (qq=='q')
    continue;
  
  dis_str(22,22,"                                       ",0,0);

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"������������");
    dis_err(sys_err_str);
    return(-1);
  }

  if((da.account1=get_acc(da.open_br,'H'))<0)
   {
    sprintf(sys_err_str,"ȡ�����ʺŴ�!");
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
   }
   chk=get_chk_no(da.open_br,da.account1);
   if (chk<0)
   {
     dis_err("����У��λ��!");  
     mysql_rollback(conn);
  	 return(0);
   }
  mvprintw(7,30,"�¿��ʺ�:[%ld_%1d]",da.account1,chk);
  refresh();
  
  note("Ϊ���ڷֻ��޸��ֵ!");
  
	sprintf(sql,"update s_mast set acc_flag='C',transfer=%ld WHERE card_acc='%s' and open_br=%d",
		da.account1,da.card_acc,da.open_br);

  if (mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"�޸��ʻ���!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  
  note("Ϊ��־��ֵ!");

  tt.account=da.account1;
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_code=1511;
  tt.amount=da.amount;
  strcpy(tt.type,"H");
  tt.open_br=da.open_br;
  strcpy(tt.card_acc,da.card_acc1);
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,ss.customer);
  tt.open_d=ss.acc_open_d;
  tt.balance=ss.balance;
  tt.interest=ss.interest;
  tt.item_num=ss.item_num;
  tt.det_num=1;
  note("ȡ��ˮ��!");
  if ((tt.ser_num = get_ser(da.open_br)) < 0)
  {
    sprintf(sys_err_str,"ȡ��־�Ŵ�!%s.",mysql_error(conn));
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
  ss.account=da.account1;
  ss.det_num=1;

  ss.transfer= da.account;
  strcpy(ss.card_acc,da.card_acc1);
  strcpy(ss.acc_flag,"A");
  ss.open_op=sys_operator;
  ss.acc_close_d=0;
  ss.close_br=0;
  ss.close_op=0;
  note("д�ֻ�!");
  
  set_ss_mast_i_sql(&ss,sql);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"д���ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  note("д������ϸ!");
  da.tr_op=sys_operator;
  
  sprintf(sql,"insert into s_detail values(%d,%ld,0,%d,%ld,%ld,'0','O',%d,%d,%ld)",
    ss.open_br,da.account1,ss.det_num,ss.last_day,ss.balance,ss.tr_br,dd.tr_op,da.amount);
    
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"д������ϸ�ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  if (update_vou(da.open_br, "H0", 'O')<0)
  {
    sprintf(sys_err_str,"�޸Ŀ���ƾ֤��!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }



  mysql_commit(conn);
 
  mvprintw(1,68,"��ˮ��:%d",tt.ser_num);
  mvprintw(22,22,"���ڶ�����ָʾ�����󻮴���д����!");
  refresh();
  sprintf(str2,"%09ld'%09ld'%05d",ss.account, ss.balance, ss.det_num);
  write_msr(str2);

  mvprintw(22,22,"����뿪��ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  {
  	prt_h_t(&tt,1501,1);
  }
  mvprintw(22,22,"���ٲ��뿪��ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  {
  	prt_h_t(&tt,1501,2);
  }
  mvprintw(22,22,"�������ۺ���һ��,q�˳�!        ");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
    prt_h_z(&tt);


  break;
}
  return(0);
}
