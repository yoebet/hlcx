/****************************************************************
  ������:del_item.ec 
  ������:del_item() 
  ��  ��:ɾ����Ŀ��
  ��  ��:2403
  ��  ��:
         1.�����Ŀ��(000000�˳�)
         2.�ӿ�Ŀ�ļ�item_file�м�����ޱ���Ŀ
         3.��Ŀ�ļ�item_file���ñ���ĿΪ����
****************************************************************/              
#include<stdio.h>
#include<string.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern MYSQL *conn;
extern char sql[];

del_item()
{

  short br_no;
  short kind;
  static short in[2]={24,8};
  short num=2,flag;
  struct in_data da;
  char   item_name[21],item_flag[2];
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"��ɾ����Ŀ�š�");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  
  
	sprintf(sql,"SELECT item_name,item_flag FROM item_file WHERE item_num='%ld'",da.item);
	mysql_query(conn,sql);
	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL)
	{
    sprintf(sys_err_str,"�޴˿�Ŀ!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
	}
	strcpy(item_name,row[0]);
	strcpy(item_flag,row[1]);
  mysql_free_result(res);

  mvprintw(7+num,30,"��Ŀ����:[%s]",item_name);
  refresh();
  if (item_flag[0]!='A')
  {
    dis_str(22,22,"����Ŀ������!",60,0);
    getch();
    return(-1);
  }

  dis_str(22,22,"�����޸Ŀ�Ŀ�ļ�...",60,0);
  
  sprintf(sql,"update item_file set item_flag='K' where item_num=%ld",da.item);
	
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"�޸Ŀ�Ŀ�ļ���!%s",mysql_error(conn));
    dis_err(sys_err_str);
    return(-1);
  }
 
  clear_l22();
  dis_err("ɾ�����,����һ������!");
  return(0);
}
