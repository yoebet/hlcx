/****************************************************************
程序名:total_op.ec
函数名:total_op()
功  能:生成指定所号及操作员的的科目日结单
代  码:1812
流  程:  
      1.输入
      2.从科目文件中取所有科目存入数组sum_op
      3.从日志中取本所的所有记录
      4.根据交易码统计各科目及各凭证的有关数据
****************************************************************/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "../include/sh_table.h"
#include "mysql.h"

extern char sys_err_str[];
extern char sys_op_name[9];
extern short sys_branch,sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

short item_count,vou_count;
struct to_sum  sum_op[100];
struct to_vou  vou[50];

total_op()
{
  struct s_trade tt;
  struct f_item  it;
  struct f_voufile  vo;
  struct in_data da;
  char yy;
  FILE *fp,*fp1,*fopen();
  char ddd[150];
  static short in[3]={27,31,8};
  short num=3,flag;
 
  long int_item;
  short itt,li,ii,nn,ii_s,ii101,ii164,iiint;
  short code,co12,co34;
  char cro[2],abst[2];
  short debug=0;
  short inttax;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.tr_br=sys_branch;
  da.tr_op =sys_operator;
  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"《按操作员轧帐》");
    refresh();

    if (debug==1)
    {
      mvprintw(21,22,"正在初始化日志表!");
      refresh();
      getch();
    }

    init_t(&tt);

    if (debug==1)
    {
      mvprintw(21,22,"正在初始化凭证数组!");
      refresh();
      getch();
    }
    note("初始化凭证数组!");
    for (ii=1;ii<=9;ii++)
    {
      strcpy(vou[ii].vou_type," ");
      vou[ii].rest_que=0;
      vou[ii].use_que=0;
      vou[ii].cancel=0;
      vou[ii].chg_book=0;
      vou[ii].takeback=0;
    }
    
    if (debug==1)
    {
      mvprintw(21,22,"正在从item_file中读科目号及相关信息!");
      refresh();
      getch();
    }
    note("从item_file中读科目号及相关信息!");
    nn=0;
    
	  sprintf(sql,"select * from item_file where item_flag!='K'");
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"取科目错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break; //由于没有用户输入，continue 会死循环。以下均是
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"没有科目!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
    }
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_f_item(&it,row);
      nn++;
      sum_op[nn].item_num=it.item_num;
      strcpy(sum_op[nn].item_name,it.item_name);
      sum_op[nn].sum_flag=it.sum_flag;
      sum_op[nn].prt_ser=it.prt_ser;
      sum_op[nn].j_x_num=0;
      sum_op[nn].j_x_amt=0;
      sum_op[nn].j_z_num=0;
      sum_op[nn].j_z_amt=0;
      sum_op[nn].d_x_num=0;
      sum_op[nn].d_x_amt=0;
      sum_op[nn].d_z_num=0;
      sum_op[nn].d_z_amt=0;
      sum_op[nn].open_num=0;
      sum_op[nn].close_num=0;
      sum_op[nn].yes_bal=0;
      sum_op[nn].today_bal=0;
    }
    mysql_free_result(res);
    item_count=nn;
   
    
    if (debug==1)
    {
      mvprintw(21,22,"正在从vou_file中读凭证及相关信息!");
      refresh();
      getch();
    }
    note("从vou_file中读凭证及相关信息!");
    nn=0;
    
	  sprintf(sql,"select * from vou_file where flag!='K'");
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"取凭证错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"没有凭证!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
    }
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_f_voufile(&vo,row);
      nn++;
      strcpy(vou[nn].vou_type,vo.vou_type);
      strcpy(vou[nn].vou_name,vo.vou_name);
      strcpy(vou[nn].type,vo.type);
      vou[nn].rest_que=0;
      vou[nn].use_que=0;
      vou[nn].cancel=0;
      vou[nn].chg_book=0;
      vou[nn].takeback=0;
    }
    mysql_free_result(res);
    vou_count=nn;
   

    if (debug==1)
    {
      mvprintw(21,22,"正在得到现金科目101000的数组下标!");
      refresh();
      getch();
    }
    note("得到现金科目101000的数组下标!");
    ii101=get_i3_item(101000);
    if (ii101<0)
    {
      sprintf(sys_err_str,"无现金科目[101000]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      //getch();
      //continue;
    }

    note("得到辖内往来科目164000的数组下标!");
    ii164=get_i3_item(164000);
    if (ii164<0)
    {
      sprintf(sys_err_str,"无现金科目[164000]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      //getch();
      //continue;
    }

    inttax=get_i3_item(532001);
    if (inttax<0)
    {
      sprintf(sys_err_str,"无扣税科目[532001]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      //getch();
      //continue;
    }
    if (debug==1)
    {
      mvprintw(21,22,"正在从trade中取流水!");
      refresh();
      getch();
    }
    note("从trade中取流水!");
    
	  sprintf(sql,"select * from trade where branch=%d and operator=%d order by operator,ser_num",da.tr_br,da.tr_op);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"取流水错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"没有流水!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
    }
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_s_trade(&tt,row);
      code=tt.tr_code;
      co12=tt.tr_code/100;
      co34=tt.tr_code%100;
      strcpy(abst,tt.abstract);
      strcpy(cro,tt.correct);

      note("开户冲正需统计使用凭证数!");
      if (cro[0]=='K' && co34==1)
      {
        note("根据储种修改对应凭证使用数量!");
        up_vou(tt.type,"U",1);
        continue;
      }

      if (cro[0]=='K')
        continue;
      note("操作码后两位大于10,为非正常业务,不需处理!");
      if (co34>10)
      {
        note("非正常业务!");
        continue;
      }
      note("得到本业务科目数组下标!");
      ii=get_i3_item(tt.item_num);
      if (ii<0)
      {
        sprintf(sys_err_str,"无科目[%ld]对应的数组下标!",tt.item_num);
        dis_str(22,22,sys_err_str,60,0);
        getch();
        continue;
      }
      switch (co34)
      {
        case 1:
          note("开户业务!");
          if (abst[0]=='0')
          {
            note("记原科目现金业务!");
            note("贷方现金笔数");
            sum_op[ii].d_x_num++;
            note("贷方现金金额");
            sum_op[ii].d_x_amt+=tt.amount;
            note("开户数");
            sum_op[ii].open_num++;

            note("记现金科目业务!");
            note("借方现金笔数");
            sum_op[ii101].j_x_num++;
            note("借方现金金额");
            sum_op[ii101].j_x_amt+=tt.amount;
          }
          else
          {
            note("记原科目转帐业务!");
            note("贷方转帐笔数");
            sum_op[ii].d_z_num++;
            note("贷方转帐金额");
            sum_op[ii].d_z_amt+=tt.amount;
            note("开户数");
            sum_op[ii].open_num++;

            note("记辖内往来科目转帐业务!");
            note("借方转帐笔数");
            sum_op[ii164].j_z_num++;
            note("借方转帐金额");
            sum_op[ii164].j_z_amt+=tt.amount;
          }
          note("根据储种修改对应凭证使用数量!");
          up_vou(tt.type,"U",1);
          break;
        case 2:
          note("销户业务!");

          note("找销户科目的对转利息科目的下标!");
          int_item=found(tt.type,tt.term,tt.item_num);
          if (int_item<0)
          {
            sprintf(sys_err_str,"无科目[%ld]对应的利息科目!",tt.item_num);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          iiint=get_i1_item(int_item);
          if (iiint<0)
          {
            sprintf(sys_err_str,"无科目[%ld]对应的数组下标!",int_item);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          note("若为定期提前支取取活期利息!");
          if (tt.interest==tt.s_int)
          {
            iiint=get_i3_item(521100);
            if (iiint<0)
            {
              sprintf(sys_err_str,"无利息科目[521100]对应的数组下标!");
              dis_str(22,22,sys_err_str,60,0);
              getch();
              continue;
            }
          }
          if (abst[0]=='0')
          {
            note("记原科目现金业务!");
            note("借方现金笔数");
            sum_op[ii].j_x_num++;
            note("借方现金金额");
            sum_op[ii].j_x_amt+=tt.amount;
            note("销户数");
            sum_op[ii].close_num++;

            note("记本科目对应利息科目!");
            note("贷方现金笔数");
            sum_op[iiint].j_x_num++;
            note("贷方现金金额");
            sum_op[iiint].j_x_amt+=tt.interest;
            sum_op[inttax].j_x_num++;
            sum_op[inttax].j_x_amt+=tt.tax_int ;

            note("记现金科目业务!");
            note("贷方现金笔数");
            sum_op[ii101].d_x_num++;
            note("贷方现金金额");
            sum_op[ii101].d_x_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          else
          {
            note("记原科目转帐业务!");
            note("借方转帐笔数");
            sum_op[ii].j_z_num++;
            note("借方转帐金额");
            sum_op[ii].j_z_amt+=tt.amount;
            note("销户数");
            sum_op[ii].close_num++;

            note("记本科目对应利息科目!");
            note("贷方转帐笔数");
            sum_op[iiint].j_z_num++;
            note("贷方转帐金额");
            sum_op[iiint].j_z_amt+=tt.interest;
            sum_op[inttax].j_z_num++;
            sum_op[inttax].j_z_amt+=tt.tax_int ;

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum_op[ii164].d_z_num++;
            note("贷方现金金额");
            sum_op[ii164].d_z_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          break;
        case 3:
          note("续存业务!");
          if (abst[0]=='0')
          {
            note("记原科目现金业务!");
            note("贷方现金笔数");
            sum_op[ii].d_x_num++;
            note("贷方现金金额");
            sum_op[ii].d_x_amt+=tt.amount;

            note("记现金科目业务!");
            note("借方现金笔数");
            sum_op[ii101].j_x_num++;
            note("借方现金金额");
            sum_op[ii101].j_x_amt+=tt.amount;
          }
          else
          {
            note("记原科目转帐业务!");
            note("贷方转帐笔数");
            sum_op[ii].d_z_num++;
            note("贷方转帐金额");
            sum_op[ii].d_z_amt+=tt.amount;

            note("记辖内往来科目转帐业务!");
            note("借方转帐笔数");
            sum_op[ii164].j_z_num++;
            note("借方转帐金额");
            sum_op[ii164].j_z_amt+=tt.amount;
          }
          break;
        case 4:
          note("支取业务!");
          if (abst[0]=='0')
          {
            note("记原科目现金业务!");
            note("借方现金笔数");
            sum_op[ii].j_x_num++;
            note("借方现金金额");
            sum_op[ii].j_x_amt+=tt.amount;

            note("记现金科目业务!");
            note("贷方现金笔数");
            sum_op[ii101].d_x_num++;
            note("贷方现金金额");
            sum_op[ii101].d_x_amt+=tt.amount;
          }
          else
          {
            note("记原科目转帐业务!");
            note("借方转帐笔数");
            sum_op[ii].j_z_num++;
            note("借方转帐金额");
            sum_op[ii].j_z_amt+=tt.amount;

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum_op[ii164].d_z_num++;
            note("贷方现金金额");
            sum_op[ii164].d_z_amt+=tt.amount;
          }
          break;
        case 5:
          note("部提业务!");
          note("按原户销户后再开新户处理!");

          note("找销户科目的对转活期利息科目的下标!");
          int_item=found("S",tt.term,tt.item_num);
          if (int_item<0)
          {
            sprintf(sys_err_str,"无活期对应的利息科目!");
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          ii_s=get_i1_item(int_item);
          if (ii_s<0)
          {
            sprintf(sys_err_str,"无科目[%ld]对应的数组下标!",int_item);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          if (abst[0]=='0')
          {
            note("记原科目现金业务!");

            note("原户销户!");

            note("借方现金笔数");
            sum_op[ii].j_x_num++;
            note("借方现金金额");
            sum_op[ii].j_x_amt+=tt.balance;
            note("销户数");
            sum_op[ii].close_num++;

            note("记本科目对应利息科目!");
            note("贷方现金笔数");
            sum_op[ii_s].j_x_num++;
            note("贷方现金金额");
            sum_op[ii_s].j_x_amt+=tt.interest;
            sum_op[inttax].j_x_num++;
            sum_op[inttax].j_x_amt+=tt.tax_int ;

            note("记现金科目业务!");
            note("贷方现金笔数");
            sum_op[ii101].d_x_num++;
            note("贷方现金金额");
            sum_op[ii101].d_x_amt+=tt.balance+tt.interest+tt.tax_int;

            note("新户开户!");
            note("贷方现金笔数");
            sum_op[ii].d_x_num++;
            note("贷方现金金额");
            sum_op[ii].d_x_amt+=tt.balance-tt.amount;
            note("开户数");
            sum_op[ii].open_num++;

            note("记现金科目业务!");
            note("借方现金笔数");
            sum_op[ii101].j_x_num++;
            note("借方现金金额");
            sum_op[ii101].j_x_amt+=tt.balance-tt.amount;

          }
          else
          {
            note("记原科目转帐业务!");
            note("原户销户!");

            note("借方现金笔数");
            sum_op[ii].j_z_num++;
            note("借方现金金额");
            sum_op[ii].j_z_amt+=tt.balance;
            note("销户数");
            sum_op[ii].close_num++;

            note("记本科目对应利息科目!");
            note("贷方现金笔数");
            sum_op[ii_s].j_z_num++;
            note("贷方现金金额");
            sum_op[ii_s].j_z_amt+=tt.interest;
            sum_op[inttax].j_z_num++;
            sum_op[inttax].j_z_amt+=tt.tax_int ;

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum_op[ii164].d_z_num++;
            note("贷方现金金额");
            sum_op[ii164].d_z_amt+=tt.balance+tt.interest+tt.tax_int;

            note("新户开户!");
            note("贷方现金笔数");
            sum_op[ii].d_z_num++;
            note("贷方现金金额");
            sum_op[ii].d_z_amt+=tt.balance-tt.amount;
            note("开户数");
            sum_op[ii].open_num++;

            note("记辖内往来科目业务!");
            note("借方现金笔数");
            sum_op[ii164].j_z_num++;
            note("借方现金金额");
            sum_op[ii164].j_z_amt+=tt.balance-tt.amount;
          }
          break;
        case 6:
          note("挂失补开!");
          note("根据储种修改对应凭证使用数量!");
          up_vou(tt.type,"U",1);
          break;
        case 8:
          note("凭证调整!");
          note("根据调整的凭证类型,数量及调整内容修改对应凭证使用数量!");
          note("修改类型 R_领回 U_使用 K_作废 C_换折 T_上缴");
          if (tt.det_num==1)
          {
            note("领回");
            up_vou(tt.abstract,"R",tt.term);
          }
          else if (tt.det_num==2)
          {
            note("使用");
            up_vou(tt.abstract,"U",tt.term);
          }
          else if (tt.det_num==3)
          {
            note("作废");
            up_vou(tt.abstract,"K",tt.term);
          }
          else if (tt.det_num==4)
          {
            note("上缴");
            up_vou(tt.abstract,"T",tt.term);
          }
          else if (tt.det_num==5)
          {
            note("换折");
            up_vou(tt.abstract,"C",tt.term);
          }
          break;
        case 9:
          note("总帐调整!");
          note("根据调整的总帐科目金额笔数开/销户数及");
          note("调整内容修改对应科目的内容!");

          note("得到本调整科目的数组下标!");
          ii=get_i3_item(tt.item_num);
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
              sum_op[ii].j_x_num+=tt.term;
              note("借方现金金额");
              sum_op[ii].j_x_amt+=tt.amount;
            }
            else
            {
              note("转帐!");
              note("借方转帐笔数");
              sum_op[ii].j_z_num+=tt.term;
              note("借方转帐金额");
              sum_op[ii].j_z_amt+=tt.amount;
            }
            note("销户数");
            sum_op[ii].close_num+=tt.transfer;
          }
          else
          {
            note("贷方调整!");
            if (tt.abstract[0]=='0')
            {
              note("现金!");
              note("贷方现金笔数");
              sum_op[ii].d_x_num+=tt.term;
              note("贷方现金金额");
              sum_op[ii].d_x_amt+=tt.amount;
            }
            else
            {
              note("转帐!");
              note("贷方转帐笔数");
              sum_op[ii].d_z_num+=tt.term;
              note("贷方转帐金额");
              sum_op[ii].d_z_amt+=tt.amount;
            }
            note("开户数");
            sum_op[ii].open_num+=tt.transfer;
            itt=tt.item_num/100;
            if (itt==2151)
              strcpy(tt.type,"F");
            else if (itt==2111)
              strcpy(tt.type,"S");
            else if (itt==2153)
              strcpy(tt.type,"P");
            else
              strcpy(tt.type,"M");
            note("根据储种修改对应凭证使用数量!");
            up_vou(tt.type,"U",tt.transfer);
          }
          break;
        default:
          break;
      }
      note("当前交易处理完毕!");
    }
    mysql_free_result(res);

    note("显示统计结果!");
    note("显示各科目统计结果!");
    for(ii=1;ii<=item_count;ii++)
    {
      li=6;
      note("当前科目无发生情况不处理!");
      if(sum_op[ii].j_z_num+sum_op[ii].j_x_num+sum_op[ii].j_z_amt+sum_op[ii].j_x_amt+sum_op[ii].d_z_num+sum_op[ii].d_x_num+sum_op[ii].d_z_amt+sum_op[ii].d_x_amt+sum_op[ii].open_num+sum_op[ii].close_num==0.00)
        continue;
      if (sum_op[ii].item_num==532001)
        continue;
      clear_scr(li,22,22,77);
      mvprintw(li+1,15,"  操作员[%2d]   科目[%ld]日结单 ",da.tr_op,sum_op[ii].item_num);
      mvprintw(li+2,15,"===================================================");
      mvprintw(li+3,15,"    借         方               贷         方    ");
      mvprintw(li+4,15,"===================================================");
      mvprintw(li+5,15,"现金笔数     现金发生额      现金笔数    现金发生额");
      mvprintw(li+6,15," %5d       %10.2lf       %5d      %10.2lf ",sum_op[ii].j_x_num,sum_op[ii].j_x_amt/100.00,sum_op[ii].d_x_num,sum_op[ii].d_x_amt/100.00);
      //mvprintw(li+7,15,"转帐笔数     转帐发生额      转帐笔数    转帐发生额");
      //mvprintw(li+8,15," %5d       %10.2lf       %5d      %10.2lf ",sum_op[ii].j_z_num,sum_op[ii].j_z_amt/100.00,sum_op[ii].d_z_num,sum_op[ii].d_z_amt/100.00);
      //mvprintw(li+9,15,"合计笔数     合计发生额      合计笔数    合计发生额");
      //mvprintw(li+10,15," %5d       %10.2lf       %5d      %10.2lf ",sum_op[ii].j_z_num+sum_op[ii].j_x_num,(sum_op[ii].j_z_amt+sum_op[ii].j_x_amt)/100.00,sum_op[ii].d_z_num+sum_op[ii].d_x_num,(sum_op[ii].d_z_amt+sum_op[ii].d_x_amt)/100.00);
      mvprintw(li+12,15,"  开户数:%5d       销户数:%5d ",sum_op[ii].open_num,sum_op[ii].close_num);
      mvprintw(li+13,15,"===================================================");
      refresh();
      dis_str(22,22,"请检查,按 q_退出,其它键继续!",0,0);
      yy=getch();
      if (yy=='q' || yy=='Q')
        break;
    }
    note("显示各凭证统计结果!");
    li=7;
    clear_scr(li,22,22,77);
    mvprintw(li+1,15,"     操作员[%2d]     凭证使用统计",da.tr_op);
    mvprintw(li+2,15,"===================================================");
    mvprintw(li+3,15,"类型      名称       使用数  作废数  换折数  交回数");
    mvprintw(li+4,15,"===================================================");
    for(ii=1;ii<=vou_count;ii++)
    {
      mvprintw(li+4+ii,15," %2s    %10s     %4d    %4d    %4d    %4d ",vou[ii].vou_type,vou[ii].vou_name,vou[ii].use_que,vou[ii].cancel,vou[ii].chg_book,vou[ii].takeback);
    }
    mvprintw(li+4+ii,15,"===================================================");
    refresh();


  mvprintw(22,20,"  是否打印(Y/y_打印,其它键退出)!");
  refresh();
  yy=getch();
  if (yy !='Y' && yy!='y')
	{
	
    dis_err("处理完毕! ");  
      break;
	}
     	fp=fopen("/tmp/zzd_op","w");
	if(fp==NULL)
        {
		dis_err("不能打开临时文件/tmp/dzd_op!");
		return(-1);
   	}

    for(ii=1;ii<=item_count;ii++)
    {
      li=6;
      if(sum_op[ii].j_z_num+sum_op[ii].j_x_num+sum_op[ii].j_z_amt+sum_op[ii].j_x_amt+sum_op[ii].d_z_num+sum_op[ii].d_x_num+sum_op[ii].d_z_amt+sum_op[ii].d_x_amt+sum_op[ii].open_num+sum_op[ii].close_num==0.00)
        continue;
      if (sum_op[ii].item_num==532001)
        continue;
      fprintf(fp,"  操作员[%2d]   科目[%ld]日结单\n",da.tr_op,sum_op[ii].item_num);
      fprintf(fp,"===================================================\n");
      fprintf(fp,"    借         方               贷         方    \n");
      fprintf(fp,"===================================================\n");
      fprintf(fp,"现金笔数     现金发生额      现金笔数    现金发生额\n");
      fprintf(fp," %5d       %10.2lf       %5d      %10.2lf \n",sum_op[ii].j_x_num,sum_op[ii].j_x_amt/100.00,sum_op[ii].d_x_num,sum_op[ii].d_x_amt/100.00);
      //fprintf(fp,"转帐笔数     转帐发生额      转帐笔数    转帐发生额\n");
      //fprintf(fp," %5d       %10.2lf       %5d      %10.2lf \n",sum_op[ii].j_z_num,sum_op[ii].j_z_amt/100.00,sum_op[ii].d_z_num,sum_op[ii].d_z_amt/100.00);
      //fprintf(fp,"合计笔数     合计发生额      合计笔数    合计发生额\n");
      //fprintf(fp," %5d       %10.2lf       %5d      %10.2lf \n",sum_op[ii].j_z_num+sum_op[ii].j_x_num,(sum_op[ii].j_z_amt+sum_op[ii].j_x_amt)/100.00,sum_op[ii].d_z_num+sum_op[ii].d_x_num,(sum_op[ii].d_z_amt+sum_op[ii].d_x_amt)/100.00);
      fprintf(fp,"  开户数:%5d       销户数:%5d \n",sum_op[ii].open_num,sum_op[ii].close_num);
      fprintf(fp,"===================================================\n");
    }

    fprintf(fp,"    操作员[%2d]     凭证使用统计\n",da.tr_op);
    fprintf(fp,"===================================================\n");
    fprintf(fp,"类型  名    称  剩余数 使用数 作废数 换折数 交回数\n");
    fprintf(fp,"===================================================\n");
    for(ii=1;ii<=vou_count;ii++)
    {
      fprintf(fp," %2s %10s   %4d   %4d   %4d   %4d   %4d \n",vou[ii].vou_type,vou[ii].vou_name,vou[ii].rest_que,vou[ii].use_que,vou[ii].cancel,vou[ii].chg_book,vou[ii].takeback);
    }
    fprintf(fp,"===================================================\n");
    fclose(fp);

     fp1=fopen("/tmp/zzd_op","r");
     openlp();
     newline(3);
     while ( !feof(fp1)) 
     {
     	fgets(ddd,150,fp1);
       	printf("%s",ddd);
       	sleep(1);
     }
     newline(1);
     backpage();
     closelp(); 
     fclose(fp1);
      break;

      break;
  }
  return(0);
}

/*********************************************
函数名:get_i3_item(item)
功  能:根据科目号得到对应数组下标
参  数:
       long item; 科目号
**********************************************/
get_i3_item(item)
long item;
{
   short i;
   for (i=1;i<=item_count;i++)
   {
     if (sum_op[i].item_num==item)
       break;
   }
   if (i>item_count)
     return(-1);
   else
     return(i);
}
