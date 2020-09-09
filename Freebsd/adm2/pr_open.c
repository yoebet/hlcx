/**************************************************************
������:pr_open.ec           
������:pr_open()           
��  ��:��ӡ���ո��������ֿ����嵥
**************************************************************/     
#include <stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern short sys_operator;    
extern long sys_today;
extern MYSQL *conn;
extern char sql[];
  
MYSQL_RES *res;
MYSQL_ROW row;

short item_count,vou_count;

struct s_trade tt;
FILE *fp;
short branch_id;
extern short br_id[50];

pr_open()
{
  char file[50],cmd_str[100],yy;
  char br_name[11];
  short ii,ymd[3];

  static short in[2]={27,8};
  short num=2,flag;
  struct in_data da;

  init_in(&da);

  rjulmdy(sys_today,ymd);

  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"����ӡ�����嵥��");
    refresh(); 
    da.tr_br=100;
    branch_id=da.tr_br;
	  
		sprintf(sql,"SELECT br_name FROM br_file WHERE  branch= %d ",branch_id);
		
		mysql_query(conn,sql);
		
		res=mysql_store_result(conn);

		if((row=mysql_fetch_row(res))!=NULL){
		  strcpy(br_name,row[0]);
		}
		mysql_free_result(res);
  
		if(row==NULL)
		{
		  sprintf(sys_err_str,"ȡ����¼��!%s.",mysql_error(conn));
		  dis_str(22,22,sys_err_str,60,0);
		  getch();
		  return(-1);
		}
	  
    sprintf(file,"./tmp/tmp_open_%d",branch_id);
    if((fp=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"�������ɿ����ļ�!",60,0);
      getch();
      return(-1);
    }
    fprintf(fp,"   �����嵥");
    fprintf(fp,"%20s ��ӡ����:%4d��%2d��%2d��\n"," ",ymd[2],ymd[0],ymd[1]);
    fprintf(fp,"����������������������������������������������������������������������\n\n\n");
    //re_open("D");
    re_open("H");
    fclose(fp);
    dis_str(22,22,"�Ƿ��ӡ(y/n):[ ]",60,0);
    flag=yorn(22,44);
    if(flag=='y'||flag=='Y') 
    {
      dis_str(22,22,"��׼���ô�ӡ������һ����ʼ��ӡ",60,0);
      getch();
      sprintf(cmd_str,"pr %s > "Printer, file);
      system(cmd_str);
    }
    break;
  }
  return(0);
}
/****************************************************
������:re_open(s_type)
��  ��:��ָ���ֻ������ҳ����������ļ�¼,�Ҵ�ӡ����   
****************************************************/
re_open(s_type)
char s_type[2];
{
  char acc_flag[9],st_d[11],en_d[11];
  long scount;
  char fname[10];
  short ymd[3];
  short code1,code2,code3;
  double sum_bal;

  long ltoday;

  scount=0L; /*  ����   */
  sum_bal=0.0;
  switch(s_type[0])
  {
    case 'H':
      strcpy(fname,"����");
      code1=1501;
      code2=1506;
      code3=1506;
      break;
    case 'D':
      strcpy(fname,"˫��");
      code1=1101;
      code2=1105;
      code3=1106;
      break;
    default:
      break;
  }
  fprintf(fp," ����:[%s]\n",fname);
  fprintf(fp,"����������������������������������������������������������������\n");
  fprintf(fp,"      �ʺ�      ����         ���  ��Ŀ��       ������   ״̬\n");
  fprintf(fp,"����������������������������������������������������������������\n");
  note("��trade��ȡ��ˮ!");
  
	/*******************************/
	
	sprintf(sql,"SELECT * FROM trade\
		WHERE branch=%d AND correct!='K' AND ltype='%s' AND (tr_code=%d OR tr_code=%d OR tr_code=%d)\
		ORDER BY operator,ser_num",branch_id,s_type,code1,code2,code3);
	mysql_query(conn,sql);
	  
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"%s.",mysql_error(conn));
	  dis_str(22,22,sys_err_str,60,0);
	  getch();
	  return(-1);
	}

	res=mysql_use_result(conn);
  
  clear_scr(6,22,22,77);
  mvprintw(10,22,"              ��ǰ������ˮ����");
  mvprintw(11,22,"==================================================");
  mvprintw(13,22,"���� ��Ա ���׺� ������ ��־ ��Ŀ��  ���   ��Ϣ  ");
  mvprintw(16,22,"==================================================");
  refresh();
  
  for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
	{
	  if(mysql_errno(conn)!=0)
	    break;
	  set_s_trade(&tt,row);
	
    note("��������!");
    if(tt.tr_code%100!=1 && tt.tr_code%100!=5 && tt.tr_code%100!=6)
      continue;
    mvprintw(14,22,"%3d   %2d   %4d   %4d   %1s   %6ld %7.2lf %6.2lf ", tt.branch,tt.tr_op,tt.ser_num,tt.tr_code,tt.correct,tt.item_num,tt.amount/100.00,tt.interest/100.00);
    refresh();
    if (tt.tr_code%100==05)
    {
      strcpy(acc_flag,"�����¿�");
      tt.account=tt.transfer;
      tt.amount=tt.balance-tt.amount;
    }
    else if (tt.tr_code%100==06)
    {
      strcpy(acc_flag,"��ʧ����");
      tt.account=tt.transfer;
    }
    else
      strcpy(acc_flag,"��    ��");
    scount++;
    sum_bal+=tt.amount;
    rjulmdy(tt.open_d,ymd);
    sprintf(st_d,"%04d/%02d/%02d",ymd[2],ymd[0],ymd[1]);
    fprintf(fp,"  %08ld  %12s   %10.2lf  %06ld   %10s %6s\n",tt.account,tt.customer,tt.amount/100.00,tt.item_num,st_d,acc_flag);
  }
  mysql_free_result(res);
	
  fprintf(fp,"����������������������������������������������������������������\n");
  fprintf(fp,"  �����ֹ���ӡ%6ld��     ���:%10.2lfԪ\n",scount,sum_bal/100.00);
  fprintf(fp,"\n\n");
  return(0);
}
