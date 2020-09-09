/******************************************************************
* 程序名:pr_tab_d.ec 
* 函数名:pr_tab_d(br,flag)
* 功  能:打印指定所的日报/月报/年报
* 参  数:br   所号 999_全部所 A99_A办事处 000_退出 ABC_指定所
         flag 标志  0上年报 1-12某月报 90日报 99本年报
*******************************************************************/
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

pr_tab_d(br,flag)
short br,flag;
{

  /**日报表格式数组,与需打印表格顺序一致**/
  struct tab
  {
    short bz;
    long km;
    char mc[19];
    long clos;
    double jf_amt;
    long open;
    double df_amt;
    long rest;
    double jf_bal;
    double df_bal;
    long jf_cnt;
    long df_cnt;
  }dd[100];

  /**报表格式字符串,与需打印表格一致,为处理零输出**/
  char mc[19],s_clos[5],s_jf_amt[11],s_open[5],s_df_amt[11];
  char s_rest[7],s_jf_bal[13],s_df_bal[13],s_df_cnt[5],s_jf_cnt[5];
  
  /**总帐表格式数组,与sumtabl表格顺序一致,用于读取数据**/
  /*
  struct sum
  {
    short br;
    short kind;
    long km;
    char flag[2];
    dec_t det_amt;
    dec_t cre_amt;
    long open;
    long clos;
    long rec;
    long pay;
    dec_t bal;
    long  rest;
  } ss;
  */
  struct f_sumtable ss;

  FILE *fp_i,*fp_o;
  short branch,kind;
  long item;
  char fname[50],item_name[19],s_item[7],s_bz[3];
  double bal;
  short kk,i,j,bz,ymd[3];
  char buffer[24],*sp;
  long ltoday;
  char bt0[5];
  short br_id[130],s_br,e_br;
  char br_name[130][11];
  short i_br,k,br_num,item_num,ii,jj;
  char cmd_str[100];

  MYSQL_RES *res;
  MYSQL_ROW row;

  if((fp_i=fopen("./data/def_tab_d","r"))==NULL)
  {	
    dis_err("日报定义文件未找到!"); 
    return(1);
  }

  i=1;
  while (fgets(buffer,74,fp_i))
  {
    if (feof(fp_i))
    {
      fclose (fp_i);
      break;
    }
    sp=buffer;
    j=0;
    memcpy(s_bz,sp+j,2); 
    s_bz[2]='\0';
    dd[i].bz=atoi(s_bz);
    j+=2;
    memcpy(s_item,sp+j,6); 
    s_item[6]='\0';
    j+=6;
    memcpy(item_name,sp+j,17); 
    item_name[17]='\0';
    item=atol(s_item);
    dd[i].km=item;
    strcpy(dd[i].mc,item_name);
    if (item==222222)
      break;
    i=i+1;
  }
  item_num=i;
  fclose(fp_i);

  rtoday(&ltoday);
  rjulmdy(ltoday,ymd);

  kind=flag;
  if (flag==90)
    strcpy(bt0,"日报");
  else if (flag==99)
    strcpy(bt0,"年报");
  else if (flag==0)
    strcpy(bt0,"上年");
  else
    strcpy(bt0,"月报");

  note("正在读取需处理的所有所!");
  s_br=br;
  e_br=br; 
  i_br=br/100;
  if (br==999)
  {
    s_br=101;
    e_br=999; 
  }
  /**
  if (br==i_br*100)
  {
    s_br=i_br*100+1;
    e_br=i_br*100+99; 
  }
  **/
  
  /**********************************/
  
  sprintf(sql,"SELECT branch,br_name FROM br_file WHERE branch>=%d AND branch<=%d ORDER BY branch",s_br,e_br);
  	
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"查询出错!%s.",mysql_error(conn));
    dis_err(sys_err_str);
    return(-1);
  }

  res=mysql_store_result(conn);

  br_num=0;
  for(k=1;;k++)
  {
    if((row=mysql_fetch_row(res))==NULL||mysql_errno(conn)!=0)
      break;
	br_id[k]=atoi(row[0]);
	strcpy(br_name[k],row[1]);
  }
  mysql_free_result(res);
	
	/**********************************/

  sprintf(sql,"SELECT * FROM sumtable WHERE branch=? AND kind=? AND item_num=? ");
  
  MYSQL_STMT *stmt;
  MYSQL_BIND param_bind[3];
  MYSQL_BIND result_bind[12];
  
  stmt = mysql_stmt_init(conn);
  if (!stmt||mysql_stmt_prepare(stmt, sql, strlen(sql)))
  {
	sprintf(sys_err_str,"初始化预处理语句错!");
	dis_err(sys_err_str);
	return(-1);
  }
  memset(param_bind, 0, sizeof(param_bind));
  
  param_bind[0].buffer_type= MYSQL_TYPE_SHORT;
  param_bind[0].buffer= &branch;

  param_bind[1].buffer_type= MYSQL_TYPE_SHORT;
  param_bind[1].buffer= &kind;

  param_bind[2].buffer_type= MYSQL_TYPE_LONG;
  param_bind[2].buffer= &item;
  
  memset(result_bind, 0, sizeof(result_bind));
  
  result_bind[0].buffer_type= MYSQL_TYPE_SHORT;
  result_bind[0].buffer= &ss.branch;
  result_bind[1].buffer_type= MYSQL_TYPE_SHORT;
  result_bind[1].buffer= &ss.kind;
  result_bind[2].buffer_type= MYSQL_TYPE_LONG;
  result_bind[2].buffer= &ss.item_num;
  result_bind[3].buffer_type= MYSQL_TYPE_STRING;
  result_bind[3].buffer= &ss.flag;
  result_bind[3].buffer_length=2;
  result_bind[4].buffer_type= MYSQL_TYPE_DOUBLE;
  result_bind[4].buffer= &ss.det_amt;
  result_bind[5].buffer_type= MYSQL_TYPE_DOUBLE;
  result_bind[5].buffer= &ss.cre_amt;
  result_bind[6].buffer_type= MYSQL_TYPE_LONG;
  result_bind[6].buffer= &ss.open_que;
  result_bind[7].buffer_type= MYSQL_TYPE_LONG;
  result_bind[7].buffer= &ss.cls_que;
  result_bind[8].buffer_type= MYSQL_TYPE_LONG;
  result_bind[8].buffer= &ss.dvou_que;
  result_bind[9].buffer_type= MYSQL_TYPE_LONG;
  result_bind[9].buffer= &ss.cvou_que;
  result_bind[10].buffer_type= MYSQL_TYPE_DOUBLE;
  result_bind[10].buffer= &ss.balance;
  result_bind[11].buffer_type= MYSQL_TYPE_LONG;
  result_bind[11].buffer= &ss.rest_que;
  

	/**********************************/
	
  if (k==1)
    br_num=1;
  else
    br_num=k-1;
	
  for(ii=1;ii<=br_num;ii++)
  {
    branch=br_id[ii];

    note("初始化报表格式数组!");
    for (i=0;i<=item_num;i++)
    {  
      dd[i].clos=0;
      dd[i].jf_amt=0.0;
      dd[i].open=0;
      dd[i].df_amt=0.0;
      dd[i].rest=0;
      dd[i].jf_bal=0.0;
      dd[i].df_bal=0.0;
      dd[i].jf_cnt=0;
      dd[i].df_cnt=0;
    }
    for(jj=1;jj<item_num;jj++)
    {
      item=dd[jj].km;
      strcpy(item_name,dd[jj].mc);
      if (item==111111 || item==222222 || item==888888)
        continue;
      sprintf(sys_err_str,"正在读总帐  科目:[%6ld]!",item);
      note(sys_err_str);
	  
	  /****************************/
	  
	  
	  if (mysql_stmt_bind_param(stmt, param_bind)||
			mysql_stmt_execute(stmt)||
			mysql_stmt_bind_result(stmt, result_bind)||
			mysql_stmt_store_result(stmt)||
			mysql_stmt_fetch(stmt))
	  {
        sprintf(sys_err_str,"读总帐错!科目号[%ld],标志[%d]!",item,kind);
        dis_err(sys_err_str);
        continue;
	  }
	  mysql_stmt_free_result(stmt);
	  
	  /****************************/
	  
      dd[jj].clos=ss.cls_que;
      bal=ss.det_amt;
      //dectodbl(&ss.det_amt,&bal);
      dd[jj].df_amt=bal;
      dd[jj].open=ss.open_que;
      bal=ss.cre_amt;
      //dectodbl(&ss.cre_amt,&bal);
      dd[jj].jf_amt=bal;
      dd[jj].rest=ss.rest_que;
      bal=ss.balance;
      //dectodbl(&ss.balance,&bal);
      if (bal>0)
        dd[jj].df_bal=bal;
      else
        dd[jj].jf_bal=-bal;
      dd[jj].jf_cnt=ss.cvou_que;
      dd[jj].df_cnt=ss.dvou_que;
    }
	
    note("正在生成报表!");
    sprintf(fname,"./tmp/tmp_tab_d_%d",branch);
    if((fp_o=fopen(fname,"w"))==NULL)
    {	
      dis_err("日报文件不能生成!");
      return(1);
    }
    fprintf(fp_o,"%40s营业%s表\n", " ",bt0);
    fprintf(fp_o,"%20s  制表日期:%4d年%2d月%2d日      \n"," ",ymd[2],ymd[0],ymd[1]);
    fprintf(fp_o,"┏━━━━━━━━━┳━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━┳━━━━━┓\n");
  if (flag==90)
    fprintf(fp_o,"┃                  ┃          今日发生额              ┃             今日余额             ┃  业务量  ┃\n");
  else if (flag==99)
    fprintf(fp_o,"┃                  ┃          本年发生额              ┃             本年余额             ┃  业务量  ┃\n");
  else if (flag==0)
    strcpy(bt0,"上年");
  else
    fprintf(fp_o,"┃                  ┃          本月发生额              ┃             本月余额             ┃  业务量  ┃\n");
    fprintf(fp_o,"┃  业  务  种  类  ┣━━┳━━━━━┳━━┳━━━━━┳━━━┳━━━━━━┳━━━━━━┳━━┳━━┫\n");
    fprintf(fp_o,"┃                  ┃销户┃  借  方  ┃开户┃  贷  方  ┃ 户数 ┃   借  方   ┃   贷  方   ┃借方┃贷方┃\n");
    fprintf(fp_o,"┣━━━━━━━━━╋━━╋━━━━━╋━━╋━━━━━╋━━━╋━━━━━━╋━━━━━━╋━━╋━━┫\n");

    for(i=1;i<=item_num;i++)
    {
      if (dd[i].km==111111)
      {
        fprintf(fp_o,"┣━━━━━━━━━╋━━╋━━━━━╋━━╋━━━━━╋━━━╋━━━━━━╋━━━━━━╋━━╋━━┫\n");
        continue;
      }
      if (dd[i].km==222222)
      {
        fprintf(fp_o,"┗━━━━━━━━━┻━━┻━━━━━┻━━┻━━━━━┻━━━┻━━━━━━┻━━━━━━┻━━┻━━┛\n");
        fprintf(fp_o,"                                       制表:                     复核:\n");
        break;
      }
      if (dd[i].km==888888)
        kk=0;
      else
        kk=i;
  
      strcpy(mc,dd[i].mc);

      if (dd[kk].clos==0)
        strcpy(s_clos,"    ");
      else
        sprintf(s_clos,"%4d",dd[kk].clos);

      if (dd[kk].jf_amt==0.0)
        strcpy(s_jf_amt,"          ");
      else
        sprintf(s_jf_amt,"%10.2lf",dd[kk].jf_amt/100.00);

      if (dd[kk].open==0)
        strcpy(s_open,"    ");
      else
        sprintf(s_open,"%4d",dd[kk].open);

      if (dd[kk].df_amt==0.0)
        strcpy(s_df_amt,"          ");
      else
        sprintf(s_df_amt,"%10.2lf",dd[kk].df_amt/100.00);

      if (dd[kk].rest==0)
        strcpy(s_rest,"      ");
      else
        sprintf(s_rest,"%6d",dd[kk].rest);

      if (dd[kk].jf_bal==0.0)
        strcpy(s_jf_bal,"            ");
      else
        sprintf(s_jf_bal,"%12.2lf",dd[kk].jf_bal/100.00);

      if (dd[kk].df_bal==0.0)
        strcpy(s_df_bal,"            ");
      else
        sprintf(s_df_bal,"%12.2lf",dd[kk].df_bal/100.00);

      if (dd[kk].df_cnt==0)
        strcpy(s_df_cnt,"    ");
      else
        sprintf(s_df_cnt,"%4d",dd[kk].df_cnt);

      if (dd[kk].jf_cnt==0)
        strcpy(s_jf_cnt,"    ");
      else
        sprintf(s_jf_cnt,"%4d",dd[kk].jf_cnt);

      fprintf(fp_o,"┃%18s┃%4s┃%10s┃%4s┃%10s┃%6s┃%12s┃%12s┃%4s┃%4s┃\n",mc,s_clos,s_jf_amt,s_open,s_df_amt,s_rest,s_jf_bal,s_df_bal,s_jf_cnt,s_df_cnt);
      if (dd[i].bz==1)
      {
        dd[0].clos  +=dd[i].clos;
        dd[0].jf_amt+=dd[i].jf_amt;
        dd[0].open  +=dd[i].open;
        dd[0].df_amt+=dd[i].df_amt;
        dd[0].rest  +=dd[i].rest;
        dd[0].jf_bal+=dd[i].jf_bal;
        dd[0].df_bal+=dd[i].df_bal;
        dd[0].jf_cnt+=dd[i].jf_cnt;
        dd[0].df_cnt+=dd[i].df_cnt;
      }
    }
    fclose(fp_o);
    clear_scr(6,20,22,77);
    dis_str(22,22,"是否打印报表(y/n):[ ]",0,0);
    flag=yorn(22,48);
    if(flag=='y'||flag=='Y') 
    {
      dis_err("请准备好打印机后按任一键开始打印");
      sprintf(cmd_str,"pr %s> "Printer,fname);
      system(cmd_str);
    }
  }
  mysql_stmt_close(stmt);
  
  clear_l22();
  return(0);
}
