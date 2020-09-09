/****************************************************************
程序名:pr_total.ec
函数名:pr_total()
功  能:生成指定所号的科目日结单
****************************************************************/

#include <stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern char sys_op_name[9];
extern short sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

short item_count,vou_count;

struct to_sum sum[100];

struct to_vou vou[50];


pr_total()
{
  FILE *fp;
  struct s_trade tt;
  struct f_item  it;
  struct f_branch  br;
  struct f_voufile  vo;
  struct in_data da;
  char yy;
  static short in[2]={27,8};
  short num=2,flag;
 
  short itt,li,ii,nn,ii_s,ii101,ii164,iiint;
  short code,co12,co34;
  char cro[2],abst[2];
  short debug=0;
  char file[55],cmd_str[100];
  short branch_id;
  short inttax;
  long int_item;
  
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  while(1)//TODO: continue会造成死循环
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,22,"《打印科目日结单》");
    refresh();
    dis_str(22,22,"正在处理,请稍侯......",0,0);  

    da.tr_br=100;
    branch_id=da.tr_br;

    note("取所号文件中指定所记录");
	
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
    
    note("从item_file中读科目号及相关信息!");	
	
	  sprintf(sql,"SELECT * FROM item_file WHERE  item_flag != 'K'");
  	
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      getch();
      continue;
    }
	
    res=mysql_use_result(conn);

    if(mysql_errno(conn)!=0)
    {
      snprintf(sys_err_str,95,"打开游标错!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
  
    nn=0;
	
    row=mysql_fetch_row(res);
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(mysql_errno(conn)!=0)
        break;
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
      sum[nn].yes_bal=0;
      sum[nn].today_bal=0;
	  //行数小于100？
    }
    mysql_free_result(res);
	
    item_count=nn;
   
    note("从vou_file中读凭证及相关信息!");
    nn=0;
	
	  sprintf(sql,"SELECT * FROM vou_file WHERE  flag != 'K'");
  	
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      continue;
    }
	
    res=mysql_use_result(conn);

    if(mysql_errno(conn)!=0)
    {
      snprintf(sys_err_str,95,"打开游标错!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
  
    nn=0;
	
    row=mysql_fetch_row(res);
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(mysql_errno(conn)!=0)
        break;
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
   
	  
    note("得到现金科目101000的数组下标!");
    ii101=get_i_item_pt(101000);
    if (ii101<0)
    {
      sprintf(sys_err_str,"无现金科目[101000]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }

    inttax=get_i_item_pt(532001);
    if (inttax<0)
    {
      sprintf(sys_err_str,"无扣税科目[532001]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }

    note("得到辖内往来科目164000的数组下标!");
    ii164=get_i_item_pt(164000);
    if (ii164<0)
    {
      sprintf(sys_err_str,"无现金科目[164000]对应的数组下标!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }

    note("从trade中取流水!");
    sprintf(sql,"SELECT * FROM trade\
       WHERE branch = %d AND operator>=%d AND correct='A'\
       ORDER BY operator,ser_num",da.tr_br);	
	
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      continue;
    }
	
    res=mysql_use_result(conn);

    if(mysql_errno(conn)!=0)
    {
      snprintf(sys_err_str,95,"打开游标错!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
	
  clear_scr(6,20,22,77);
  mvprintw(10,22,"              当前处理流水内容");
  mvprintw(11,22,"==================================================");
  mvprintw(13,22,"所号 柜员 交易号 交易码 标志 科目号  金额   利息  ");
  mvprintw(16,22,"==================================================");
  refresh();
  
  row=mysql_fetch_row(res);
  for(;row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
      break;
	set_s_trade(&tt,row);
    mvprintw(14,22,"%3d   %2d   %4d   %4d   %1s   %6ld %7.2lf %6.2lf ", tt.branch,tt.tr_op,tt.ser_num,tt.tr_code,tt.correct,tt.item_num,tt.amount/100.00,tt.interest/100.00);
    refresh();
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

      note("操作码后两位大于10,为非正常业务,不需处理!");
      if (co34>10)
      {
        note("非正常业务!");
        continue;
      }
      if (co12<=15)
      {
        note("正常业务则得到本业务科目数组下标!");
        ii=get_i_item_pt(tt.item_num);
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
          if (abst[0]=='0')
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
          else if (abst[0]=='1')
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
          iiint=get_i_item_pt(int_item);
          if (iiint<0)
          {
            sprintf(sys_err_str,"无科目[%ld]对应的利息科目1!",tt.item_num);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          if (abst[0]=='0')
          {
            note("记原科目现金业务!");
            note("借方现金笔数");
            sum[ii].j_x_num++;
            note("借方现金金额");
            sum[ii].j_x_amt+=tt.amount;
            note("销户数");
            sum[ii].close_num++;

            note("记本科目对应利息科目!");
            note("贷方现金笔数");
            sum[iiint].d_x_num++;
            note("贷方现金金额");
            sum[iiint].d_x_amt+=tt.interest;

            sum[inttax].j_x_num++;
            sum[inttax].j_x_amt+=tt.tax_int ;

            note("记现金科目业务!");
            note("贷方现金笔数");
            sum[ii101].d_x_num++;
            note("贷方现金金额");
            sum[ii101].d_x_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          else if (abst[0]=='1')
          {
            note("记原科目转帐业务!");
            note("借方转帐笔数");
            sum[ii].j_z_num++;
            note("借方转帐金额");
            sum[ii].j_z_amt+=tt.amount;
            note("销户数");
            sum[ii].close_num++;

            sum[inttax].j_z_num++;
            sum[inttax].j_z_amt+=tt.tax_int ;

            note("记本科目对应利息科目!");
            note("贷方转帐笔数");
            sum[iiint].d_z_num++;
            note("贷方转帐金额");
            sum[iiint].d_z_amt+=tt.interest;

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum[ii164].d_x_num++;
            note("贷方现金金额");
            sum[ii164].d_x_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          break;
        case 3:
          note("续存业务!");
          if (abst[0]=='0')
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
          else if (abst[0]=='1')
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
          if (abst[0]=='0')
          {
            note("记原科目现金业务!");
            note("借方现金笔数");
            sum[ii].j_x_num++;
            note("借方现金金额");
            sum[ii].j_x_amt+=tt.amount;
            note("销户数");
            sum[ii].close_num++;

            note("记现金科目业务!");
            note("贷方现金笔数");
            sum[ii101].d_x_num++;
            note("贷方现金金额");
            sum[ii101].d_x_amt+=tt.amount;
          }
          else if (abst[0]=='1')
          {
            note("记原科目转帐业务!");
            note("借方转帐笔数");
            sum[ii].j_z_num++;
            note("借方转帐金额");
            sum[ii].j_z_amt+=tt.amount;

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum[ii164].d_x_num++;
            note("贷方现金金额");
            sum[ii164].d_x_amt+=tt.amount;
          }
          break;
        case 5:
          note("部提业务!");
          note("按原户销户后再开新户处理!");

          note("找销户科目的对转活期利息科目的下标!");
          int_item=found("S",tt.term,tt.item_num);
          if (ii_s<0)
          {
            sprintf(sys_err_str,"无活期对应的利息科目!");
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          ii_s=get_i_item_pt(int_item);
          if (iiint<0)
          {
            sprintf(sys_err_str,"无科目[%ld]对应的利息科目!",int_item);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          if (abst[0]=='0')
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
            note("贷方现金笔数");
            sum[ii_s].d_x_num++;
            note("贷方现金金额");
            sum[ii_s].d_x_amt+=tt.interest;

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
          else if (abst[0]=='1')
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

            note("记辖内往来科目业务!");
            note("贷方现金笔数");
            sum[ii164].d_z_num++;
            note("贷方现金金额");
            sum[ii164].d_z_amt+=tt.balance+tt.interest+tt.tax_int;
            sum[inttax].j_z_num++;
            sum[inttax].j_z_amt+=tt.tax_int ;


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
          note("根据储种修改对应凭证使用数量!");
          up_vou(tt.type,"U",1);
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
          ii=get_i_item_pt(tt.item_num);
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
	

    sprintf(file,"./tmp/tmp_total_%d",branch_id);
    if((fp=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"不能生成科目日结单!",60,0);
      refresh();
      getch();       
      return(-1);
    }
    fprintf(fp,"                          科目日结单\n");
    fprintf(fp,"┏━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    fprintf(fp,"┃  科  ┃                  借                  方                  ┃                  贷                  方                  ┃\n");
    fprintf(fp,"┃      ┣━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━╋━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┫\n");
    fprintf(fp,"┃  目  ┃     现    金     ┃     转    帐     ┃     合    计     ┃      现    金    ┃      转    帐    ┃     合    计     ┃\n");
    fprintf(fp,"┃      ┣━━┳━━━━━━╋━━┳━━━━━━╋━━┳━━━━━━╋━━┳━━━━━━╋━━┳━━━━━━╋━━┳━━━━━━┫\n");
    fprintf(fp,"┃  号  ┃笔数┃   发生额   ┃笔数┃   发生额   ┃笔数┃   发生额   ┃笔数┃   发生额   ┃笔数┃   发生额   ┃笔数┃   发生额   ┃\n");

    note("显示统计结果!");
    note("显示各科目统计结果!");
    for(ii=1;ii<=item_count;ii++)
    {
      li=6;
      note("当前科目无发生情况不处理!");
      if(sum[ii].j_z_num+sum[ii].j_x_num+sum[ii].j_z_amt+sum[ii].j_x_amt+sum[ii].d_z_num+sum[ii].d_x_num+sum[ii].d_z_amt+sum[ii].d_x_amt+sum[ii].open_num+sum[ii].close_num==0.00)
        continue;
      fprintf(fp,"┣━━━╋━━╋━━━━━━╋━━╋━━━━━━╋━━╋━━━━━━╋━━╋━━━━━━╋━━╋━━━━━━╋━━╋━━━━━━┫\n");
      fprintf(fp,"┃%6ld┃%4d┃%12.2lf┃%4d┃%12.2lf┃%4d┃", sum[ii].item_num,sum[ii].j_x_num,sum[ii].j_x_amt/100.00,sum[ii].j_z_num,sum[ii].j_z_amt/100.00, sum[ii].j_x_num+sum[ii].j_z_num);
      fprintf(fp,"%12.2lf┃%4d┃%12.2lf┃%4d┃%12.2lf┃%4d┃%12.2lf┃\n",(sum[ii].j_x_amt+sum[ii].j_z_amt)/100.00, sum[ii].d_x_num,sum[ii].d_x_amt/100.00,sum[ii].d_z_num,sum[ii].d_z_amt/100.00, sum[ii].d_x_num+sum[ii].d_z_num,(sum[ii].d_x_amt+sum[ii].d_z_amt)/100.00);

    }
    fprintf(fp,"┗━━━┻━━┻━━━━━━┻━━┻━━━━━━┻━━┻━━━━━━┻━━┻━━━━━━┻━━┻━━━━━━┻━━┻━━━━━━┛\n");
    note("显示各凭证统计结果!");
    fprintf(fp,"\n\n            全所凭证使用统计");
    fprintf(fp,"\n===================================================");
    fprintf(fp,"\n类型  名    称  剩余数 使用数 作废数 换折数 交回数");
    fprintf(fp,"\n===================================================");
    for(ii=1;ii<=vou_count;ii++)
    {
/**
      if(vou[ii].rest_que+vou[ii].use_que+vou[ii].cancel+vou[ii].chg_book+vou[ii].takeback==0)
        continue;
**/
      fprintf(fp,"\n %2s %10s   %4d   %4d   %4d   %4d   %4d ",vou[ii].vou_type,vou[ii].vou_name,vou[ii].rest_que,vou[ii].use_que,vou[ii].cancel,vou[ii].chg_book,vou[ii].takeback);
    }
    fprintf(fp,"\n===================================================\n");
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
    break;
  }
  return(0);
}

/*****************************************************************
函数名:up_vou(type,flag,num)
功  能:根据储种修改对应凭证数量
参  数:
       char type[2];储种: D_定 H_活 
       char flag[2];修改类型 R_领回 U_使用 K_作废 C_换折 T_交回
       short num;   修改数量
******************************************************************/
up_vou(type,flag,num)
char type[2],flag[2];
short num;
{
  short i;

  note("根据对应储种找到凭证数组中的对应下标!");
  for(i=1;i<=vou_count;i++)
  {
    if (vou[i].type[0]==type[0])
      break;
  }
  if (i>vou_count)
  {
    dis_str(22,22,"无对应凭证类型!",60,0);
    getch();
    return(-1);
  }

  switch (flag[0])
  {
    case 'R':
      vou[i].rest_que+=num;
      break;
    case 'U':
      vou[i].use_que+=num;
      vou[i].rest_que-=num;
      break;
    case 'K':
      vou[i].cancel+=num;
      vou[i].rest_que-=num;
      break;
    case 'C':
      vou[i].chg_book+=num;
      vou[i].rest_que-=num;
      break;
    case 'T':
      vou[i].takeback+=num;
      vou[i].rest_que-=num;
      break;
  }
  return(0);
}

/***************************************
函数名:found(type,term,item)
功  能:找销户科目的对转利息科目
参  数:
       char  type[2];储种
       long  item;   科目号
       short term;   存期
***************************************/
found(type,term,item)
char type[2];
long item;
short term;
{
  long int_item;
  short ii;

  note("根据储种和存期决定科目");

  switch(type[0])
  {
    case 'H':
      note("活期"); 

      int_item=521100;
      break;
    case 'D':
      note("定期"); 

      if (term<=6)
        int_item=521300;
      else
        int_item=521400;
      break;

    default:
      int_item=-1;
      break;
  }
  if (int_item<0)
    return(-1);
  else
    return(int_item);
}

get_i_item_pt(item)
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



