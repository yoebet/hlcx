/**************************************************************
������:rate_print.ec
������:rate_print()
��  ��:���ʱ��ӡ 
**************************************************************/
#include<stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;
extern MYSQL *conn;
extern char sql[];

rate_print()
{
  FILE *fp;
  struct f_rate rat;
  long item_s,item_e;
  short ii,pp,line;
  char yy,file[50],f_name[5],cmd_str[50],str[11];
  char type[2];
  short term1,term2;

  static short in[2]={50,8};
  short num=2,flag;
  struct in_data da;
  MYSQL_RES *res;
  MYSQL_ROW row;

    init_in(&da);
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"����ӡ���ʡ�");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);
    dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  
    switch(da.rate_type[0])
    {
      case '2':
        strcpy(type,"D");
        strcpy(f_name,"����");
        break;
      case '1':
        strcpy(type,"H");
        strcpy(f_name,"����");
        break;
      default:
        dis_str(22,22,"��������ѡ���!",0,0);
        getch();
        return(-1);
        break;
    }
    da.term=99;
    if (da.term==99)
    {
      term1=0;
      term2=99;
    }
    else
    {
      term1=da.term;
      term2=da.term;
    }
	
	sprintf(sql,"SELECT * FROM ratefile WHERE ltype='%s' AND term>=%d AND term <=%d ORDER BY ltype,term,int_date",type,term1,term2);
	  
	mysql_query(conn,sql);
	  
	if(mysql_errno(conn))
	{
		sprintf(sys_err_str,"%s.",mysql_error(conn));
		dis_str(22,22,sys_err_str,60,0);
		getch();
		return(-1);
	}
	res=mysql_store_result(conn);
  
    strcpy(file,"./tmp/tmp_rate");
    if((fp=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"�������������ļ�!",60,0);
      getch();
      return(-1);
    }
    fprintf(fp,"����:[%s]  ��  ��  ��   ��1ҳ\n",f_name);
    fprintf(fp,"������������������������������\n");
    fprintf(fp,"����      ��Ϣ��       ������\n");
    pp=1;
    line=0;
	
	for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
	{
	  if(mysql_errno(conn)!=0)
	    break;
	  set_f_rate(&rat,row);
	
      rfmtdate(rat.int_date,"yyyy/mm/dd",str);
      fprintf(fp,"%2d     %10s     %7.4lf\n",rat.term,str,rat.rate/10000.00);
      line++;
      if (line%50==0)
      {
        fprintf(fp,"������������������������������\n");
        for (ii=1;ii<=2;ii++)
          fprintf(fp,"\n");
        pp++;
        fprintf(fp,"����:[%s]  ��  ��  ��   ��%dҳ\n",f_name,pp);
        fprintf(fp,"������������������������������\n");
        fprintf(fp,"����      ��Ϣ��       ������\n");
      }
    }
	mysql_free_result(res);
  
    fprintf(fp,"������������������������������\n");
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
	clear_l22();
    dis_err("�������! ");  
  return(0);
}
