/****************************************************************
程序名:check_sum.ec
函数名:check_sum()
功  能:检查总帐与分户帐一致性
流  程:  
      2.从指定文本data/def_check_sum取所有需核对的科目
        科目号  对应储种
      3.从科目文件中取科目名称并检查科目合法性
      4.根据科目号对应储种从指定储种累计本科目的户数和金额
      5.从总帐90中取本科目的余户,余额
      6.将4,5两项的结果4,5两项进行比较
说  明:需在记总帐后,初始化前执行本功能.
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern char sys_op_name[9];
extern short sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

short count=0;
FILE *fp,*fpe;
check_sum()
{
  char item_flag[2];
  char yy,cmd_str[50],file[50];
  long item_num;
  long item_num_1,item[100];
  short ymd[3],i,nn,debug=0,type,item_type[100];
  short branch,balind,homeind;
  double yes_bal;
  long bal;
  short yes_home,home_sum;

  static short in[2]={27,8};
  short num=2,flag;

  MYSQL_RES *res;
  MYSQL_ROW row;

  rjulmdy(sys_today,ymd);

    clear_scr(5,22,22,77);
    mvaddstr(5,30,"《总帐与分户核对》");
    refresh();
    dis_str(22,22,"正在处理,请稍侯......",0,0);  

    branch=100;

    if (debug==1)
    {
      mvprintw(21,22,"正在从data/def_check_sum中读科目号及相关信息!");
      refresh();
      getch();
    }
    fp=fopen("./data/def_check_sum","r");
    if(fp==NULL) 
    {
      dis_err("不能打开文件/usr/hlcx/data/def_check_sum");
      return(0);
    }
    nn=0;
    while(fscanf(fp,"%1d %06ld\n",&type,&item_num_1)==2)
    {
      nn++;
      item[nn]=item_num_1;
      item_type[nn]=type;
    }
    fclose(fp);
    note("生成出错文件!");
    sprintf(file,"./tmp/check_sum_err");
    if((fpe=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"不能生成出错文件!",60,0);
      getch();
      return(-1);
    }
    fprintf(fpe,"         总分核对出错清单  \n");
    fprintf(fpe,"━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    fprintf(fpe,"科目号  错误原因         错误码   分户内容    总帐内容\n");

    note("依次处理每个科目号及相关信息!");
    for(i=1;i<=nn;i++)
    {
      item_num=item[i];
      type=item_type[i];
      sprintf(sys_err_str,"正在处理科目:[%ld].",item_num);
      dis_str(22,22,sys_err_str,60,0);

      note("从item_file中读科目号并检查相关信息!");
      
	    sprintf(sql,"select item_flag from item_file where item_num=%ld",item_num);
      if(mysql_query(conn,sql))
      {
        write_e(item_num,"取科目记录",mysql_errno(conn),0,0,0);
        continue;
      }
	    res=mysql_store_result(conn);
	    row=mysql_fetch_row(res);
      if(row==NULL)
      {
        write_e(item_num,"取科目记录",-1,0,0,0);
        continue;
      }
      strcpy(item_flag,row[0]);
      mysql_free_result(res);
      if (item_flag[0]!='A')
      {
        write_e(item_num,"科目已作废",0,0,0,0);
        continue;
      }

      note("根据科目号对应储种从指定储种累计本科目的户数和金额");
      if (type==1)
      {
	      sprintf(sql,"select sum(balance),count(*) from s_mast\
	       where open_br=%d and item_num=%ld and acc_flag!='K' and acc_flag!='C'",branch,item_num);
      }
      else if (type==2)
      {
	      sprintf(sql,"select sum(balance),count(*) from f_mast\
	       where open_br=%d and item_num=%ld and acc_flag!='K' and acc_flag!='C'",branch,item_num);
      }
      else
      {
        write_e(item_num,"储种定义错",0,0,0,0);
        continue;
      }
      if(mysql_query(conn,sql))
      {
        write_e(item_num,"取总帐累计数错",mysql_errno(conn),0,0,0);
        continue;
      }
      res=mysql_store_result(conn);
      row=mysql_fetch_row(res);
      if(row!=NULL)
      {
	    	bal=atol(row[0]);
	    	home_sum=atoi(row[1]);
	      mysql_free_result(res);
      }
      else
      {
        bal = 0;
        home_sum = 0;
      }
      
      note("从总帐90中取本科目的余户,余额!");
	    sprintf(sql,"select balance,rest_que from sumtable where kind = 90\
	     and branch=%d and item_num=%ld",branch,item_num);
      if(mysql_query(conn,sql))
      {
        write_e(item_num,"取总帐累计数错",mysql_errno(conn),0,0,0);
        continue;
      }
      res=mysql_store_result(conn);
      row=mysql_fetch_row(res);
      if(row==NULL)
      {
        write_e(item_num,"取总帐累计数错",-1,0,0,0);
        continue;
      }
    	yes_bal=atof(row[0]);
    	yes_home=atoi(row[1]);
      mysql_free_result(res);
      
      if (yes_bal!=bal)
      {
        write_e(item_num,"余额不符",0,bal,yes_bal,1);
      }
      if (yes_home!=home_sum)
      {
        write_e(item_num,"余户不符",0,home_sum,yes_home,0);
      }
    }
    fprintf(fpe,"━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    fprintf(fpe,"   有[%d]笔出错记录!",count);
    fprintf(fpe,"   打印日期:%4d年%2d月%2d日\n",ymd[2],ymd[0],ymd[1]);
    fclose(fpe);
    clear_l22();
    dis_str(22,22,"是否打印(y/n):[ ]",60,0);
    flag=yorn(22,44);
    if(flag=='y'||flag=='Y') 
    {
      dis_str(22,22,"请准备好打印机后按任一键开始打印",60,0);
      getch();
      sprintf(cmd_str,"pr %s > "Printer, file);
      system(cmd_str);
    }
  return(0);
}
/*************************************************
函数名:write_e(item_num,err_str,err_code,mast_bal,sum_bal,flag)
功  能:写检查中出现的错误信息
参  数:  
       item_num 科目号
       err_str  错误原因
       err_code 错误码 
       mast_bal 分户内容
       sum_bal  总帐内容
       flag     标志 0_整数处理  1_小数处理
*************************************************/
write_e(item_num,err_str,err_code,mast_bal,sum_bal,flag)
long item_num,mast_bal;
double sum_bal;
short err_code;
char err_str[16];
short flag;
{
  if (flag==1)
    fprintf(fpe,"%6ld  %-15s  %6ld %10.2lf  %10.2lf\n",item_num,err_str,err_code,mast_bal/100.00,sum_bal/100.00); 
  else
    fprintf(fpe,"%6ld  %-15s  %6ld     %6ld      %6ld\n",item_num,err_str,err_code,mast_bal,sum_bal); 
   count++;
}
