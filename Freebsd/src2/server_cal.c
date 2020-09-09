
/****************************************************************
������:server_cal.c
������:server_c()
��  ��:��ʾ����
��  ��:1712
��  ��:
      1.����:��/��
      2.��cal yyyy>aa
        cut -c1-20 aa>aa1
        cut -c24-44 aa>aa1
        cut -c47-67 aa>aa1
        cat aa1+aa2+aa3>aa0
      3.��aa0��ȡ�·�����
        ���ζ�û���ַ���
        �жϵ�9-11���Ƿ�Ϊ�ַ���(�ɱȽϵھ�λ�Ƿ�Ϊ��ĸ)
        ת��Ϊ�·ݺ���
˵  ��:���з�Ϊascii 10������Ϊascii 0 1Ϊascii49
 ****************************************************************/

#include<stdio.h>
#include<curses.h>
#include  <string.h>
#include  <sys/types.h>
#include <time.h>
#include "../include/sh_table.h"

extern long sys_today;

server_cal()
{
  static short in[2]={25,8};
  short num=2,flag;
  char tmp_str[100];
  char kk,str[25],mm[4],yy[5],*monstr;
  FILE *fp,*fopen();
  struct in_data da;
  short month,mon,cow;

  struct tm *stm;
  time_t tt=sys_today;
  stm=localtime(&tt);
  month=stm->tm_mon+1;
  da.year=stm->tm_year+1900;
	
  clear_scr(5,22,22,77);
  mvaddstr(5,22,"����ʾ������");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
    return(0);
  clear_scr(6,22,22,77);
  dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  
  sprintf(tmp_str,"cal %d >/tmp/tmp_txt 2>/dev/null",da.year);
  system(tmp_str);
  //system(" cut -c1-20 /tmp/tmp_txt >/tmp/tmp_txt1 2>/dev/null");
  //system(" cut -c24-44 /tmp/tmp_txt >>/tmp/tmp_txt1 2>/dev/null");
  //system(" cut -c47-67 /tmp/tmp_txt >>/tmp/tmp_txt1 2>/dev/null");
  //FreeBSD�����ո�
  system(" cut -c1-20 /tmp/tmp_txt >/tmp/tmp_txt1 2>/dev/null");
  system(" cut -c23-42 /tmp/tmp_txt >>/tmp/tmp_txt1 2>/dev/null");
  system(" cut -c45-64 /tmp/tmp_txt >>/tmp/tmp_txt1 2>/dev/null");
  system(" grep -v Su /tmp/tmp_txt1 >/tmp/tmp_txt 2>/dev/null");

  fp=fopen("/tmp/tmp_txt","r");
  if(fp==NULL)
  {
    dis_str(22,22,"open /tmp/tmp_txt1 err!",0,0);  
    getch();
    return(-1);
  }
  for (;;month=((flag==3)? (month==1? 12:month-1):(month==12? 1:month+1)))
  {
    note("�����ļ���ʼ");
    rewind(fp);
    clear_scr(10,22,22,77);
    mvprintw(9,38,"<%d��%d��>",da.year,month);
    mvprintw(11,35,"�� һ �� �� �� �� ��");
    refresh();
    cow=13;
    flag=0;

    while(1)
    {
    	if(fgets(str,23,fp)==NULL){
        if (flag==1)
        {
          dis_str(22,22,"��p ��ʾ��һ�£���������ʾ��һ��, q �˳�!",0,0); 
          kk=getch();
          if (kk=='p'||kk=='P')flag=3;
          if (kk=='q'||kk=='Q')flag=2;
        }
        break;
    	}
      note("������Ϊ�����򲻴���");
      if (strlen(str)==1)
        continue;
      //monstr�ҵ��·ݿ�ʼλ��
      for(monstr=str;*monstr==' ';monstr++);
      if(isalpha(monstr[0])>0 )
      {
        //note("���·�,��ȡ9-11λ�ж��Ƿ�Ϊ����ʾ��");
        if (flag==1)
        {
          dis_str(22,22,"��p ��ʾ��һ�£���������ʾ��һ��, q �˳�!",0,0);  
          kk=getch();
          if (kk=='p'||kk=='P')flag=3;
          if (kk=='q'||kk=='Q')flag=2;
          break;
        }
        strncpy(mm,monstr,3);
        mm[3]='\0';
        mon=get_month(mm);
        if (mon==month)
          flag=1;
      }

      if (flag==1)
      {
        note("��ʾĳ������");
        //note("������Ϊ���YYYY��Ϊ�������򲻴���(9-12�е����������)");
        if (atoi(str)==0 || atoi(str)==da.year)
          continue;
        found_char(str);
        str[20]='\0';
        mvprintw(cow,35,"%s",str);
        refresh();
        cow++;
      }
    }
    if (flag==2)
      break;
  }
  close(fp);
  return(0);
}
