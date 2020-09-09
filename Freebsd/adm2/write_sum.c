/**************************************************************
程序名:write_sum.ec
函数名:write_sum()
功  能:根据日志记指定所号的总帐
流  程:  
      2.从科目文件中取所有科目存入数组sum
      3.从日志中取本所的所有记录
      4.根据交易码统计各科目的有关数据
      5.按科目取昨日余户,余额,与本日进行加收减付
      6.根据汇总科目定义文件统计各汇总科目的数据
      7.将数组结果写入各科目的90标志记录中,
        等本所执本日结束时再转入上日/本月/本年等记录中.
      8.修改所文件中的记总帐标志
****************************************************************/

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern char sys_op_name[9];
extern short sys_operator;    
extern long sys_today,sys_work_d;
extern MYSQL *conn;
extern char sql[];

MYSQL *conn2;
short item_count,vou_count;

struct to_sum sum[100];

void _clscon_s(){
	if(conn2!=NULL)mysql_close(conn2);
}

write_sum()
{
  struct s_trade tt;
  struct f_item  it;
  struct f_branch  br;
  char yy;
  
  long ii0,ii9,itt,li,ii,nn,ii_s,ii101,ii164;
  long iiint;
  long iiint0;
  short code,co12,co34;
  short abst;
  short debug=0;
  short inttax;
   
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_RES *res2;
  MYSQL_ROW row2;

  FILE *fp;
  long saccno,acc1[20];
  short s1,s2;
  long itemno;
  short branch;
  
  double yes_bal;
  short yes_home;

  static short in[2]={27,8};
  short num=2,flag;
  struct in_data da;

  init_in(&da);

    clear_scr(5,22,22,77);
    mvaddstr(5,22,"《记总帐》");
    refresh();
    dis_str(22,22,"正在处理,请稍侯......",0,0);  

    if (debug==1)
    {
      mvprintw(21,22,"正在初始化日志表!");
      refresh();
      getch();
    }

    if (debug==1)
    {
      mvprintw(21,22,"正在从item_file中读科目号及相关信息!");
      refresh();
      getch();
    }
    da.tr_br=100;
    branch=da.tr_br;

    note("取所号文件中指定所记录");
    
    
    sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",branch);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"取所记录错!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      dis_str(22,22,"本所不存在!",0,0);  
      getch();
      return(-1);
		}
    set_f_branch(&br,row);
    mysql_free_result(res);
    
    if(br.br_flag[0]!='A') 
    {
      dis_str(22,22,"本所已作废!",0,0);
      getch();
      return(-1);
    }

    note("从item_file中读科目号及相关信息!");
    nn=0;
    
	  sprintf(sql,"select * from item_file where item_flag!='K'");
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"取科目错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"没有科目!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
    
    if(conn2==NULL){
    	conn2=mysql_init(NULL);
      atexit(_clscon_s);
	    if(!connectdb(conn2)){
	      sprintf(sys_err_str,"%s",mysql_error(conn2));
	      dis_str(22,22,sys_err_str,60,0);
	      getch();
	      return(-1);
	    }
    }
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_f_item(&it,row);
      nn++;
      sum[nn].item_num=it.item_num;
      strcpy(sum[nn].item_name,it.item_name);
      sum[nn].sum_flag=it.sum_flag;
      sum[nn].prt_ser=it.prt_ser;
      sum[nn].j_x_num=0;
      sum[nn].j_x_amt=0;
      sum[nn].j_z_num=0;
      sum[nn].j_z_amt=0;
      sum[nn].d_x_num=0;
      sum[nn].d_x_amt=0;
      sum[nn].d_z_num=0;
      sum[nn].d_z_amt=0;
      sum[nn].open_num=0;
      sum[nn].close_num=0;
      note("取本所本科目的昨日余户和余额!"); 
      
	    sprintf(sql,"select balance,rest_que from sumtable where kind = 91\
	       and branch=%d and item_num=%ld",branch,it.item_num);
	    if(mysql_query(conn2,sql))
	    {
        yes_bal=0;
        yes_home=0;
      }else{
	      res2=mysql_store_result(conn2);
	      row2=mysql_fetch_row(res2);
	      if(row2!=NULL){
      	  yes_bal=atof(row2[0]);
      	  yes_home=atoi(row2[1]);
          mysql_free_result(res2);
	      }
        yes_bal=0;
        yes_home=0;
      }
      sum[nn].yes_bal=yes_bal;
      sum[nn].today_bal=0;
      sum[nn].yes_home=yes_home;
      sum[nn].today_home=0;
    }
    mysql_free_result(res);
    item_count=nn;
   
    if (debug==1)
    {
      mvprintw(21,22,"正在得到现金科目101000的数组下标!");
      refresh();
      getch();
    }
    note("得到现金科目101000的数组下标!");
    ii101=get_i_item_ws(101000);
    if (ii101<0)
    {
      sprintf(sys_err_str,"无现金科目[101000]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    inttax=get_i_item_ws(532001);
    if (inttax<0)
    {
      sprintf(sys_err_str,"无扣税科目[532001]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
    note("得到辖内往来科目164000的数组下标!");
    ii164=get_i_item_ws(164000);
    if (ii164<0)
    {
      sprintf(sys_err_str,"无现金科目[164000]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    if (debug==1)
    {
      mvprintw(21,22,"正在从trade中取流水!");
      refresh();
      getch();
    }
    note("从trade中取流水!");
    
	  sprintf(sql,"select * from trade where branch = %d and correct!='K'",branch);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"取流水错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"没有流水!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
    
    clear_scr(6,22,22,77);
    mvprintw(10,22,"              当前处理流水内容");
    mvprintw(11,22,"==================================================");
    mvprintw(13,22,"所号 柜员 交易号 交易码 标志 科目号  金额   利息  ");
    mvprintw(16,22,"==================================================");
    refresh();
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_s_trade(&tt,row);
      code=tt.tr_code;
      co12=tt.tr_code/100;
      co34=tt.tr_code%100;
      abst=atoi(tt.abstract);
      mvprintw(14,22,"%3d   %2d   %4d   %4d   %1s   %6ld %7.2lf %6.2lf ", tt.branch,tt.tr_op,tt.ser_num,tt.tr_code,tt.correct,tt.item_num,tt.amount/100.00,tt.interest/100.00);
      refresh();
      note("操作码后两位大于10,为非正常业务,不需处理!");
      if (co34>10)
      {
        note("非正常业务!");
        continue;
      }
      if (co12<=15)
      {
        note("正常业务则得到本业务科目数组下标!");
        ii=get_i_item_ws(tt.item_num);
        if (ii<0)
        {
          sprintf(sys_err_str,"无科目[%ld]对应的数组下标!",tt.item_num);
          dis_str(22,22,sys_err_str,60,0);
          getch();
          continue;
        }
      }
      switch (co34)
      {
        case 1:
          note("开户业务!");
          if (abst==0)
          {
            note("记原科目现金业务!");
            note("贷方现金笔数");
            sum[ii].d_x_num++;
            note("贷方现金金额");
            sum[ii].d_x_amt+=tt.amount;
            note("开户数");
            sum[ii].open_num++;

            note("记现金科目业务!");
            note("借方现金笔数");
            sum[ii101].j_x_num++;
            note("借方现金金额");
            sum[ii101].j_x_amt+=tt.amount;
          }
          else
          {
            note("记原科目转帐业务!");
            note("贷方转帐笔数");
            sum[ii].d_z_num++;
            note("贷方转帐金额");
            sum[ii].d_z_amt+=tt.amount;
            note("开户数");
            sum[ii].open_num++;

            note("记辖内往来科目转帐业务!");
            note("借方转帐笔数");
            sum[ii164].j_z_num++;
            note("借方转帐金额");
            sum[ii164].j_z_amt+=tt.amount;
          }
          break;
        case 2:
          note("销户业务!");

          note("找销户科目的对转利息科目的下标!");
          iiint=found(tt.type,tt.term,tt.item_num);
          if (iiint<0)
          {
            sprintf(sys_err_str,"无科目[%ld]对应的利息科目!",tt.item_num);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          note("若为定期提前支取取活期利息!");
          if (tt.interest==tt.s_int)
          {
            iiint=get_i_item_ws(521100);
            if (iiint<0)
            {
              sprintf(sys_err_str,"无利息科目[521100]对应的数组下标!");
              dis_str(22,22,sys_err_str,60,0);
              getch();
              continue;
            }
          }
          else
          {
            iiint0=get_i_item_ws(iiint);
            if (iiint<0)
            {
              sprintf(sys_err_str,"无利息科目[%d]对应的数组下标!",iiint);
              dis_str(22,22,sys_err_str,60,0);
              getch();
              continue;
            }
            iiint=iiint0;
          }
          if (abst==0)
          {
            note("记原科目现金业务!");
            note("借方现金笔数");
            sum[ii].j_x_num++;
            note("借方现金金额");
            sum[ii].j_x_amt+=tt.amount;
            note("销户数");
            sum[ii].close_num++;

            note("记本科目对应利息科目!");
            note("借方利息笔数");
            sum[iiint].j_x_num++;
            note("借方利息");
            sum[iiint].j_x_amt+=tt.interest;
            sum[inttax].j_x_num++;
            sum[inttax].j_x_amt+=tt.tax_int ;

            note("记现金科目业务!");
            note("贷方现金笔数");
            sum[ii101].d_x_num++;
            note("贷方现金金额");
            sum[ii101].d_x_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          else
          {
            note("记原科目转帐业务!");
            note("借方转帐笔数");
            sum[ii].j_z_num++;
            note("借方转帐金额");
            sum[ii].j_z_amt+=tt.amount;
            note("销户数");
            sum[ii].close_num++;

            note("记本科目对应利息科目!");
            note("借方转帐笔数");
            sum[iiint].j_z_num++;
            note("借方转帐金额");
            sum[iiint].j_z_amt+=tt.interest;
            sum[inttax].j_z_num++;
            sum[inttax].j_z_amt+=tt.tax_int ;

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum[ii164].d_z_num++;
            note("贷方现金金额");
            sum[ii164].d_z_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          break;
        case 3:
          note("续存业务!");
          if (abst==0)
          {
            note("记原科目现金业务!");
            note("贷方现金笔数");
            sum[ii].d_x_num++;
            note("贷方现金金额");
            sum[ii].d_x_amt+=tt.amount;

            note("记现金科目业务!");
            note("借方现金笔数");
            sum[ii101].j_x_num++;
            note("借方现金金额");
            sum[ii101].j_x_amt+=tt.amount;
          }
          else
          {
            note("记原科目转帐业务!");
            note("贷方转帐笔数");
            sum[ii].d_z_num++;
            note("贷方转帐金额");
            sum[ii].d_z_amt+=tt.amount;

            note("记辖内往来科目转帐业务!");
            note("借方转帐笔数");
            sum[ii164].j_z_num++;
            note("借方转帐金额");
            sum[ii164].j_z_amt+=tt.amount;
          }
          break;
        case 4:
          note("支取业务!");
          if (abst==0)
          {
            note("记原科目现金业务!");
            note("借方现金笔数");
            sum[ii].j_x_num++;
            note("借方现金金额");
            sum[ii].j_x_amt+=tt.amount;

            note("记现金科目业务!");
            note("贷方现金笔数");
            sum[ii101].d_x_num++;
            note("贷方现金金额");
            sum[ii101].d_x_amt+=tt.amount;
          }
          else
          {
            note("记原科目转帐业务!");
            note("借方转帐笔数");
            sum[ii].j_z_num++;
            note("借方转帐金额");
            sum[ii].j_z_amt+=tt.amount;

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum[ii164].d_z_num++;
            note("贷方现金金额");
            sum[ii164].d_z_amt+=tt.amount;
          }
          break;
        case 5:
          note("部提业务!");
          note("按原户销户后再开新户处理!");

          note("找销户科目的对转活期利息科目的下标!");
          ii_s=found("S",tt.term,tt.item_num);
          if (ii_s<0)
          {
            sprintf(sys_err_str,"无活期对应的利息科目!");
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          if (abst==0)
          {
            note("记原科目现金业务!");

            note("原户销户!");

            note("借方现金笔数");
            sum[ii].j_x_num++;
            note("借方现金金额");
            sum[ii].j_x_amt+=tt.balance;
            note("销户数");
            sum[ii].close_num++;

            note("记本科目对应利息科目!");
            note("借方现金笔数");
            sum[ii_s].j_x_num++;
            note("借方现金金额");
            sum[ii_s].j_x_amt+=tt.interest;
            sum[inttax].j_x_num++;
            sum[inttax].j_x_amt+=tt.tax_int ;

            note("记现金科目业务!");
            note("贷方现金笔数");
            sum[ii101].d_x_num++;
            note("贷方现金金额");
            sum[ii101].d_x_amt+=tt.balance+tt.interest+tt.tax_int;

            note("新户开户!");
            note("贷方现金笔数");
            sum[ii].d_x_num++;
            note("贷方现金金额");
            sum[ii].d_x_amt+=tt.balance-tt.amount;
            note("开户数");
            sum[ii].open_num++;

            note("记现金科目业务!");
            note("借方现金笔数");
            sum[ii101].j_x_num++;
            note("借方现金金额");
            sum[ii101].j_x_amt+=tt.balance-tt.amount;

          }
          else
          {
            note("记原科目转帐业务!");
            note("原户销户!");

            note("借方现金笔数");
            sum[ii].j_z_num++;
            note("借方现金金额");
            sum[ii].j_z_amt+=tt.balance;
            note("销户数");
            sum[ii].close_num++;

            note("记本科目对应利息科目!");
            note("贷方现金笔数");
            sum[ii_s].d_z_num++;
            note("贷方现金金额");
            sum[ii_s].d_z_amt+=tt.interest;
            sum[inttax].j_z_num++;
            sum[inttax].j_z_amt+=tt.tax_int ;

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum[ii164].d_z_num++;
            note("贷方现金金额");
            sum[ii164].d_z_amt+=tt.balance+tt.interest+tt.tax_int;

            note("新户开户!");
            note("贷方现金笔数");
            sum[ii].d_z_num++;
            note("贷方现金金额");
            sum[ii].d_z_amt+=tt.balance-tt.amount;
            note("开户数");
            sum[ii].open_num++;

            note("记辖内往来科目业务!");
            note("借方现金笔数");
            sum[ii164].j_z_num++;
            note("借方现金金额");
            sum[ii164].j_z_amt+=tt.balance-tt.amount;
          }
          break;
        case 9:
          note("总帐调整!");
          note("根据调整的总帐科目金额笔数开/销户数及");
          note("调整内容修改对应科目的内容!");

          note("得到本调整科目的数组下标!");
          ii=get_i_item_ws(tt.item_num);
          if (ii<0)
          {
            sprintf(sys_err_str,"无科目[%ld]对应的数组下标!",tt.item_num);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          if (tt.det_num==1)
          {
            note("借方调整!");
            if (tt.abstract[0]=='0')
            {
              note("现金!");
              note("借方现金笔数");
              sum[ii].j_x_num+=tt.term;
              note("借方现金金额");
              sum[ii].j_x_amt+=tt.amount;
            }
            else
            {
              note("转帐!");
              note("借方转帐笔数");
              sum[ii].j_z_num+=tt.term;
              note("借方转帐金额");
              sum[ii].j_z_amt+=tt.amount;
            }
            note("销户数");
            sum[ii].close_num+=tt.transfer;
          }
          else
          {
            note("贷方调整!");
            if (tt.abstract[0]=='0')
            {
              note("现金!");
              note("贷方现金笔数");
              sum[ii].d_x_num+=tt.term;
              note("贷方现金金额");
              sum[ii].d_x_amt+=tt.amount;
            }
            else
            {
              note("转帐!");
              note("贷方转帐笔数");
              sum[ii].d_z_num+=tt.term;
              note("贷方转帐金额");
              sum[ii].d_z_amt+=tt.amount;
            }
            note("开户数");
            sum[ii].open_num+=tt.transfer;
          }
          break;
        default:
          break;
      }
      note("当前交易处理完毕!");
    }
    mysql_free_result(res);

    if (debug==1)
    {
      mvprintw(22,22,"正在统计各科目的本日余户和余额!");
      refresh();
      getch();
    }
    
    note("统计各科目的本日余户和余额!");
    for (ii=1;ii<=item_count;ii++)
    {
      sum[ii].today_bal=sum[ii].yes_bal
                        +sum[ii].d_x_amt+sum[ii].d_z_amt
                        -sum[ii].j_x_amt-sum[ii].j_z_amt;
      sum[ii].today_home=sum[ii].yes_home
                        +sum[ii].open_num-sum[ii].close_num;
      if (debug==1)
      {
        mvprintw(18,22,"item_num=%d,item_name=[%s],balance=%10.2lf,home=%d",sum[ii].item_num,sum[ii].item_name,sum[ii].today_bal,sum[ii].today_home);
        mvprintw(19,22,"d_x_amt=%ld,d_z_amt=%ld,j_x_amt=%ld,j_z_amt=%ld", sum[ii].d_x_amt,sum[ii].d_z_amt,sum[ii].j_x_amt,sum[ii].j_z_amt);
        mvprintw(20,22,"yes_home=%d,open_num=%d,close_num=%d", sum[ii].yes_home,sum[ii].open_num,sum[ii].close_num);
        refresh();
        getch();
      }
    }

    if (debug==1)
    {
      mvprintw(21,22,"正在根据汇总文件统计各汇总科目的数据!");
      refresh();
      getch();
    }
    note("根据汇总科目定义文件统计各汇总科目的数据!");
    fp=fopen("./data/def_sum","r");
    if(fp==NULL) 
    {
      dis_str(22,22,"不能打开汇总定义文件data/def_sum",60,0);
      getch();
      return(-1);
    }
    s1=0;
    while(fscanf(fp,"%ld",&saccno)==1)
    {
      if(saccno != 0L)
      {
        note("依次读一行内各累加科目!");
        acc1[s1]=saccno;
        s1++;
        continue;
      }
      else
      {
        note("已读完一行内容!");
        if(s1<2)
        {
          s1=0;
          dis_str(22,22,"汇总定义文件格式错!无累加科目!",60,0);
          getch();
          continue;
        }
        note("汇总科目 itemno!");
        itemno=acc1[0];
        note("得到汇总科目的数组下标!");
        ii0=get_i_item_ws(itemno);
        if (ii0<0)
        {
          sprintf(sys_err_str,"无科目[%ld]对应的数组下标!",itemno);
          dis_str(22,22,sys_err_str,60,0);
          getch();
          continue;
        }
          sum[ii0].j_x_num   =0;
          sum[ii0].j_x_amt   =0;
          sum[ii0].j_z_num   =0;
          sum[ii0].j_z_amt   =0;
          sum[ii0].d_x_num   =0;
          sum[ii0].d_x_amt   =0;
          sum[ii0].d_z_num   =0;
          sum[ii0].d_z_amt   =0;
          sum[ii0].open_num  =0;
          sum[ii0].close_num =0;
          sum[ii0].yes_bal   =0;
          sum[ii0].today_bal =0;
          sum[ii0].yes_home  =0;
          sum[ii0].today_home=0;
        note("将各累加科目加入汇总科目的数组!");
        for(s2=1;s2<s1;s2++)
        {
          itemno=acc1[s2];
          ii9=get_i_item_ws(itemno);
          if (ii9<0)
          {
            sprintf(sys_err_str,"无科目[%ld]对应的数组下标!",itemno);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          sum[ii0].j_x_num   +=sum[ii9].j_x_num;
          sum[ii0].j_x_amt   +=sum[ii9].j_x_amt;
          sum[ii0].j_z_num   +=sum[ii9].j_z_num;
          sum[ii0].j_z_amt   +=sum[ii9].j_z_amt;
          sum[ii0].d_x_num   +=sum[ii9].d_x_num;
          sum[ii0].d_x_amt   +=sum[ii9].d_x_amt;
          sum[ii0].d_z_num   +=sum[ii9].d_z_num;
          sum[ii0].d_z_amt   +=sum[ii9].d_z_amt;
          sum[ii0].open_num  +=sum[ii9].open_num;
          sum[ii0].close_num +=sum[ii9].close_num;
          sum[ii0].yes_bal   +=sum[ii9].yes_bal;
          sum[ii0].today_bal +=sum[ii9].today_bal;
          sum[ii0].yes_home  +=sum[ii9].yes_home;
          sum[ii0].today_home+=sum[ii9].today_home;
        }
        s1=s2=0;
      }
    }
    fclose(fp);

    
    /***********************************/
    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"不能启动事务");
      dis_err(sys_err_str);
      return(-1);
    }
    if (debug==1)
    {
      mvprintw(21,22,"正在将统计结果写入本日90记录中!");
      refresh();
      getch();
    }
    note("将统计数组结果写入各科目的90标志记录中!");

    for (ii=1;ii<=item_count;ii++)
    {
    	sprintf(sql,"update sumtable set det_amt=%ld,cre_amt=%ld,open_que=%d,\
        cls_que=%d,dvou_que=%d,cvou_que=%d,balance=%f,rest_que=%d\
        where branch=%d and kind=90 and item_num = %d",
        sum[ii].d_x_amt+sum[ii].d_z_amt,sum[ii].j_x_amt+sum[ii].j_z_amt,sum[ii].open_num,sum[ii].close_num,
        sum[ii].d_x_num+sum[ii].d_z_num,sum[ii].j_x_num+sum[ii].j_z_num,sum[ii].today_bal,sum[ii].today_home,branch,sum[ii].item_num);
      if(mysql_query(conn,sql)) 
      {
        sprintf(sys_err_str,"修改总帐错!item=%ld.%s",sum[ii].item_num,mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        return(-1);
      }
    }
    if (debug==1)
    {
      mvprintw(21,22,"修改所文件中的记总帐标志!");
      refresh();
      getch();
    }
    
    sprintf(sql,"update br_file set sum_flag=1 where branch=%d",branch);
    if(mysql_query(conn,sql))
    {
      sprintf(sys_err_str,"修改所文件错!%s",mysql_error(conn));
      dis_str(23,10,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      return(-1);
    }
    mysql_commit(conn);

    note("显示各科目统计结果!");
    for(ii=1;ii<=item_count;ii++)
    {
      li=6;
      note("当前科目无发生情况不处理!");
      if(sum[ii].j_z_num+sum[ii].j_x_num+sum[ii].j_z_amt+sum[ii].j_x_amt+sum[ii].d_z_num+sum[ii].d_x_num+sum[ii].d_z_amt+sum[ii].d_x_amt+sum[ii].open_num+sum[ii].close_num==0.00)
        continue;
      clear_scr(li,22,22,77);
      mvprintw(li+1,25,"所号[%3d]      全所科目[%ld]日结单 ",branch,sum[ii].item_num);
      mvprintw(li+2,25,"===================================================");
      mvprintw(li+3,25,"    借         方               贷         方    ");
      mvprintw(li+4,25,"===================================================");
      mvprintw(li+5,25,"现金笔数     现金发生额      现金笔数    现金发生额");
      mvprintw(li+6,25," %5d       %10.2lf       %5d      %10.2lf ",sum[ii].j_x_num,sum[ii].j_x_amt/100.00,sum[ii].d_x_num,sum[ii].d_x_amt/100.00);
      mvprintw(li+7,25,"转帐笔数     转帐发生额      转帐笔数    转帐发生额");
      mvprintw(li+8,25," %5d       %10.2lf       %5d      %10.2lf ",sum[ii].j_z_num,sum[ii].j_z_amt/100.00,sum[ii].d_z_num,sum[ii].d_z_amt/100.00);
      mvprintw(li+9,25,"合计笔数     合计发生额      合计笔数    合计发生额");
      mvprintw(li+10,25," %5d       %10.2lf       %5d      %10.2lf ",sum[ii].j_z_num+sum[ii].j_x_num,(sum[ii].j_z_amt+sum[ii].j_x_amt)/100.00,sum[ii].d_z_num+sum[ii].d_x_num,(sum[ii].d_z_amt+sum[ii].d_x_amt)/100.00);
      mvprintw(li+12,25,"  开户数:%5d       销户数:%5d ",sum[ii].open_num,sum[ii].close_num);
      mvprintw(li+13,25,"===================================================");
      refresh();
    }
    
    dis_err("处理完毕! ");  
  return(0);
}

get_i_item_ws(item)
long item;
{
   short i;
   for (i=1;i<=item_count;i++)
   {
     if (sum[i].item_num==item)
       break;
   }
   if (i>item_count)
     return(-1);
   else
     return(i);
}
