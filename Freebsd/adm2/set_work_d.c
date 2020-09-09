/****************************************************************
�� �� ��: set_work_d.ec
�� �� ��: set_work_d()
��    ��: ���ù�����
****************************************************************/
#include<stdio.h>
#include<curses.h>
#include<string.h>
#include "../include/apt.h"
#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern long sys_today;
extern short br_id[50];
extern MYSQL *conn;
extern char sql[];

short flag;

set_work_d()
{
  short branch;
  short i,br_sum,ymd[3];
  char confirm[4];

  struct f_branch br;
  static short in[2]={12,8};
  short num=2,flag;
  struct in_data da;
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  da.k_date=sys_today;
  clear_scr(5,22,22,77);
  mvaddstr(5,30,"�����ù����ա�");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  da.tr_br=100;
  br_sum=1; 
  br_id[0]=da.tr_br;

  for (i=0;i<br_sum;i++)
  {
    branch=br_id[i];

    note("ȡ�����ļ���ָ������¼");
	  
    sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",branch);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"ȡ����¼��!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      dis_str(22,22,"����������!",0,0);  
      getch();
      return(-1);
		}
    set_f_branch(&br,row);
    mysql_free_result(res);
    
    if(br.br_flag[0]!='A') 
    {
      dis_str(22,22,"����������!",0,0);
      getch();
      return(-1);
    }
    
    note("���������ڲ����ڵ�ǰ�������򲻴���");
    if(br.work_d==da.k_date) 
    {
      dis_err("����������!");
      return(-1);
    }
    rjulmdy(br.work_d,ymd);
    if (da.k_date-1!=br.work_d )
    {
      sprintf(sys_err_str,"ԭ������Ϊ%d��%d��%d��!",ymd[2],ymd[0],ymd[1]);
      dis_str(15,20,sys_err_str,0,0); 
      dis_str(17,20,"�Ƿ�ǿ������(yes/no)?[   ]",0,0); 
      confirm[0]='\0';
      while(strncmp(confirm,"YES",3)!=0&&strncmp(confirm,"yes",3)!=0&&
        strncmp(confirm,"NO",2)!=0&&strncmp(confirm,"no",2)!=0)
      {
        dis_str(22,22,"������(yes��no)!",0,0); 
        flag=acc_str(17,49,confirm,3);
        if(flag==ESC)
          return(-1);
      }
      if(strncmp(confirm,"YES",3)!=0&&strncmp(confirm,"yes",3)!=0)
        return(-1);
    }
    
    /***********************************/
    
    note("����������Ϊstart_d(������)");
    note("��������ʼ����־����");

    note("��������ˮ���!");
    
    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"������������");
      dis_err(sys_err_str);
      return(-1);
    }
  
    sprintf(sql,"delete from trade where branch=%d",branch);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"��ձ�����־��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      return(-1);
    }
    
    sprintf(sql,"update br_file set work_d=%ld,init_flag=0 where branch=%d",da.k_date,branch);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"�޸ı����ļ���!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      return(-1);
    }
    
    mysql_commit(conn);
  }
  dis_err("������ϣ�");
  return(0);
}

