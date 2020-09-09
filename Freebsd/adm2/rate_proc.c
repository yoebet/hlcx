/**************************************************************
* 程序名:int_proc.ec                                          *
* 函数名:int_proc()                                           *
* 功能 : 结息处理                                             *
**************************************************************/              
#include<stdio.h>
#include<string.h>
#include<curses.h>
#include<ctype.h>
#include<time.h>
#include<signal.h>
#include "mysql.h"
#include "../include/sh_table.h"
#include "../include/mainadm.h"
#define ILFN "data/tmp_%1cintl%03ld"
#define ILFN "data/tmp_%1cintl%03ld"

extern char sys_err_str[96];

extern char sys_loc_name[21],sys_op_name[9],sys_op_level[2];
extern short sys_loc_num,sys_operator;    /* system variable */

extern long sys_today;
extern long loc_country;
extern MYSQL *conn;
extern char sql[];

long get_rate(type,term)
char type;
short term;
{    
  long rate;
  MYSQL_RES *res;
  MYSQL_ROW row;
  
	sprintf(sql,"SELECT rate FROM ratefile WHERE term=%d and ltype='%c'",term,type);
	
	if(mysql_query(conn,sql))
	{
	  return(-1);
	}
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row)
	{
		rate=atol(row[0]);
    mysql_free_result(res);
    return(rate);
	}
}


rate_proc() /* int process every year */
{
char confirm[4],type[2],str[96];
short proc_ret;

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《活期结息》");  
  refresh();
rfmtdate(sys_today,"yyyy/mm/dd",str);
mvaddstr(12,25,"日期:");printw("%1s",str);
mvaddstr(13,25,"若结息错,数据将不一致,请先备份数据库!!");
mvaddstr(14,25,"应在记载总帐之后结息,在本日业务结束前打印结息清单!!");
mvaddstr(16,26,"确认(yes/no):");
strcpy(type,"s");
confirm[0]='\0';
while(strncmp(confirm,"YES",3)!=0&&strncmp(confirm,"yes",3)!=0&&
      strncmp(confirm,"NO",2)!=0&&strncmp(confirm,"no",2)!=0)
 {
  acc_str(16,41,confirm,3);
 }
if(strncmp(confirm,"YES",3)!=0&&strncmp(confirm,"yes",3)!=0)
  return(0);

if((type[0]=='s')||(type[0]=='S'))
{
  proc_ret=int_s_proc();
if(proc_ret!=0)
  dis_err("请先改错.然后恢复数据库,再结息!!");
else
  dis_err("处理完毕,按任一键返回主菜单.");
}
return(0);
}

FILE *fp,*fp1;
char int_str[128];
char int_str1[128];
long last_branch;
long s_rate;
long amt_P,amt_s;
long cnt_P,cnt_s;
long amt_t_P,amt_t_s;
long amt_t0,amt_t1,amt_t2;
long amt0,amt1,amt2;
struct ss_mast smb;

MYSQL_RES *res;
MYSQL_ROW row;

#define Iacc_sP 255101
#define Iacc_ss 255102

#define Iint_sP 521001
#define Iint_ss 521001

int_s_proc() /* s type int process */
{
  short branch;
  long account;
  short today_mdy[3];
  rjulmdy(sys_today,today_mdy);

  last_branch=0;
  s_rate=get_rate('H',0);

  sprintf(sql,"SELECT * FROM s_mast ORDER BY open_br,account");
    
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"查询账户出错!%s",mysql_error(conn));
    dis_err(sys_err_str);
    return(1);
  }
  res=mysql_use_result(conn);
  row=mysql_fetch_row(res);
  if(row==NULL)
  {
    dis_err("没有账户！");
    return(1);
  }
  for(;row!=NULL;row=mysql_fetch_row(res))
  {
	  set_ss_mast(&smb,row);
	
    mvprintw(15,25,"正在处理 所号:%3d 帐号:%8ld",smb.open_br,smb.account);
    refresh();
    if(sbeginbr_proc()
    	||acc_sint_proc()
    	||sendbr_proc())
    {
      mysql_free_result(res);
      return(1);
    }
  }
 
  mysql_free_result(res);
  return(0);
}

sbeginbr_proc() /* begin branch process */
{
amt_P=0;
amt_s=0;
amt_t_P=0;
amt_t_s=0;
amt0=0;
amt1=0;
amt2=0;
amt_t0=0;
amt_t1=0;
amt_t2=0;
cnt_P=0;
cnt_s=0;
sprintf(int_str,"./tmp/s_int_%d",smb.open_br);
if((fp=fopen(int_str,"w"))==NULL)
 {
  dis_err("结息文件生成错");
  return(1);
 }
rfmtdate(sys_today,"yyyy/mm/dd",int_str);
fprintf(fp,"\n%25s活期结息清单\n\n","");
fprintf(fp,"%5s所号:%03ld%35s日期:%10s\n","",smb.open_br,"",int_str);
fprintf(fp,
  "%5s-----------------------------------------------------------------------------------------\n","");
fprintf(fp,
  "%5s 帐  号 %2s 姓  名 %6s性质%2s结息前余额%2s  转 息 额%2s结息后余额 %3s结息后积数%2s代扣税款\n",
  "","","","","","","","");

sprintf(int_str1,"./tmp/sum_int_%d",smb.open_br);
if((fp1=fopen(int_str1,"w"))==NULL)
 {
  dis_err("结息汇总文件生成错!");
  return(1);
 }
fprintf(fp1,"\n%25s活期结息汇总清单\n\n","");
fprintf(fp1,"%5s所号:%03ld%35s日期:%10s\n","",smb.open_br,"",int_str);
fprintf(fp1,
  "%5s-----------------------------------------------------------------------------------------\n","");
return(0);
}

sendbr_proc() /* end branch process */
{
      dis_str(22,22,sys_err_str,60,0);
      getch();
fprintf(fp,
  "%5s-----------------------------------------------------------------------------------------\n","");
fprintf(fp,"%10s    存折户  户数:%9ld%10s转息额:%11.2lf%5s\n","",cnt_P,"",amt_P/100.00,"");
fprintf(fp,"%10s    卡户    户数:%9ld%10s转息额:%11.2lf%5s\n","",cnt_s,"",amt_s/100.00,"");
fprintf(fp,"%10s    总  计  户数:%9ld%10s转息额:%11.2lf%5s\n", "",cnt_P+cnt_s,"", (amt_P+amt_s)/100.00,"");
fprintf(fp,"%5s--------------------------------------------------------------------------\n","");
fclose(fp);

fprintf(fp1,"%5s--------------------------------------------------------------------------\n","");
fprintf(fp1,"%10s    存折户  户数:%9ld%10s转息额:%11.2lf%5s\n","",cnt_P,"",amt_P/100.00,"",amt_t_P/100.00);
fprintf(fp1,"%10s    卡户    户数:%9ld%10s转息额:%11.2lf%5s\n","",cnt_s,"",amt_s/100.00,"",amt_t_s/100.00);
fprintf(fp1,"%10s    总  计  户数:%9ld%10s转息额:%11.2lf%5s\n","",cnt_P+cnt_s,"",(amt_P+amt_s)/100.00,"");
fprintf(fp1,"%5s--------------------------------------------------------------------------\n","");
fclose(fp1);

dis_str(22,22,sys_err_str,60,0);
getch();
amt0=amt_P+amt_s;
amt_t0=amt_t_P+amt_t_s;
amt2=amt1+amt0;
return(0);
}

acc_sint_proc() /* account int day process */
{
	long n_interest,n_balance,tax_amount;
	long o_interest,tmp_tax;
	struct  s_tax  ptx;
	short ptoday_mdy[3];
	long ltoday;
	if(smb.acc_flag[0]=='C'||smb.acc_flag[0]=='K'||smb.acc_type[0]=='D')
	  return(0);
	rjulmdy(sys_today,ptoday_mdy);
	tmp_tax=0L;
	o_interest=smb.interest*(s_rate/3600000.00)+0.50001;
	n_balance=smb.balance+o_interest-(o_interest*TAXRATE/100)+tmp_tax;/*扣除税款*/
	tax_amount=o_interest*TAXRATE/100+0.50001;
	n_interest=(n_balance/100)*360;
	o_interest=o_interest+tmp_tax;
	smb.det_num++;
	
	MYSQL *conn2;

	//外部conn查询还没结束，所以再打开一个连接
  conn2=mysql_init(NULL);
  if(!connectdb(conn2)){
    sprintf(sys_err_str,"数据库错误!%s",mysql_error(conn2));
    dis_err(sys_err_str);
    return(1);
  }

  if(mysql_query(conn2,"START TRANSACTION")){
    sprintf(sys_err_str,"不能启动事务");
    dis_err(sys_err_str);
    mysql_close(conn2);
    return(1);
  }
	
	sprintf(sql,"update s_mast set balance=%ld,interest=%ld,det_num= %d \
		where open_br=%d and account=%ld",n_balance,n_interest,smb.det_num,smb.open_br,smb.account);
	
  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"数据库错误!%s",mysql_error(conn2));
    dis_err(sys_err_str);
    mysql_rollback(conn2);
    mysql_close(conn2);
    return(1);
  }

	sprintf(sql,"insert into s_detail values(%d,%ld,%s,%d,%ld,%ld,'1','D',%d,%d)",
		smb.open_br,smb.account,smb.card_acc,smb.det_num,sys_today,o_interest,smb.open_br,sys_operator);
	
  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"数据库错误!%s",mysql_error(conn2));
    dis_err(sys_err_str);
    mysql_rollback(conn2);
    mysql_close(conn2);
    return(1);
  }

	smb.det_num++;

	sprintf(sql,"insert into s_detail values(%d,%ld,%s,%d,%ld,%ld,'1','C',%d,%d)",
		smb.open_br,smb.account,smb.card_acc,smb.det_num,sys_today,tax_amount,smb.open_br,sys_operator);
	
  if(mysql_query(conn2,sql))
  {
    /*税摘要为't'*/
    sprintf(sys_err_str,"数据库错误!%s",mysql_error(conn2));
    dis_err(sys_err_str);
    mysql_rollback(conn2);
    mysql_close(conn2);
    return(1);
  }
 
/*插入所得税表*/
	ptx.branch=smb.open_br;
	ptx.account=smb.account;
	strcpy(ptx.customer,smb.customer);
	ptx.open_d=smb.acc_open_d;
	if (smb.acc_open_d< 10000)
		ptx.open_d=smb.card_open_d;
	ptx.start_d=ptx.open_d;
	ptx.end_d=sys_today;
	ptx.acc_type[0]=smb.acc_type[0];
	ptx.acc_flag[0]=smb.acc_flag[0];
	ptx.transfer=0L;
	ptx.close_op=0;
	ptx.item_num=smb.item_num;
	ptx.withdraw[0]='\0';
	ptx.rate=0;
	ptx.balance=0L;
	ptx.interest=o_interest-tax_amount;
	ptx.taxamt=tax_amount;
	ptx.ser_num=0;
	ptx.back1=0L;
	ptx.back2[0]='\0';
	ptx.back3=0L;
	ptx.back4[0]='\0';
	
	set_s_tax_i_sql(&ptx,sql);

  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"数据库错误!%s",mysql_error(conn2));
    dis_err(sys_err_str);
    mysql_rollback(conn2);
    mysql_close(conn2);
    return(1);
  }
	 
fprintf(fp,"%2s%8ld%5s%8s%2s%4s%2s%10.2lf%2s%10.2lf%2s%10.2lf%4s%10.2lf%5s%10.2lf\n","",smb.account,"",smb.customer,"",smb.acc_type,"",smb.balance/100.00,"",
  o_interest/100.00,"",n_balance/100.00,"",n_interest/100.00,"",tax_amount/100.00);
switch(smb.acc_type[0])
 {case 'P':amt_P+=o_interest;
           amt_t_P+=tax_amount;
           cnt_P++;
           break;
  case 'K':amt_s+=o_interest;
           cnt_s++;
           amt_t_s+=tax_amount;
           break;
  default:dis_err("账户类型错");
          mysql_rollback(conn2);
          mysql_close(conn2);
          return(1);
 }
 
mysql_commit(conn2);
mysql_close(conn2);

return(0);
}

//显示错误信息
dis_err1(char *str)
{
  WINDOW *hp;
  int i=0;
  int j,l,l1,w,bx;
  
  l=strlen(str);
  w=2;
  if (l%2 !=0 ) 
  {
    l=l+1;
    w=3;
  }
  //bx=(80-l-4)/2;
  bx=(88-l-4)/2;
  
  mvprintw(21,22,"%55s"," ");
  mvprintw(22,22,"%50s"," ");
  refresh();
  //hp=newwin(3,l+4,18,bx);
  hp=newwin(3,l+4,20,bx);
  wstandout(hp);
  mvwaddstr(hp,0,0,"┌");
  l1=l/2;
  for (j=1;j<=l1;j++)
    mvwaddstr(hp,0,2*j,"─");
  mvwaddstr(hp,0,2*j,"┐");
  mvwaddstr(hp,1,0,"│");
  if (w==3) mvwaddstr(hp,1,2," ");
  mvwaddstr(hp,1,w,str);
  mvwaddstr(hp,1,2*j,"│");
  mvwaddstr(hp,2,0,"└");
  for (j=1;j<=l1;j++)
    mvwaddstr(hp,2,2*j,"─");
  mvwaddstr(hp,2,2*j,"┘");
  wrefresh(hp);
  wstandend(hp);
  wgetch(hp);//改为按任一键结束
  //wclear(hp);
  //wrefresh(hp);
  delwin(hp);
  touchwin(stdscr);
  wrefresh(stdscr);
}
