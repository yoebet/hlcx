/**************************************************************
 程序名:pr_daily.ec 
 函数名:pr_daily() 
 功  能:生成出各所流水帐   
**************************************************************/              
#include<stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;

extern short br_id[50];
short branch_id,b_count;
short ymd[3];
char br_name[11];
char code_str[13];
int divint;
double ddtax,damount,ddbal,ddint,ddval;
long ltoday;
short i;

short op1,op2;
struct s_trade tt;
struct f_branch  br;
char yy;
struct in_data da;

extern MYSQL *conn;
extern char sql[];

pr_daily()
{

  static short in[2]={31,8};
  short num=2,flag;
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《打印流水帐》");  
  refresh();

    dis_str(22,22,"正在处理,请稍侯......",0,0);  

    rtoday(&ltoday);
    rjulmdy(ltoday,ymd);
	
    mysql_ping(conn);

    da.tr_br=100;
    branch_id=da.tr_br;
    if (branch_id==0)
      return(0);
      op1=1;
      op2=98;
    b_count=1; 
    br_id[0]=branch_id;
	  
    for (i=0;i<b_count;i++)
    {
      branch_id=br_id[i];
      refresh();
	  
	    sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",branch_id);
  	
      mysql_query(conn,sql);
	
      res=mysql_store_result(conn);

      if((row=mysql_fetch_row(res))!=NULL){
	      set_f_branch(&br,row);
      }
      mysql_free_result(res);
  
      if(row==NULL) 
      {
        dis_str(22,22,"本所不存在!",0,0);  
        getch();
        continue;
      }
      if(mysql_errno(conn))
      {
        sprintf(sys_err_str,"取所记录错!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        getch();
        continue;
      }
      if(br.br_flag[0]!='A') 
      {
        dis_str(22,22,"本所已作废!",0,0);
        getch();
        continue;
      }
      strcpy(br_name,br.br_name);
      pr_da();
    }
    dis_err("处理完毕! ");  
  return(0);
}
/********************************************************
函数名:pr_da()
作  用:按所打印交易流水   
返  回:0━>成功  1━>出错 
********************************************************/
pr_da()
{
  FILE *fp;
  char chabs[9],lin[150],branch_name[11];
  char file[55],cmd_str[100];
  char flag;
  char acctype[9],type[5];
  short j,count=0;
  short function,ii,page=1;
  struct total_s
  {
    char     type[10];     /*科目名称*/
    long     open_num;     /*开 户 数*/
    long     close_num;    /*销 户 数*/
    long     out_num;      /*借方笔数*/
    long     out_amt;      /*借方金额*/
    long     in_num;       /*贷方笔数*/
    long     in_amt;       /*贷方金额*/
  } t_d[6];

  sprintf(t_d[1].type,"%s","  定期  ");
  sprintf(t_d[2].type,"%s","  活期  ");
  for (j=0; j<=2; j++)
  {
    t_d[j].open_num= 0;
    t_d[j].in_num= 0;
    t_d[j].in_amt= 0;
    t_d[j].close_num=0;
    t_d[j].out_num= 0;
    t_d[j].out_amt= 0;
  }
  sprintf(file,"./tmp/tmp_daily_%d",branch_id);
  if((fp=fopen(file,"w"))==NULL)
  {
    dis_str(22,22,"不能生成流水文件!",60,0);
    refresh();
    return(-1);
  }
	 
  
  sprintf(sql,"SELECT * FROM trade\
    WHERE branch = %d AND operator>=%d AND operator<=%d\
    ORDER BY operator,ser_num",branch_id,op1,op2);	
  
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      return(-2);
    }

    MYSQL_RES *res;
    MYSQL_ROW row;
	
    res=mysql_use_result(conn);

  if(mysql_errno(conn)!=0)
  {
    snprintf(sys_err_str,95,"打开游标错!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }
  clear_scr(6,20,22,77);
  mvprintw(10,22,"              当前处理流水内容");
  mvprintw(11,22,"=====================================================");
  mvprintw(13,22,"所号  柜员 交易号 交易码 标志  科目号    金额    利息  ");
  mvprintw(16,22,"=====================================================");
  refresh();
  
  row=mysql_fetch_row(res);
  for(;row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
      break;
	set_s_trade(&tt,row);
	
    mvprintw(14,22,"%3d   %2d   %4d   %4d   %1s   %6ld %7.2lf %6.2lf ", tt.branch,tt.tr_op,tt.ser_num,tt.tr_code,tt.correct,tt.item_num,tt.amount/100.00,tt.interest/100.00);
    refresh();
    note("统计合计项!");
    if (tt.correct[0]!= 'K')
    {
      function= tt.tr_code%100;
      switch(tt.tr_code/100)
      {  
        case 11:
          switch(function)
          {
            case 01:
              t_d[1].open_num++;
              t_d[1].in_num++;
              t_d[1].in_amt += tt.amount;
              break;
            case 02:
              t_d[1].close_num++;
              t_d[1].out_num++;
              t_d[1].out_amt += tt.amount;
              break;
            case 05:
              t_d[1].open_num++;
              t_d[1].in_num++;
              t_d[1].in_amt += tt.amount;
              t_d[1].close_num++;
              t_d[1].out_num++;
              t_d[1].out_amt += tt.amount;
              break;
            default:
              break;
          }
          break;
        case 15: 
          switch(function)
          {
            case 01:
              t_d[2].open_num++;
              t_d[2].in_num++;
              t_d[2].in_amt += tt.amount;
              break;
            case 02:
            case 12:
              t_d[2].close_num++;
              t_d[2].out_num++;
              t_d[2].out_amt += tt.amount;
              break;
            case 03:
              t_d[2].in_num++;
              t_d[2].in_amt += tt.amount;
              break;
            case 04:
              t_d[2].out_num++;
              t_d[2].out_amt += tt.amount;
              break;
            default:
              break;
          }
          break;
        default:
          	break;
      }
    }
	
    note("生成开始表头!");
    if (count==0)
    {
      _CutSpace(br_name);
      sprintf(lin,"            流水帐清单       ",branch_id);
      fprintf(fp,"%s\n",lin);
        sprintf(lin,"          日期:%4d年%2d月%2d日       %20s第%2d页 ",ymd[2],ymd[0],ymd[1]," ",page);
      fprintf(fp,"%s\n",lin);
      fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
      fprintf(fp,"操作员 交易号   操作类型  帐号        卡号      发生额   冲标   储种  存期 开户所      类别   户 名    摘要     余  额     利 息     利息税    本笔科目\n");
      fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    }
	
    note("余额");
    ddbal=((double)(tt.balance))/100.0;
    note("利息");
    ddint=((double)(tt.interest))/100.0;
    note("利息税");
    ddtax=((double)(tt.tax_int))/100.0;
    note("发生额");
    damount=((double)(tt.amount))/100.0;
    note("摘要");
    fnote(tt.abstract[0],chabs);
    facctype(tt.type[0],tt.acc_type[0],acctype);
    note("储种");
    ftype(tt.type[0],type);
    get_code_str(tt.tr_code);
    sprintf(lin," %3d   %4d  %12s %8ld %8ld %10.2lf %4s    %4s     %2d   %3d  %12s %10s %6s %10.2lf %9.2lf %8.2lf  %ld ",tt.operator,tt.ser_num,code_str,tt.account,tt.card_acc,damount,tt.correct,type,tt.term,tt.open_br,acctype,tt.customer,chabs,ddbal,ddint,ddtax,tt.item_num);
    fprintf(fp," %s\n",lin);
    count=count+1;
    if (count==50)
    { 
      count=0;
      page=page+1;
      fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    }
  }
	
  mysql_free_result(res);
	
  if (count!=50)
    fprintf(fp,"━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"); 
  fprintf(fp,"\n\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"); 
  fprintf(fp,"   储种       开户数  销户数  借方发生数  借方发生额  贷方发生数  贷方发生额\n",lin);
  for (ii=1;ii<=2;ii++)
  {
    sprintf(lin,"%8s     %4d    %4d      %4d      %10.2lf    %4d      %10.2lf", t_d[ii].type, t_d[ii].open_num, t_d[ii].close_num, t_d[ii].out_num, t_d[ii].out_amt/100.00, t_d[ii].in_num, t_d[ii].in_amt/100.00);
     fprintf(fp," %s\n",lin);
  }
  fprintf(fp,"\n\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"); 
  fclose(fp);
  dis_str(22,22,"是否打印(y/n):[ ]",60,0);
  flag=yorn(22,44);
  if(flag=='y'||flag=='Y') 
  {
    dis_str(22,22,"请准备好打印机后按任一键开始打印",60,0);
    getch();       
    sprintf(cmd_str,"pr %s > "Printer, file);
    system(cmd_str);
  }
	clear_l22();
  return(0);
}
/******************************************
函数名:ftype(dacctype,type)
参  数:
       char dacctype 储种
       char type[]   储种名称
作  用:将内部储种类型转换成中文类型     
******************************************/
ftype(dacctype,type)
char dacctype,type[];
{
  switch(dacctype)
  {
    case 'D':
      strcpy(type,"定期");
      break;
    case 'H':
      strcpy(type,"活期");
      break;
    default:
      strcpy(type,"    ");
      break;
  }
  return(0);
}

/******************************************
函数名:fnote(subnote,chnote)
参  数:
       char subnote   内部摘要
       char chnote[]  中文摘要           
作  用:将内部摘要转换成中文摘要           
******************************************/
fnote(subnote,chnote)
char subnote,chnote[];
{
  if (subnote=='0'|| subnote==' ')
    strcpy(chnote,"现金");
  else if (subnote=='1')
    strcpy(chnote,"转收");
  else if (subnote=='2')
    strcpy(chnote,"转付");
  else if (subnote=='3')
    strcpy(chnote,"托收");
  else if (subnote=='4')
    strcpy(chnote,"划付");
  else if (subnote=='5')
    strcpy(chnote,"领回");
  else if (subnote=='6')
    strcpy(chnote,"使用");
  else if (subnote=='7')
    strcpy(chnote,"作废");
  else if (subnote=='8')
    strcpy(chnote,"上缴");
  else if (subnote=='9')
    strcpy(chnote,"换折");
  else
    strcpy(chnote,"    ");
  refresh();
  return(0);
}

get_code_str(code)
short code;
{
  switch(code)
  {
    case 1101: strcpy(code_str,"定期开户"); break;
    case 1102: strcpy(code_str,"定期销户");break;
    case 1105: strcpy(code_str,"定期部份提支");break;
    case 1111: strcpy(code_str,"定期挂失补开");break;
    case 1121: strcpy(code_str,"定期解挂解冻");break;
    case 1181: strcpy(code_str,"定期冲正");break;
    case 1501: strcpy(code_str,"活期开户");break;
    case 1502: strcpy(code_str,"活期销户");break;
    case 1503: strcpy(code_str,"活期续存");break;
    case 1504: strcpy(code_str,"活期支取");break;
    case 1506: strcpy(code_str,"活期挂失补开");break;
    case 1507: strcpy(code_str,"活期登折");break;
    case 1500: strcpy(code_str,"活期换折");break;
    case 1511: strcpy(code_str,"活期挂失");break;
    case 1512: strcpy(code_str,"活期特殊销户");break;
    case 1521: strcpy(code_str,"活期解挂解冻");break;
    case 1581: strcpy(code_str,"活期冲正");break;
    case 1978: strcpy(code_str,"查询校验码");break;
    case 1961: strcpy(code_str,"查询业务处理");break;
    case 1976: strcpy(code_str,"查询主机流水");break;
    case 1962: strcpy(code_str,"设置标志业务");break;
    case 1963: strcpy(code_str,"密码初始业务");break;
    case 1964: strcpy(code_str,"操作修改业务");break;
    case 1965: strcpy(code_str,"修改户名地址");break;
    case 1951: strcpy(code_str,"总帐贷处理");break;
    case 1952: strcpy(code_str,"总帐借处理");break;
    case 1981: strcpy(code_str,"总帐调整冲正");break;
    case 1908: strcpy(code_str,"调整凭证");break;
    case 1972: strcpy(code_str,"调整凭证处理");break;
    case 1982: strcpy(code_str,"调整现金库存");break;
    case 1974: strcpy(code_str,"主机轧帐处理");break;
    case 142: strcpy(code_str,"柜员签退业务");break;
    case 144: strcpy(code_str,"增加柜员业务");break;
    case 145: strcpy(code_str,"删除柜员业务");break;
    case 146: strcpy(code_str,"更新柜员业务");break;
    case 147: strcpy(code_str,"柜员修改密码");break;
    default: strcpy(code_str,"未知符");break;
  }
}

/*━━━━━━━━━━━━-
  功    能:转换帐户性质
━━━━━━━━━━━━-*/
facctype(save_flag,typ,acctype) 
char save_flag,typ,acctype[];
{

  strcpy(acctype,"        "); 
  if (typ=='D')
    strcpy(acctype,"定    期"); 
  else if (typ=='H' )
    strcpy(acctype,"个人活折");
  else if (typ=='K')
    strcpy(acctype,"卡      ");
  else
    strcpy(acctype,"        ");
  refresh();
  return(0);
}
