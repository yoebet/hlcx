/**************************************************************
������:pr_detail.ec           
������:pr_detail()           
��  ��:��ӡ���ո�����������ϸ�嵥
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
  
pr_detail()
{
  static short in[3]={22,23,8};
  short num=3,flag;
  struct in_data da;
  FILE *fp;
  char tr_d[11],fname[5],file[50],cmd_str[100],yy;
  char sbr[180],br_name[11];
  short page,count,ii,ymd[3];
  long  account,amount,tr_date;
  short det_num,tr_br,tr_op;
  char  det_flag[2];
  char  det_type[3];
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  rjulmdy(sys_today,ymd);

  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"����ӡ��ϸ�嵥��");
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);
    clear_line(22,22,77);
    dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

	/****************************/
    da.open_br=100;
	
	sprintf(sql,"SELECT br_name FROM br_file WHERE  branch= %d ",da.open_br);
  	
    mysql_query(conn,sql);
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
	  strcpy(br_name,row[0]);
    }
    mysql_free_result(res);
  
    if(row==NULL)
    {
      sprintf(sys_err_str,"ȡ[%d]������¼��!%s.",da.open_br,mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
	
	/****************************/
	
    sprintf(file,"./tmp/tmp_det_%d",da.open_br);
    if((fp=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"����������ϸ�ļ�!",60,0);
      getch();
      return(-1);
    }
    strcpy(da.ltype,"2");
    switch(da.ltype[0])
    {
      case '2':
        sprintf(sbr,"select account,det_num,tr_date,amount,flag,tr_br,tr_op from s_detail\
			where open_br = %d and account >= %d and account <= %d",da.open_br,da.account1,da.account2);
        strcpy(fname,"����");
        break;
      default:
        dis_str(22,22,"����ѡ���!",60,0);
        getch();
        return(-1);
        break;
    }
	
	
	/*******************************/
	
	  mysql_query(conn,sbr);
	  
	  if(mysql_errno(conn))
	  {
		sprintf(sys_err_str,"%s.",mysql_error(conn));
		dis_str(22,22,sys_err_str,60,0);
		getch();
		return(-1);
	  }

	  res=mysql_use_result(conn);

    page=1;
    fprintf(fp,"   ��ϸ�嵥  %20s��[%d]ҳ\n\n"," ",page);
    fprintf(fp,"            ��ʼ�ʺ�:%8ld  ��ֹ�ʺ�:%8ld \n            ��ӡ����:%4d��%2d��%2d��\n",da.account1,da.account2,ymd[2],ymd[0],ymd[1]);
    fprintf(fp,"��������������������������������������������������������������\n");
    fprintf(fp,"  �ʺ�    ���    ҵ����         ���   ״̬ ҵ������ ҵ���Ա\n");
    fprintf(fp,"��������������������������������������������������������������\n");
    count=0;
	
	for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
	{
	  if(mysql_errno(conn)!=0)
	    break;
	  account=atol(row[0]);
	  det_num=atoi(row[1]);
	  tr_date=atol(row[2]);
	  amount=atol(row[3]);
	  strcpy(det_flag,row[4]);
	  tr_br=atoi(row[5]);
	  tr_op=atoi(row[6]);
	  
      if (det_flag[0]=='D')
        strcpy(det_type,"���");
      else if (det_flag[0]=='W')
        strcpy(det_type,"ȡ��");
      else if (det_flag[0]=='O')
        strcpy(det_type,"����");
      else if (det_flag[0]=='C')
        strcpy(det_type,"����");
      else if (det_flag[0]=='K')
        strcpy(det_type,"����");
      else if (det_flag[0]=='Q')
        strcpy(det_type,"��˰");
      else if (det_flag[0]=='I')
        strcpy(det_type,"��Ϣ");
      else 
        strcpy(det_type,det_flag);
   
      rfmtdate(tr_date,"yyyy/mm/dd",tr_d);
      fprintf(fp,"%8ld  %2d    %10s  %10.2lf  %4s     %3d      %2d\n",account,det_num,tr_d,amount/100.00,det_type,tr_br,tr_op);
      count++;
      if (count%50==0)
      {
        fprintf(fp,"��������������������������������������������������������������\n");
        page++;
        for(ii=1;ii<=3;ii++)
          fprintf(fp,"\n");
    fprintf(fp,"   ��ϸ�嵥  %20s��[%d]ҳ\n\n"," ",page);
    fprintf(fp,"            ��ʼ�ʺ�:%8ld  ��ֹ�ʺ�:%8ld \n            ��ӡ����:%4d��%2d��%2d��\n",da.account1,da.account2,ymd[2],ymd[0],ymd[1]);
        fprintf(fp,"��������������������������������������������������������������\n");
        fprintf(fp,"  �ʺ�    ���    ҵ����         ���   ״̬ ҵ������ ҵ���Ա\n");
        fprintf(fp,"��������������������������������������������������������������\n");
      }
    }
	mysql_free_result(res);

    fprintf(fp,"��������������������������������������������������������������\n");
    fprintf(fp," �ۼƱ���:[%d]             ��ӡ����:%4d��%2d��%2d��\n",count,ymd[2],ymd[0],ymd[1]);
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
