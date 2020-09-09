/****************************************************************
������:zh_pro_vou.ec
������:zh_pro_vou()
��  ��:����ƾ֤
��  ��:1908
��  ��:  
      1.����
        ƾ֤���� (vou_type)
        ��������(sum_que) 
        ƾ֤����(vou_flag) 
        ����Ա��(open_op)
        ҵ������ (tr_br �س�ȡ��¼����)
      2.���ƾ֤�Ƿ����
      3.д��־
      4.дƾ֤�ļ�
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
    mvaddstr(5,30,"��ƾ֤������");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    init_t(&tt);

    sprintf(tt.abstract,"%d",da.vou_flag+4);
    note("���ƾ֤�Ƿ����!");
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
         dis_str(22,22,"����ƾ֤���ʹ�!",0,0); 
         break;
    }
    
	  sprintf(sql,"select flag from vouch where branch=%d and vou_type='%s' and kind=90",da.open_br,vou_type);
	  mysql_query(conn,sql);
	  if(mysql_errno(conn))
	  {
      sprintf(sys_err_str,"ȡƾ֤��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  if(row==NULL)
	  {
      sprintf(sys_err_str,"�޴�ƾ֤!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  strcpy(vou_flag,row[0]);
    mysql_free_result(res);

    if (vou_flag[0]=='K')
    {
      dis_str(22,22,"��ƾ֤������",60,0);
      getch();
      continue;
    }
    note("Ϊ��־��ֵ!");
    tt.branch=da.tr_br;
    tt.operator=sys_operator;
    tt.tr_op=da.tr_op;
    tt.tr_code=1908;
    strcpy(tt.type,"M");
    tt.open_br=da.open_br;
    tt.account=da.account;
    strcpy(tt.acc_type,da.acc_type);
    tt.open_d=da.open_d;
    note("��������");
    tt.det_num=da.vou_flag;
    note("��������");
    tt.term=da.sum_que;

/***********************************/

    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"������������");
      dis_err(sys_err_str);
      return(-1);
    }
    
    note("ȡ��ˮ��!");
    if ((tt.ser_num = get_ser(da.open_br)) < 0)
    {
      sprintf(sys_err_str,"ȡ��־�Ŵ�!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
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
    note("�޸�ƾ֤!");
    if (da.vou_flag==1)
    {
      note("���");
	    sprintf(sql,"update vouch set rest_que=rest_que+%d,takeback=takeback+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (da.vou_flag==2)
    {
      note("ʹ��");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d,use_que=use_que+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (da.vou_flag==3)
    {
      note("����");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d,cancel=cancel+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (da.vou_flag==4)
    {
      note("�Ͻ�");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d,takeback=takeback+%d where branch=%d and kind=90 and vou_type='%s'",
	      da.sum_que,da.sum_que,da.open_br,vou_type);
    }
    else if (da.vou_flag==5)
    {
      note("����");
	    sprintf(sql,"update vouch set rest_que=rest_que-%d,chg_book=chg_book+%d where branch=%d and kind=90 and vou_type='%s'",
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
