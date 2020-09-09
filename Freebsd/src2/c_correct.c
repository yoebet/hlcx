/****************************************************************
������:s_correct.ec
������:s_correct()
��  ��:�����ճ���
��  ��:1581
��  ��:  
      1.����
        �ʺ� (card_acc)
        ��� (amount) 
        ���׺�(ser_num)
      2.ȡָ�����׺ŵ���־
      3.�����־״̬������¼�����
      4.ȡ���ʺŶ�Ӧ�ֻ�,�����ֻ��е���������
      5.����ԭ�������޸ķֻ�
      6.д���������
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

struct s_trade tt;

c_correct()
{
  struct ss_mast ss;
  struct in_data da;
  static short in[4]={66,2,11,8};
  short num=4,flag;
  short br,op,op_no;
  long day;
  char yy,qq;
  short mday[3];
  char str2[50];
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  da.tr_br=sys_branch;
  strcpy(da.acc_type,"P");
while(1)
{
  clear_scr(5,22,22,77);
  mvaddstr(5,30,"����ҵ���ճ�����");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  init_s(&ss);
  init_t(&tt);

  note("���ָ�����׺��Ƿ����!");
  br=da.open_br;
  op=sys_operator;
  
  sprintf(sql,"SELECT * FROM trade WHERE branch=%d AND card_acc='%s' AND operator=%d AND ser_num=%d",
    br,da.card_acc,op,da.ser_num);
    
	mysql_query(conn,sql);
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"ȡ�����״�!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    continue;
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  
  if(row==NULL){
    dis_str(22,22,"�����ײ�����!",0,0);
    getch();
    continue;
  }
  set_s_trade(&tt,row);
  mysql_free_result(res);
  
  note("�����־���й����Ƿ���¼��һ��!");
  if (tt.correct[0]!='A')
  {
    dis_str(22,22,"��־״̬��!",0,0);
    getch();
    continue;
  }

  if (tt.amount!=da.amount)
  {
    dis_str(22,22,"����־�н���!",0,0);
    getch();
    continue;
  }


  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"������������");
    dis_err(sys_err_str);
    return(-1);
  }

  note("�޸�ԭ��־Ϊ����!");
  
	sprintf(sql,"update trade set correct='K' where branch=%d and operator=%d and ser_num=%d",
	  br,op,da.ser_num);

  if (mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"�޸ı����״�!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }

  note("ȡ��־��Ӧ�ֻ���¼!");
  
  sprintf(sql,"SELECT * FROM s_mast WHERE card_acc='%s' AND open_br=%d",tt.card_acc,tt.open_br);
    
	mysql_query(conn,sql);
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"ȡ���ʻ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  
  if(row==NULL){
    dis_str(22,22,"���ʺŲ�����!",0,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  if (ss.acc_flag[0]=='L')
  {
    dis_str(22,22,"��Ӧ�ֻ��ѹ�ʧ!",0,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  if (ss.acc_flag[0]=='K')
  {
    dis_str(22,22,"��Ӧ�ֻ��ѳ���!",0,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  switch (tt.tr_code)
  {
    case 1501:
      note("��������:���¿����ֻ�״̬��Ϊ K");
      if (ss.acc_flag[0]!='A')
      {
        dis_str(22,22,"��Ӧ�ֻ�״̬��!",0,0);
        mysql_rollback(conn);
        getch();
        continue;
      }

      sprintf(sql,"delete FROM s_mast WHERE card_acc='%s' AND open_br=%d",tt.card_acc,tt.open_br);
    
	    mysql_query(conn,sql);
	    if(mysql_errno(conn))
	    {
	      sprintf(sys_err_str,"�޸��ʻ���!%s",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
    	}
    	
      note("���¿�����Ӧ��ϸ״̬��Ϊ K");
      if(update_s_detail()!=0) 
      {
        sprintf(sys_err_str,"�޸��ʻ���!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      if (update_vou(tt.open_br, "K0", 'o')<0)
      {
        sprintf(sys_err_str,"�޸Ŀ���ƾ֤��!%s",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      break;
    case 1502:
      note("��������:���������ֻ�״̬��Ϊ A");
      if (ss.acc_flag[0]!='C')
      {
        dis_str(22,22,"��Ӧ�ֻ�״̬��!",0,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      
      sprintf(sql,"update s_mast set acc_flag='A' where card_acc='%s' and open_br=%d",tt.card_acc,tt.open_br);
      if (mysql_query(conn,sql))
      {
        sprintf(sys_err_str,"�޸��ʻ���!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      break;
    case 1503:
      note("�������");
      if (ss.acc_flag[0]!='A')
      {
        dis_str(22,22,"��Ӧ�ֻ�״̬��!",0,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      rjulmdy(ss.last_day,mday);
      ss.balance=ss.balance-tt.amount;
      ss.interest=ss.interest-s_int(mday,tt.amount); 
      
      sprintf(sql,"update s_mast set balance=%ld,interest=%ld where card_acc='%s' and open_br=%d",
        ss.balance,ss.interest,tt.card_acc,tt.open_br);
      if (mysql_query(conn,sql))
      {
        sprintf(sys_err_str,"�޸��ʻ���!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      
      note("����Ӧ��ϸ״̬��Ϊ K");
      if(update_s_detail()!=0) 
      {
        sprintf(sys_err_str,"�޸��ʻ���!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      break;
    case 1504:
      note("֧ȡ����");
      if (ss.acc_flag[0]!='A')
      {
        dis_str(22,22,"��Ӧ�ֻ�״̬��!",0,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      rjulmdy(ss.last_day,mday);
      ss.balance=ss.balance+tt.amount;
      ss.interest=ss.interest+s_int(mday,tt.amount); 
      
      sprintf(sql,"update s_mast set balance=%ld,interest=%ld where card_acc='%s' and open_br=%d",
        ss.balance,ss.interest,tt.card_acc,tt.open_br);
      if (mysql_query(conn,sql))
      {
        sprintf(sys_err_str,"�޸��ʻ���!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      
      note("����Ӧ��ϸ״̬��Ϊ K");
      if(update_s_detail()!=0) 
      {
        sprintf(sys_err_str,"�޸��ʻ���!");
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      break;
    default:
      dis_str(22,22,"�����ײ��ܳ���!",0,0);
        mysql_rollback(conn);
      getch();
      continue;
  }
  mysql_commit(conn);
  mvprintw(22,22,"���ڶ�����ָʾ�����󻮿�д����!");
  refresh();
 
  sprintf(str2,"%19s'%09ld'%05d",ss.card_acc, ss.balance, ss.det_num);
  write_msr(str2,1);
 
  mvprintw(22,22,"�����ԭƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_ct();

  break;
}
  return(0);
}
