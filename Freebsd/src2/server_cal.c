
/****************************************************************
程序名:server_cal.c
函数名:server_c()
功  能:显示日历
代  码:1712
流  程:
      1.输入:年/月
      2.将cal yyyy>aa
        cut -c1-20 aa>aa1
        cut -c24-44 aa>aa1
        cut -c47-67 aa>aa1
        cat aa1+aa2+aa3>aa0
      3.从aa0中取月份内容
        依次读没行字符串
        判断第9-11串是否为字符串(可比较第九位是否为字母)
        转化为月份后处理
说  明:换行符为ascii 10结束符为ascii 0 1为ascii49
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
  mvaddstr(5,22,"《显示日历》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
    return(0);
  clear_scr(6,22,22,77);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  
  sprintf(tmp_str,"cal %d >/tmp/tmp_txt 2>/dev/null",da.year);
  system(tmp_str);
  //system(" cut -c1-20 /tmp/tmp_txt >/tmp/tmp_txt1 2>/dev/null");
  //system(" cut -c24-44 /tmp/tmp_txt >>/tmp/tmp_txt1 2>/dev/null");
  //system(" cut -c47-67 /tmp/tmp_txt >>/tmp/tmp_txt1 2>/dev/null");
  //FreeBSD，两空格
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
    note("返回文件开始");
    rewind(fp);
    clear_scr(10,22,22,77);
    mvprintw(9,38,"<%d年%d月>",da.year,month);
    mvprintw(11,35,"日 一 二 三 四 五 六");
    refresh();
    cow=13;
    flag=0;

    while(1)
    {
    	if(fgets(str,23,fp)==NULL){
        if (flag==1)
        {
          dis_str(22,22,"按p 显示上一月，其他键显示下一月, q 退出!",0,0); 
          kk=getch();
          if (kk=='p'||kk=='P')flag=3;
          if (kk=='q'||kk=='Q')flag=2;
        }
        break;
    	}
      note("若本行为空行则不处理");
      if (strlen(str)==1)
        continue;
      //monstr找到月份开始位置
      for(monstr=str;*monstr==' ';monstr++);
      if(isalpha(monstr[0])>0 )
      {
        //note("是月份,再取9-11位判断是否为需显示月");
        if (flag==1)
        {
          dis_str(22,22,"按p 显示上一月，其他键显示下一月, q 退出!",0,0);  
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
        note("显示某月数据");
        //note("若本行为年份YYYY或为星期行则不处理(9-12列等于输入年份)");
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
