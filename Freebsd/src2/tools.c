
/***************************************************
ȡ����                get_rate(type,term)           
ȡ����ʺ�            get_acc(accbranch,acctype)     
ȡ��ˮ��              get_ser(serbranch)              
ȡ��ˮ��              get_oldser(oldbranch,oldoperator)
ȡ��Ŀ��              get_item(type1, acc_type, term1)
����ƾ֤��            update_vou(voubranch,vou_type,vouflag)
���㴢������          day_s(sd,ed)
��������              change_date(sd,ed)
���㵽����            foo(term,ed)
���㵽����            get_enddate(d, t)
�ж��Ƿ���          conv_date(sd,ed,term)
���������Ϣ����      s_int(mday,amt)
���㰴����Ϣ�Ļ���    s_zint(mday,amt)
�Ƚ��������ڵĴ�С    compday(sd,ed)
��ʼ��˫���ֻ���      init_f(ff)
��ʼ����־            init_t(tt)
��ʾ˫���ֻ���        disp_f(ff)
���ķ�ʽ��ʾ˫���ֻ���disp_acc(fff)
��������ʱ������ϸ    update_p_detail()
��ʼ�������ֻ���      init_p(pp)
��ʾ�����ֻ���        disp_p(ppf)
��ʾ��־����          disp_t(ttt)
��ʾ������code        disp_code(x,y,code) 
��ʾ����type          disp_type(x,y,type)
��ʾ�ʻ�����acctype   disp_acctype(x,y,acctype)
���ڳ���ʱ������ϸ    update_s_detail()
��ʾ���ڷֻ���        disp_s(ssf)
��ʼ�����ڷֻ���      init_s(ss)
������ʾ���ڷֻ���    disp_s_acc(ssf)
������ʾ�����ֻ���    disp_p_acc(ss2)
�����·ݼ�д�õ��·�  get_month(mm)
��str�е�\n��Ϊ�ո�   found_char(str)
���ݴ����޸Ķ�Ӧƾ֤  up_vou(type,flag,num)
���ݿ�Ŀ�ŵõ������±�get_i_item(item)
��������Ŀ����Ϣ��Ŀ  found(type,term,item)
��ʼ�����ʽṹ        init_zh(sum)
��ʾƾ֤              disp_vouch(vou)
****************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern MYSQL *conn;
extern char sql[];

/*****************************
������:get_rate(type,term)
��  ��:ȡָ�����ֺʹ��ڵ�����
��  ��: char   type ����
        short  term ����
����ֵ: long   rate ����
*****************************/
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

/****************************************************
������:get_acc(accbranch,acctype)
��  ��:ȡָ����,���ֵ�����ʺ�
��  ��:short accbranch ����
       char  type      ����
����ֵ:long  account   �ʺ�
****************************************************/
long get_acc(accbranch,acctype)
short accbranch;
char acctype;
{
  long account;
  MYSQL_RES *res;
  MYSQL_ROW row;
  
	sprintf(sql,"select account from acc_file WHERE branch=%d and ltype='%c'",accbranch,acctype);
	if(mysql_query(conn,sql))
	{
	  return(-1);
	}
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL)
	{
	  return(-1);
	}
	account=atol(row[0]);
  mysql_free_result(res);
	
  account=account+1;
	sprintf(sql,"update acc_file set account=%ld WHERE branch=%d and ltype='%c'",account,accbranch,acctype);
	if(mysql_query(conn,sql))
	{
	  return(-1);
	}
  return(account);
}
     
/*******************************************************
������:get_ser(serbranch)
��  ��:����ȡ��ˮ��
��  ��:short  branch  ����
����ֵ:int  ser_num ��ˮ��
*******************************************************/
get_ser(branch)
short branch;
{
  int ser_num;
  short serbranch;
  serbranch=branch;
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  //���������汣֤
  //if(mysql_query(conn,"START TRANSACTION")){
  //  return(-1);
  //}
  
	sprintf(sql,"SELECT ser_num FROM br_file WHERE branch=%d",serbranch);
	mysql_query(conn,sql);
	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL)
	{
    //mysql_rollback(conn);
    return(-1);
	}
	ser_num=atoi(row[0]);
	
  ser_num++;
  
	sprintf(sql,"update br_file set ser_num=%d where branch=%d",ser_num,serbranch);
	if(mysql_query(conn,sql)){
    //mysql_rollback(conn);
    return(-1);
	}
  //mysql_commit(conn);
	
  return(ser_num);
}
     
/******************************************************* 
������:get_oldser(oldbranch,oldoperator)
��  ��:����,����Աȡ��ˮ��
��  ��:short branch ����  short operator ��Ա��
����ֵ:int ser_num ��ˮ��
short get_oldser(oldbranch,oldoperator)
short oldbranch;
short oldoperator;
{
  int ser_num;
  $declare oldser_curs cursor for 
    select ser_num from operfile
    where branch=$oldbranch and operator=$oldoperator;
  $open oldser_curs;
  if(sqlca.sqlcode!=0)
  {
    $close oldser_curs;
    $free  oldser_curs;
    return(SQLCODE);
  }
  $fetch next oldser_curs into $ser_num;
  if(sqlca.sqlcode!=0)
  {
    $close oldser_curs;
    $free  oldser_curs;
    return(SQLCODE);
  }
  $close oldser_curs;
  $free  oldser_curs;
  return(ser_num);
}
*******************************************************/
     
/************************************************************
������:get_item(type1, acc_type, term1)
��  ��:���ݴ��ִ����ʻ�����ȡ��Ӧ��Ŀ��
��  ��:
       char  type1      ����
       char  acc_type   �ʻ�
       short term1      ����
����ֵ:long item    ��Ŀ��
************************************************************/
long	get_item(type1, acc_type, term1)
char type1;
char acc_type;
short term1;
{
  MYSQL_RES *res;
  MYSQL_ROW row;
	sprintf(sql,"SELECT item_num FROM item_file WHERE ltype='%c' and acc_type='%c' and term=%d",
	  type1,acc_type,term1);
	mysql_query(conn,sql);
	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL)
	{
    return(-1);
	}
  return(atol(row[0]));
}


/*********************************************************
������:update_vou(voubranch,vou_type,vouflag)
��  ��:����ƾ֤��vouch
��  ��:voubranch ����
       vou_type  ƾ֤����
       flag 'O'���� 'o' �忪�� 'R'���� 'r'�廻��
����ֵ:0_���� 
*********************************************************/
update_vou(voubranch,vou_type,vouflag)
short voubranch;
char vou_type[];
char vouflag;
{
  long rest_que;            /* ʣ������ */
  long open_que;            /* ������ */
  long   cancel;            /* ������ */
  long chg_book;            /* ������ */
  MYSQL_RES *res;
  MYSQL_ROW row;

  sprintf(sql,"select rest_que,use_que,cancel,chg_book from vouch\
    where branch=%d and kind=90 and vou_type='%s'",voubranch,vou_type);
    
	mysql_query(conn,sql);
	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL)
	{
    return(-1);
	}
	rest_que=atol(row[0]);
	open_que=atol(row[1]);
	cancel=atol(row[2]);
	chg_book=atol(row[3]);
  mysql_free_result(res);
  
  switch(vouflag)
  {
    case 'O':
      open_que++;
      rest_que--;
      break;
    case 'o':
      open_que--;
      cancel++;
      break;
    case 'R':
      chg_book++;
      rest_que--;
      break;
    case 'r':
      chg_book--;
      cancel++;
      break;
    default:
      return(-1);
  }
  
  sprintf(sql,"update vouch set rest_que=%ld,use_que=%ld,cancel=%ld,chg_book=%ld\
    where branch=%d and kind=90 and vou_type='%s'",rest_que,open_que,cancel,chg_book,voubranch,vou_type);
    
	if(mysql_query(conn,sql))
	{
    return(-1);
	}
	
  return(0);
}

/***********************************************************
������:day_s(sd,ed)
��  ��:�����������ڼ�Ĵ�������
��  ��:short sd[3]  ��ʼԻ��
       short ed[3]  ��ֹԻ��
����ֵ:short days   ����
***********************************************************/
long day_s(sd,ed)
short sd[3],ed[3];
{
  long  days;
  if(sd[1]==31)
    sd[1]=30;	
  if(ed[1]==31)
    ed[1]=30;	
  days=(long)((ed[2]-sd[2])*360+(ed[0]-sd[0])*30+(ed[1]-sd[1]));
  return(days);
}

/*********************************************************
������:foo(term,ed)
��  ��:������Ϣ�պʹ��ڼ��㵽����
��  ��:
       short   term    ��    ��
       short   ed[3]   ��ʼԻ��
����ֵ:short   ed[3]   �� �� ��
***********************************************************/
foo(term,ed)
short   term;
short   ed[3];
{
  switch(term)
  {
    case  03: ed[0]+=3;    break;
    case  06: ed[0]+=6;    break;
    case   9: ed[0]+=9;    break;
    case  12: ed[2]+=1;    break;
    case  24: ed[2]+=2;    break;
    case  36: ed[2]+=3;    break;
    case  60: ed[2]+=5;    break;
    case  96: ed[2]+=8;    break;
    default: return(-1); break;
  }
  if(ed[0]>12)
  {
    ed[0]-=12;
    ed[2]+=1;
  }
  return(0);
}

/******************************************
������:conv_date(sd,ed,term)
��  ��:������ʼ��,��ֹ��,�����ж��Ƿ���
��  ��:
       short sd[3]  ��ʼԻ��
       short ed[3]  ֹϢԻ�� 
       short term   ��    ��
����ֵ:	1_δ����  0_�ѵ���
******************************************/
conv_date(sd,ed,term)
short sd[3], ed[3];
short term;
{
  int day_c;
  day_c=term*30;
  if(change_date(sd,ed)>=day_c)
    day_c=0;
  else
    day_c=1;
  return(day_c);
}
/****************************
������:change_date(sd,ed)
��  ��:�������� 
��  ��:short sd[3]  ��ʼԻ��
       short ed[3]  ��ֹԻ��
����ֵ:����
***************************/
change_date(sd,ed)
short sd[],ed[];
{
  short sd1,ed1,yy_flg;

  sd1=sd[1];
  ed1=ed[1];
  yy_flg=(ed[2])%4;

  if (sd[1]==31)
    sd1=30;
  if (ed[1]==31)
    ed1=30;
  if ((ed[0]==2)&&(ed[1]==28)&&(yy_flg)&&(sd[0]==2)&&(sd[1]==29))
    sd1=28;
  if ((ed[0]==2)&&((ed[1]==28)&&(yy_flg)&&(sd[0]==8)))
  {
    switch(sd[1])
    {
      case 29:
      case 30:
      case 31:
        sd1=28;
        break;
      default:
        break;
    }
  }
  if ((ed[0]==2)&&((ed[1]==29)&&(sd[0]==8)))
  {
    switch(sd[1])
    {
      case 30:
      case 31:
        sd1=29;
        break;
      default:
        break;
    }
  }
  if ((ed[0]==2)&&(ed[1]==28)&&(yy_flg)&&(sd[0]==11))
    if((sd[1]==29)||(sd[1]==30))
      sd1=28;
  if ((ed[0]==2)&&(ed[1]==29)&&(sd[0]==11)&&(sd[1]==30))
    sd1=29;
  return(((ed[2]*12+ed[0])-(sd[2]*12+sd[0]))*30+ed1-sd1);
}

/****************************************** 
������:get_lf(term,sd,ty)
��  ��:��ָ����Ϣ���ڡ����֡�����ȡ����
��  ��:
       short  term  ��    ��
       short  sd[3] ��Ϣ����
       char   ty    ��    ��
������:long   w_rt  ����   -1 ����
get_lf(term,sd,ty)
short  term;
short  sd[3];
char   ty;
{
  char   w_key1;
  short  w_key2;
  long   w_rt;
  long   edate;
  short rd[3];

  w_key1=toupper(ty);
  w_key2=term;
  $declare r_curs cursor for
    select int_date,rate from ratefile
    where ltype=$w_key1 and term=$w_key2
    order by int_date desc;
  $open r_curs;
  do
  {
    $ fetch r_curs into $edate,$w_rt;
    if(sqlca.sqlcode!=0)
    {
      sprintf(sys_err_str,"ȡ���ʴ�!code=%d.",SQLCODE);
      dis_str(22,22,sys_err_str,0,0);
      getch();
      $ close r_curs;
      $ free  r_curs;
      return(sqlca.sqlcode);
    }
    rjulmdy(edate,rd);
  } while(compday(sd,rd)==1);
  $ close r_curs;
  $ free  r_curs;
  return(w_rt);
}
*******************************************/

/********************************************************
������:s_int(mday,amt)
��  ��:���㵽��һ��7��1��Ϊֹ�Ļ�����Ϣ����
��  ��:
       short mday[3] ��ʼ����
       long amt ������
����ֵ:long interest ��Ϣ����
********************************************************/
long s_int(mday,amt)
short mday[3];
long amt;
{
  short mday1[3];
  long days,interest;
  mday1[0]=7;
  mday1[1]=1;
  note("�ж��Ƿ����!");
  if(mday[0]>=mday1[0])
    mday1[2]=mday[2]+1;
  else
    mday1[2]=mday[2];
  if(mday[1]==31)
    mday[1]=30;
  days=(mday1[2]-mday[2])*360+(mday1[0]-mday[0])*30+mday1[1]-mday[1];

  //interest=(long)(((amt/100)*(days/30.0)*(3/1000.0)+0.005)*100)*100;

  interest=(long)((amt/100)*days);

  return(interest);
}

/********************************************************
������:long s_zint(mday,amt)
��  ��:���㰴����Ϣ�ĵ���һ����ĩ XX �� 20 ��Ϊֹ����Ϣ����
��  ��:
       short mday[3] ��ʼ����
       long amt ������
����ֵ:long interest ��Ϣ����
********************************************************/
long s_zint(mday,amt)
short mday[3];
long amt;
{
    short mday1[3];
    long days,interest;
    long day1,day2;

    mday1[2]=mday[2];
    mday1[0]=mday[0]+3;
    mday1[1]=20;
    if(mday1[0]>12)
    {
      mday1[2]=mday[2]+1;
      mday1[0]=mday[0]-12;
    }
    rmdyjul(mday,&day1);
    rmdyjul(mday1,&day2);
    days=day2-day1;
    interest=(long)(((amt/100)*(days/30.0)*(3/1000.0)+0.005)*100);
    return(interest);
}

/***************************
������:compday(sd,ed)
��  ��:�Ƚ��������ڵĴ�С
��  ��:
       short  sd[3] ��ʼ����
       short  sd[3] ��ֹ����
����ֵ:
       if  sd > ed return (-1)
       if  sd = ed return (0)
       if  sd < ed return (1)
***************************/
int compday(sd,ed)
short  sd[3],ed[3];
{
  if(sd[2]>ed[2] || (sd[2]==ed[2] && sd[0]>ed[0]) ||
    (sd[2]==ed[2] && sd[0]==ed[0] && sd[1]>ed[1]))
    return(-1);
  if(sd[2]==ed[2] && sd[0]==ed[0] && sd[1]==ed[1])
    return(0);
  return(1);
}

/***************************
������:get_enddate(d, t)
��  ��:������Ϣ�պʹ��ڼ��㵽����
��  ��:
       long d  ��Ϣ��
       int t   ��  ��
����ֵ:long end_d ������
***************************/
long get_enddate(d, t)
long d;
int t;
{
  short mdy[3];
  long end_d;

  rjulmdy(d,mdy);
  mdy[2]+= t/12;
  mdy[0]+= t%12;
  if (mdy[0]>12)
  {
    mdy[2]++;
    mdy[0]-= 12;
  }
  while (rmdyjul(mdy, &end_d)== -1206)
    mdy[1]--;
  return(end_d);
}

/****************************************************
������:init_f(ff)
��  ��:��ʼ��˫���ֻ���
��  ��:struct sf_mast *ff  ˫���ֻ��ʽṹָ��
����ֵ:
****************************************************/
init_f(ff)
struct sf_mast *ff;
{
  ff->open_br=0;
  ff->account=0;
  strcpy(ff->customer," ");
  strcpy(ff->address," ");
  ff->open_d=0;	
  ff->close_d=0;
  ff->term=0;
  ff->amount=0;
  ff->interest=0;
  ff->tax_int=0;
  strcpy(ff->acc_type,"F");
  strcpy(ff->acc_flag,"A");
  ff->transfer=0;
  ff->open_op=0;
  ff->close_br=0;
  ff->close_op=0;
  ff->item_num=0;
  strcpy(ff->abstract,"0");
  strcpy(ff->withdraw,"N");
  ff->rate=0;
  return(0);
}

/****************************************************
������:init_t(tt)
��  ��:��ʼ����־
��  ��:struct s_trade *tt  ��־�ṹָ��
����ֵ:
****************************************************/
init_t(tt)
struct s_trade *tt;
{
  tt->branch=100;
  tt->operator=0;
  tt->tr_op=0;
  tt->ser_num=0;
  tt->tr_code=0;
  tt->amount=0;
  strcpy(tt->correct,"A");
  strcpy(tt->type,"S");
  tt->term=0;
  tt->open_br=100;
  tt->account=0;
  strcpy(tt->card_acc," ");
  strcpy(tt->acc_type,"F");
  strcpy(tt->customer," ");
  strcpy(tt->abstract,"A");
  tt->open_d=0;
  tt->close_d=0;
  tt->balance=0;
  tt->interest=0;
  tt->tax_int=0;
  tt->s_int=0;   
  tt->transfer=0;
  tt->det_num=0;
  tt->item_num=0;
  strcpy(tt->card_acc," ");
  return(0);
}

/****************************************************
������:disp_f(ff)
��  ��:��ʾ˫���ֻ���
��  ��:struct sf_mast *ff  ˫���ֻ��ʽṹָ��
����ֵ:
****************************************************/
disp_f(fff)
struct sf_mast *fff;
{
  clear_scr(5,22,22,77);
  mvprintw( 8, 2,"open_br =[%d]",fff->open_br);
  mvprintw( 8,32,"account =[%ld]",fff->account);
  mvprintw( 9, 2,"customer=[%s]",fff->customer);
  mvprintw( 9,32,"address =[%s]",fff->address);
  mvprintw(10, 2,"open_d  =[%d]",fff->open_d);	
  mvprintw(10,32,"close_d =[%d]",fff->close_d);
  mvprintw(11, 2,"term    =[%d]",fff->term);
  mvprintw(11,32,"amount  =[%ld]",fff->amount);
  mvprintw(12, 2,"interest=[%ld]",fff->interest);
  mvprintw(12,32,"acc_type=[%s]",fff->acc_type);
  mvprintw(13, 2,"acc_flag=[%s]",fff->acc_flag);
  mvprintw(13,32,"transfer=[%d]",fff->transfer);
  mvprintw(14, 2,"open_op =[%d]",fff->open_op);
  mvprintw(14,32,"close_br=[%d]",fff->close_br);
  mvprintw(15, 2,"close_op=[%d]",fff->close_op);
  mvprintw(15,32,"item_num=[%ld]",fff->item_num);
  mvprintw(16, 2,"abstract=[%s]",fff->abstract);
  mvprintw(16,32,"withdraw=[%s]",fff->withdraw);
  mvprintw(17,32,"rate    =[%ld]",fff->rate);
  refresh();
  getch();
  return(0);
}

/****************************************************
������:disp_acc(ff1)
��  ��:���ķ�ʽ��ʾ˫���ֻ���
��  ��:struct sf_mast *ff1  ˫���ֻ��ʽṹָ��
����ֵ:
****************************************************/
disp_acc(ff1)
struct sf_mast *ff1;
{
  short ll;
  char str[11];
  clear_scr(6,22,22,77);
 
  ll=8;
  mvprintw(ll+ 1,22,"�� �� ��=[%d]",ff1->open_br);
  mvprintw(ll+ 1,47,"��    ��=[%ld]",ff1->account);
  mvprintw(ll+ 2,22,"��    ��=[%s]",ff1->customer);
  mvprintw(ll+ 2,47,"��    ַ=[%s]",ff1->address);
  rfmtdate(ff1->open_d,"yyyy/mm/dd",str);
  mvprintw(ll+ 3,22,"�� �� ��=[%s]",str);	
  if (ff1->close_d==0)
    strcpy(str,"δ����");
  else
    rfmtdate(ff1->close_d,"yyyy/mm/dd",str);
  mvprintw(ll+ 3,47,"�� �� ��=[%s]",str);
  mvprintw(ll+ 4,22,"��    ��=[%d��]",ff1->term);
  mvprintw(ll+ 4,47,"��    ��=[%8.2lf]",ff1->amount/100.00);
  mvprintw(ll+ 5,22,"������Ϣ=[%8.2lf]",ff1->interest/100.00);
  if (ff1->acc_type[0]=='F')
    strcpy(str,"��ͨ����");
  else
    strcpy(str,"δ����");
  mvprintw(ll+ 5,47,"�ʻ�����=[%s]",str);
  if (ff1->acc_flag[0]=='A')
    strcpy(str,"������");
  else if (ff1->acc_flag[0]=='C')
    strcpy(str,"������");
  else if (ff1->acc_flag[0]=='K')
    strcpy(str,"�ѳ���");
  else if (ff1->acc_flag[0]=='B')
    strcpy(str,"�Ѳ���");
  else if (ff1->acc_flag[0]=='G')
    strcpy(str,"�ѹ�ʧ����");
  else if (ff1->acc_flag[0]=='l')
    strcpy(str,"�ѿڹ�");
  else if (ff1->acc_flag[0]=='L')
    strcpy(str,"�ѹ�ʧ");
  else if (ff1->acc_flag[0]=='F')
    strcpy(str,"�Ѷ���");
  else
    strcpy(str,"δ����");
 
  mvprintw(ll+ 6,22,"�ʻ�״̬=[%s]",str);
  mvprintw(ll+ 6,47,"�¿��ʺ�=[%d]",ff1->transfer);
  mvprintw(ll+ 7,22,"������Ա=[%d]",ff1->open_op);
  mvprintw(ll+ 7,47,"��������=[%d]",ff1->close_br);
  mvprintw(ll+ 8,22,"������Ա=[%d]",ff1->close_op);
  mvprintw(ll+ 8,47,"�� Ŀ ��=[%ld]",ff1->item_num);
  if (ff1->abstract[0]=='0')
    strcpy(str,"�ֽ�");
  else if (ff1->abstract[0]=='1')
    strcpy(str,"ת��");
  else
    strcpy(str,"δ����");
  mvprintw(ll+ 9,22,"������ʽ=[%s]",str);

  if (ff1->withdraw[0]=='Y')
    strcpy(str,"�Ѳ���");
  else
    strcpy(str,"δ����");
  mvprintw(ll+ 9,47,"�����־=[%s]",str);
  mvprintw(ll+10,22,"ת���־=[��]");
  mvprintw(ll+10,47,"�� �� ��=[%8.2lf]",ff1->rate/10000.00);
  refresh();
  return(0);
}
/***************************
������:init_tax(tax)
��  ��:��ʼ����Ϣ˰��
��  ��:struct s_tax *tax 
����ֵ:
***************************/
init_tax(tax)
struct s_tax *tax;
{
  tax->branch=0;
  tax->account=0;
  strcpy(tax->customer," ");
  tax->open_d=1;	
  tax->start_d=1;
  tax->close_d=1;
  tax->end_d=1;
  tax->term=0;
  tax->balance=0;
  tax->interest=0;
  strcpy(tax->acc_type," ");
  strcpy(tax->acc_flag," ");
  tax->item_num=0;
  tax->rate=0;
  tax->close_op=0;
  tax->tr_country=0;
  tax->taxamt=0;
  strcpy(tax->withdraw," ");
  tax->ser_num=0;
  tax->back1=0;	
  tax->back3=0;	
  strcpy(tax->back2," ");
  strcpy(tax->back4," ");
  return(0);
}


/***************************
������:disp_t(ttt)
��  ��:��ʾ��־����
��  ��:struct s_trade *ttt;
����ֵ:
***************************/
disp_t(ttt)
struct s_trade *ttt;
{
  char str[11];
  char str1[13];
  clear_scr(6,22,22,77);
  mvprintw( 8,22,"ҵ��Ա��=[%d]",ttt->tr_op);
  mvprintw( 8,50,"�� �� ��=[%d]",ttt->ser_num);
  mvprintw( 9,22,"�� �� ��");
  disp_code(9,31,ttt->tr_code);
  mvprintw( 9,50,"�� �� ��=[%d]", ttt->amount);
  if (ttt->correct[0]=='K')
    mvprintw(10,22,"״    ̬=[����]");
  else
    mvprintw(10,22,"״    ̬=[����]");
  mvprintw(10,50,"��    ��=");
  disp_type(10,59,ttt->type);
  mvprintw(11,22,"��    ��=[%d]",ttt->term);
  mvprintw(11,50,"�� Ŀ ��=[%ld]",ttt->item_num);
  mvprintw(12,22,"��    ��=[%ld]",ttt->account);
  mvprintw(12,50,"��    ��=");
  disp_acctype(12,59,ttt->acc_type);
  mvprintw(13,22,"��    ��=[%s]",ttt->customer);
  if (ttt->abstract[0]=='0')
    mvprintw(13,50,"ժ    Ҫ=[�ֽ�]");
  else
    mvprintw(13,50,"ժ    Ҫ=[ת��]");

  rfmtdate(ttt->open_d,"yyyy/mm/dd",str);
  mvprintw(14,22,"�� �� ��=[%s]",str);
  strcpy(str," ");
  rfmtdate(ttt->close_d,"yyyy/mm/dd",str);
  mvprintw(14,50,"�� �� ��=[%s]",str);
  mvprintw(15,22,"��    ��=[%ld]",ttt->balance);
  mvprintw(15,50,"��    Ϣ=[%ld]",ttt->s_int);   
  mvprintw(16,22,"�� ϸ ��=[%d]",ttt->det_num);
  refresh();
  return(0);
}

/**********************************************
������:disp_code(x,y,code) 
��  ��:��ָ��λ��(x,y)��ʾ������code�ĺ��ֽ���
��  ��:
       short x     ��  ��
       short y     ��  ��
       short code  ������
**********************************************/
disp_code(x,y,code) 
short x,y,code;
{
  switch(code)
  {
    case 1101:
      mvprintw(x,y,"[˫������]");
      break;

/**
1908   zh_pro_vou.ec    zh_pro_vou()       ����ƾ֤
1909   zh_pro_sum.ec    zh_pro_sum()       ��������
1961   zh_quer_sum.ec   zh_quer_sum()      ��ѯ����
1962   zh_quer_vou.ec   zh_quer_vou()      ��ѯƾ֤
1971   zh_cro_vou.ec    zh_cro_vou()       ����ƾ֤����
1981   zh_cro_sum.ec    zh_cro_sum()       �������ʳ���
**/

    case 1102:
      mvprintw(x,y,"[˫������]");
      break;
    case 1105:
      mvprintw(x,y,"[˫������]");
      break;
    case 1106:
      mvprintw(x,y,"[˫������]");
      break;
    case 1111:
      mvprintw(x,y,"[˫����ʧ]");
      break;
    case 1121:
      mvprintw(x,y,"[˫�����]");
      break;
    case 1161:
      mvprintw(x,y,"[˫����ѯ]");
      break;
    case 1181:
      mvprintw(x,y,"[˫������]");
      break;
    case 1501:
      mvprintw(x,y,"[���ڿ���]");
      break;
    case 1502:
      mvprintw(x,y,"[��������]");
      break;
    case 1503:
      mvprintw(x,y,"[��������]");
      break;
    case 1504:
      mvprintw(x,y,"[����֧ȡ]");
      break;
    case 1506:
      mvprintw(x,y,"[���ڲ���]");
      break;
    case 1511:
      mvprintw(x,y,"[���ڹ�ʧ]");
      break;
    case 1521:
      mvprintw(x,y,"[���ڽ��]");
      break;
    case 1561:
      mvprintw(x,y,"[���ڲ�ѯ]");
      break;
    case 1581:
      mvprintw(x,y,"[���ڳ���]");
      break;
    case 1201:
      mvprintw(x,y,"[��������]");
      break;
    case 1202:
      mvprintw(x,y,"[��������]");
      break;
    case 1203:
      mvprintw(x,y,"[��������]");
      break;
    case 1204:
      mvprintw(x,y,"[����֧ȡ]");
      break;
    case 1206:
      mvprintw(x,y,"[��������]");
      break;
    case 1211:
      mvprintw(x,y,"[������ʧ]");
      break;
    case 1221:
      mvprintw(x,y,"[�������]");
      break;
    case 1261:
      mvprintw(x,y,"[������ѯ]");
      break;
    case 1281:
      mvprintw(x,y,"[��������]");
      break;
   default:
      mvprintw(x,y,"[δ    ֪]");
      break;
  }
  refresh();
  return(0);
}
/**********************************************
������:disp_type(x,y,type)
��  ��:��ָ��λ��(x,y)��ʾ����type�ĺ��ֽ���
��  ��:
       short x     �к�
       short y     �к�
       char type[2]  ����
**********************************************/
disp_type(x,y,type) 
short x,y;
char type[2];
{
  if (type[0]=='D')
    mvaddstr(x,y,"[����]");
  else if (type[0]=='H')
    mvaddstr(x,y,"[����]");
  else if (type[0]=='P')
    mvaddstr(x,y,"[����]");
  else if (type[0]=='M')
    mvaddstr(x,y,"[�ۺ�]");
  else
    mvaddstr(x,y,type);
  refresh();
  return(0);
}

/**********************************************
������:disp_acctype(x,y,acctype)
��  ��:��ָ��λ��(x,y)��ʾ�ʻ�����acctype�ĺ��ֽ���
��  ��:
       short x     �к�
       short y     �к�
       char acctype[2]  �ʻ�����
**********************************************/
disp_acctype(x,y,acctype) 
short x,y;
char acctype[2];
{
  if (acctype[0]=='F')
    mvprintw(x,y,"[��ͨ����]  ");
  else if (acctype[0]=='P')
    mvprintw(x,y,"[���˻���]  ");
  else if (acctype[0]=='s')
    mvprintw(x,y,"[��������]  ");
  else if (acctype[0]=='L')
    mvprintw(x,y,"[��������]  ");
  else if (acctype[0]=='j')
    mvprintw(x,y,"[�� �� ��]  ");
  else
    mvprintw(x,y,"[δ    ֪]  ");
  refresh();
  return(0);
}

/***************************
������:init_s(ss)
��  ��:��ʼ�����ڷֻ���
��  ��:struct ss_mast *ss;
����ֵ:
***************************/
init_s(ss)
struct ss_mast *ss;
{
  ss->open_br=0;
  ss->account=0;
  strcpy(ss->acc_type," ");
  strcpy(ss->card_acc," ");
  strcpy(ss->card_type," ");
  strcpy(ss->customer," ");
  strcpy(ss->address," ");
  strcpy(ss->zj_kind," ");
  strcpy(ss->zj_num," ");
  ss->card_open_d=0;	
  ss->card_close_d=0;
  ss->acc_open_d=0;	
  ss->acc_close_d=0;
  ss->balance=0;
  ss->det_num=0;
  ss->interest=0;
  ss->tax_int=0;
  strcpy(ss->acc_flag," ");
  ss->transfer=0;
  ss->item_num=0;
  ss->last_day=0;	
  ss->open_op=0;
  ss->close_br=0;
  ss->close_op=0;
  return(0);
}

/***************************
������:disp_s(ss1)
��  ��:��ʾ���ڷֻ���
��  ��:struct ss_mast *ss1 
***************************/
disp_s(ss1)
struct ss_mast *ss1;
{
  clear_scr(5,22,22,77);
  mvprintw( 8, 2,"open_br =[%d]",ss1->open_br);
  mvprintw( 8,32,"account =[%ld]",ss1->account);
  mvprintw( 9, 2,"customer=[%s]",ss1->customer);
  mvprintw( 9,32,"address =[%s]",ss1->address);
  mvprintw(10, 2,"acc_open_d  =[%ld]",ss1->acc_open_d);	
  mvprintw(10,32,"acc_close_d =[%ld]",ss1->acc_close_d);
  mvprintw(11, 2,"det_num =[%d]",ss1->det_num);
  mvprintw(11,32,"balance =[%ld]",ss1->balance);
  mvprintw(12, 2,"interest=[%ld]",ss1->interest);
  mvprintw(12,32,"acc_type=[%s]",ss1->acc_type);
  mvprintw(13, 2,"acc_flag=[%s]",ss1->acc_flag);
  mvprintw(13,32,"transfer=[%d]",ss1->transfer);
  mvprintw(14, 2,"open_op =[%d]",ss1->open_op);
  mvprintw(14,32,"close_br=[%d]",ss1->close_br);
  mvprintw(15, 2,"close_op=[%d]",ss1->close_op);
  mvprintw(15,32,"item_num=[%ld]",ss1->item_num);
  mvprintw(16, 2,"card_acc=[%s]",ss1->card_acc);
  mvprintw(16,32,"card_type=[%s]",ss1->card_type);
  mvprintw(17, 2,"card_acc=[%s]",ss1->card_acc);
  mvprintw(17,32,"passwd=[%s]",ss1->passwd);
  mvprintw(18, 2,"card_open_d  =[%ld]",ss1->card_open_d);	
  mvprintw(18,32,"card_close_d =[%ld]",ss1->card_close_d);
  mvprintw(19, 2,"zj_kind=[%s]",ss1->zj_kind);
  mvprintw(19,32,"zj_num=[%s]",ss1->zj_num);
  mvprintw(20, 2,"tax_int=[%ld]",ss1->tax_int);
  mvprintw(20,32,"last_day=[%ld]",ss1->last_day);
  refresh();
  getch();
  return(0);
}

/***************************
������:disp_s_acc(ssf)
��  ��:���ķ�ʽ��ʾ���ڷֻ���
��  ��:struct ss_mast *ssf  ���ڷֻ��ʽṹָ��
����ֵ:
***************************/
disp_s_acc(ssf,iscard)
struct ss_mast *ssf;
short iscard;
{
  short ll;
  char str[11];

  clear_scr(6,22,22,77);
 
  ll=8;
  mvprintw(ll+ 1,22,"�� �� ��=[%d]",ssf->open_br);
  if(iscard){
    mvprintw(ll+ 1,47,"��    ��=[%s]",ssf->card_acc);
  }else{
    mvprintw(ll+ 1,47,"��    ��=[%ld]",ssf->account);
  }
  mvprintw(ll+ 2,22,"��    ��=[%s]",ssf->customer);
  rfmtdate(ssf->last_day,"yyyy/mm/dd",str);
  mvprintw(ll+ 2,47,"�� �� ��=[%s]",str);
  rfmtdate(ssf->acc_open_d,"yyyy/mm/dd",str);
  mvprintw(ll+ 3,22,"��    ��=[%8.2lf]",ssf->balance/100.00);
  mvprintw(ll+ 3,47,"��ϸ����=[%d]",ssf->det_num);
  mvprintw(ll+ 4,22,"�� �� ��=[%s]",str);	
  mvprintw(ll+ 4,47,"������Ա=[%d]",ssf->open_op);
  /*
  if (ssf->acc_close_d==0)
    strcpy(str,"δ����");
  else
    rfmtdate(ssf->card_close_d,"yyyy/mm/dd",str);
  if (ssf->acc_close_d==0)
    strcpy(str,"δ����");
  else
    rfmtdate(ssf->card_close_d,"yyyy/mm/dd",str);
  mvprintw(ll+ 3,47,"�� �� ��=[%s]",str);
  */
  mvprintw(ll+ 5,22,"��    ַ=[%s]",ssf->address);
  /*
  if (ssf->acc_flag[0]!='C')
    mvprintw(ll+ 5,22,"��Ϣ����=[%8.2lf]",ssf->interest/100.00);
  else
    mvprintw(ll+ 5,22,"������Ϣ=[%8.2lf]",ssf->interest/100.00);
  */
  if (ssf->acc_flag[0]=='A')
    strcpy(str,"������");
  else if (ssf->acc_flag[0]=='C')
    strcpy(str,"������");
  else if (ssf->acc_flag[0]=='K')
    strcpy(str,"�ѳ���");
  else if (ssf->acc_flag[0]=='B')
    strcpy(str,"�Ѳ���");
  else if (ssf->acc_flag[0]=='G')
    strcpy(str,"�ѹ�ʧ����");
  else if (ssf->acc_flag[0]=='l')
    strcpy(str,"�ѿڹ�");
  else if (ssf->acc_flag[0]=='L')
    strcpy(str,"�ѹ�ʧ");
  else if (ssf->acc_flag[0]=='F')
    strcpy(str,"�Ѷ���");
  else
    strcpy(str,"δ����");
  mvprintw(ll+ 5,22,"�ʻ�״̬=[%s]",str);
  if (ssf->acc_type[0]=='P')
    strcpy(str,"���˻���");
  else if (ssf->acc_type[0]=='K')
    strcpy(str,"��ǿ�");
  else if (ssf->acc_type[0]=='G')
    strcpy(str,"��������");
  else
    strcpy(str,"δ����");
  mvprintw(ll+ 5,47,"�ʻ�����=[%s]",str);
  if (ssf->zj_kind[0]=='0')
    strcpy(str,"���֤");
  else if (ssf->zj_kind[0]=='1')
    strcpy(str,"�绰");
  else if (ssf->zj_kind[0]=='2')
    strcpy(str,"����");
  else if (ssf->zj_kind[0]=='3')
    strcpy(str,"��ʱ֤");
  else if (ssf->zj_kind[0]=='4')
    strcpy(str,"����");
  mvprintw(ll+ 6,22,"֤������=[%s]",str);
  mvprintw(ll+ 6,47,"֤������=[%s]",ssf->zj_num);
  //mvprintw(ll+ 6,47,"�¿��ʺ�=[%d]",ssf->transfer);
  //mvprintw(ll+ 7,47,"��������=[%d]",ssf->close_br);
  //mvprintw(ll+ 8,22,"������Ա=[%d]",ssf->close_op);
  //mvprintw(ll+ 8,47,"�� Ŀ ��=[%ld]",ssf->item_num);
  refresh();
  return(0);
}

/***************************
������:get_month(mm)
��  ��:�����·ݼ�д�ַ����õ��·���
��  ��:
       char mm[4] �·��ַ���
����ֵ:�·���
***************************/
get_month(mm)
char mm[4];
{
    if (strcmp(mm, "Jan")==0) return(1);
    else if (strcmp(mm, "Feb")==0) return(2);
    else if (strcmp(mm, "Mar")==0) return(3);
    else if (strcmp(mm, "Apr")==0) return(4);
    else if (strcmp(mm, "May")==0) return(5);
    else if (strcmp(mm, "Jun")==0) return(6);
    else if (strcmp(mm, "Jul")==0) return(7);
    else if (strcmp(mm, "Aug")==0) return(8);
    else if (strcmp(mm, "Sep")==0) return(9);
    else if (strcmp(mm, "Oct")==0) return(10);
    else if (strcmp(mm, "Nov")==0) return(11);
    else if (strcmp(mm, "Dec")==0) return(12);
    else  return(0);
}

/***************************
������:found_char(str)
��  ��:��str�еĻ��з�\n asciiֵΪ10��Ϊ�ո�,
       ���⴮�г����������з�
��  ��:
       char str[23]  �账����ַ���
����ֵ:
***************************/
found_char(str)
char str[23];
{
  short l,i;
  l=strlen(str);
  for(i=0;i<=l;i++)
  {
    if (str[i]==10)
    {
      str[i]=' ';
      break;
    }
  }
  return(0);
}


/***************************************
������:found(type,term,item)
��  ��:��������Ŀ�Ķ�ת��Ϣ��Ŀ
��  ��:
       char  type[2];����
       long  item;   ��Ŀ��
       short term;   ����
***************************************/
found(type,term,item)
char type[2];
long item;
short term;
{
  long int_item;
  short ii;

  note("���ݴ��ֺʹ��ھ�����Ŀ");

  switch(type[0])
  {
    case 'H':
      note("����"); 

      int_item=521100;
      break;
    case 'D':
      note("����"); 

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

/***************************
������:init_zh(sum)
��  ��:��ʼ�����ʽṹ
��  ��:struct s_sumtable *sum;
����ֵ:
***************************/
init_zh(sum)
struct f_sumtable *sum;
{
  sum->branch=0;
  sum->kind=0;
  sum->item_num=0;
  strcpy(sum->flag,"A");
  sum->det_amt=0;
  sum->cre_amt=0;
  sum->open_que=0;
  sum->cls_que=0;
  sum->dvou_que=0;
  sum->cvou_que=0;
  sum->balance=0;
  sum->rest_que=0;
  return(0);
}

/***************************
������:disp_sumtable(sum)
��  ��:��ʾ����
��  ��:struct s_sum1table *sum1;
***************************/
disp_sumtable(sum1)
struct f_sumtable *sum1;
{
  short ll;
  char str[11];

  clear_scr(6,22,22,77);
  ll=7;
  mvprintw(ll+ 1,20,"��      ��=[%3d]",sum1->branch);
  mvprintw(ll+ 2,20,"��      ־=[%3d]",sum1->kind);
  mvprintw(ll+ 3,20,"��  Ŀ  ��=[%6ld]",sum1->item_num);
  mvprintw(ll+ 4,20,"״      ̬=[%1s]",sum1->flag);
  mvprintw(ll+ 5,20,"����������=[%10.2lf]",sum1->det_amt);
  mvprintw(ll+ 6,20,"�跽������=[%10.2lf]",sum1->cre_amt);
  mvprintw(ll+ 7,20,"��  ��  ��=[%4d]",sum1->open_que);
  mvprintw(ll+ 8,20,"��  ��  ��=[%4d]",sum1->cls_que);
  mvprintw(ll+ 9,20,"����ҵ����=[%4d]",sum1->dvou_que);
  mvprintw(ll+10,20,"�跽ҵ����=[%4d]",sum1->cvou_que);
  mvprintw(ll+11,20,"��      ��=[%10.2lf]",sum1->balance);
  mvprintw(ll+12,20,"��      ��=[%4d]",sum1->rest_que);
  return(0);
}

/***************************
������:disp_vouch(vou)
��  ��:��ʾƾ֤
��  ��:struct f_vouch *vou;
***************************/
disp_vouch(vou)
struct f_vouch *vou;
{
  short ll;
  char str[11];

  clear_scr(6,22,22,77);
  ll=8;
  mvprintw(ll+ 2,32,"ƾ֤����=[%2s]",vou->vou_type);
  mvprintw(ll+ 3,32,"��    ־=[%3d]",vou->kind);
  mvprintw(ll+ 4,32,"״    ̬=[%1s]",vou->flag);
  mvprintw(ll+ 5,32,"ʣ �� ��=[%4d]",vou->rest_que);
  mvprintw(ll+ 6,32,"ʹ �� ��=[%4d]",vou->use_que);
  mvprintw(ll+ 7,32,"�� �� ��=[%4d]",vou->cancel);
  mvprintw(ll+ 8,32,"�� �� ��=[%4d]",vou->chg_book);
  mvprintw(ll+ 9,32,"�� �� ��=[%4d]",vou->takeback);
  return(0);
}

/*********************************************
������:get_i_item(item)
��  ��:���ݿ�Ŀ�ŵõ���Ӧ�����±�
��  ��:
       long item; ��Ŀ��
get_i_item(item)
long item;
{
   short i,nn;
short item_count;
struct to_sum sum_i[100];
  struct f_item  it;
    note("��item_file�ж���Ŀ�ż������Ϣ!");
    nn=0;
    $declare it_cur cursor for select * from item_file
      where item_flag!="K" ;
    if(sqlca.sqlcode!=0)
    {
      sprintf(sys_err_str,"����Ŀ�α��!code=%ld.",SQLCODE);
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(0);
    }
    $open it_cur;
    if(sqlca.sqlcode!=0)
    {
      sprintf(sys_err_str,"�򿪿�Ŀ�α��!code=%ld.",SQLCODE);
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(0);
    }
    for(;;)
    {
      $fetch it_cur into $it;
      if(SQLCODE!=0)
        break;
      nn++;
      sum_i[nn].item_num=it.item_num;
      strcpy(sum_i[nn].item_name,it.item_name);
      sum_i[nn].sum_flag=it.sum_flag;
      sum_i[nn].prt_ser=it.prt_ser;
      sum_i[nn].j_x_num=0;
      sum_i[nn].j_x_amt=0;
      sum_i[nn].j_z_num=0;
      sum_i[nn].j_z_amt=0;
      sum_i[nn].d_x_num=0;
      sum_i[nn].d_x_amt=0;
      sum_i[nn].d_z_num=0;
      sum_i[nn].d_z_amt=0;
      sum_i[nn].open_num=0;
      sum_i[nn].close_num=0;
      sum_i[nn].yes_bal=0;
      sum_i[nn].today_bal=0;
    }
    $close it_cur;
    $free it_cur;
    item_count=nn;
   
   for (i=1;i<=item_count;i++)
   {
     if (sum_i[i].item_num==item)
       break;
   }
   if (i>item_count)
     return(-1);
   else
     return(i);
}
**********************************************/
