/****************************************************************
������:query_chk.c
������:query_chk()
��  ��:��ѯУ��λ
��  ��:1711
��  ��:  
      1.����
        �ʺ� (account)
      2.���������ʺż�����Ӧ��У��λ
 ****************************************************************/

#include <stdio.h>
#include "../include/sh_table.h"
extern short sys_branch;

query_chk()
{
  struct in_data da;
  static short in[2]={6,8};
  short num=2,flag;
  char yy;

  short ii,jj,kk,jj1;
  short br;
  long acc;
  long loc_no;
  char pan1[18];

  init_in(&da);
  clear_scr(5,22,22,77);
  da.open_br=sys_branch;
    mvaddstr(5,30,"����ѯ�ʺŵ�У��λ��");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    br=da.open_br;
    acc=da.que_acc;
    loc_no=471;
    sprintf(pan1,"%02d%04d%03d%08ld",62,loc_no,br,acc);  
    jj=0;
    jj1=0;
    for(ii=16;ii>-1;ii--)
    {
      if(ii%2!=0)
        jj+=(pan1[ii]-'0');
      else 
      {
        jj1 = (pan1[ii]-'0')*2;
        if(jj1>=10)
          jj1-= 9;
        jj += jj1;
      }
    }
    if(jj%10==0)
      kk=0;
    else
      kk=10 - jj%10;
    mvprintw(9,30,"У �� λ:[%1d]",kk);
	refresh();
    dis_err("�������!");  
  return(0);
}

