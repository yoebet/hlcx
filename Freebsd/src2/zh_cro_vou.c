/****************************************************************
������:zh_cro_vou.ec
������:zh_cro_vou()
��  ��:����ƾ֤����
��  ��:1971
��  ��:  
      1.����
        ƾ֤���� (vou_type)
        ��������(sum_que) 
        ���׺� (ser_num) 
      2.ȡָ�����׺ŵ���־
      3.�����־״̬������¼�����
      4.�޸���־Ϊ����
      5.д���������
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

zh_cro_vou()
{
  struct f_vouch sum;
  struct s_trade tt;
  struct in_data da;
  char yy;
  static short in[4]={5,38,11,8};
  short num=4,flag;
  char vou_flag[2],vou_type[3];
  short br,op;
  long day;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  strcpy(da.acc_type,"M");
  da.tr_br=sys_branch;
  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,22,"��ƾ֤����������");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    init_t(&tt);

    note("���ָ�����׺��Ƿ����!");
    br=da.open_br;
    op=sys_operator;
    
    
	  sprintf(sql,"select * from trade where branch=%d and operator=%d and ser_num=%d",
	    da.open_br,op,da.ser_num);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"ȡ�����״�!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  if(row==NULL)
	  {
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
    if (tt.tr_code!=1908)
    {
      dis_str(22,22,"����־�в����벻��!",0,0);
      getch();
      continue;
    }
    if (strcmp(tt.abstract,da.vou_type)!=0)
    {
      dis_str(22,22,"����־��ƾ֤���Ͳ���!",0,0);
      getch();
      continue;
    }
    if (tt.term!=da.sum_que)
    {
      dis_str(22,22,"����־��ҵ���������!",0,0);
      getch();
      continue;
    }

/***********************************/

    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"������������");
      dis_err(sys_err_str);
      return(-1);
    }

    note("�޸�ԭ��־Ϊ����!");
	  sprintf(sql,"update trade set correct='K' where branch=%d and operator=%d and ser_num=%d",
	    da.open_br,op,da.ser_num);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"�޸ı����״�!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      continue;
	  }

    note("�޸�ƾ֤!");
    switch (da.vou_type[0])
    {
       case '2':
         strcpy(vou_type,"H0");
         break;
       case '3':
         strcpy(vou_type,"K0");
         break;
       default:
         dis_str(22,22,"����ƾ֤���ʹ�!",0,0); 
         break;
    }
    if (tt.det_num==1)
    {
      note("���");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.open_br,vou_type);
    }
    else if (tt.det_num==2)
    {
      note("ʹ��");
	    sprintf(sql,"update vouch set rest_que=rest_que+%d,use_que=use_que-%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (tt.det_num==3)
    {
      note("����");
	    sprintf(sql,"update vouch set rest_que=rest_que+%d,cancel=cancel-%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (tt.det_num==4)
    {
      note("�Ͻ�");
	    sprintf(sql,"update vouch set rest_que=rest_que+%d,takeback=takeback-%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (tt.det_num==5)
    {
      note("����");
	    sprintf(sql,"update vouch set rest_que=rest_que+%d,chg_book=chg_book-%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    if (mysql_query(conn,sql)||(mysql_errno(conn)))
    {
      sprintf(sys_err_str,"�޸�ƾ֤�ļ���!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      continue;
    }
    mysql_commit(conn);

    mvprintw(1,68,"��ˮ��:%d",tt.ser_num);
    refresh();
    dis_err("�������! ");  
      break;
  }
  return(0);
}

