/****************************************************************
  程序名:tty_kill.c                                          
  函数名:tty_kill()                                         
  功  能:杀死某活动终端
****************************************************************/
#include<stdio.h>
#include<string.h>
#include<curses.h>
#include<stdlib.h>
#include "../include/apt.h"
tty_kill()
{
  FILE *fp;
  short pid,i,j,flag,n;
  char tmp_str[100],tmp_tty[3];
  char tty_str[40],pid_str[7],sub_str[7];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《杀死活动终端》");  
  refresh();
  strcpy(tmp_tty," ");
  move(10,25); addstr("请输入终端号:[   ]");
  refresh();
  dis_str(22,22,"若终端号为 ttyd3 则输入 d3,(99退出) !",0,0); 
  acc_str(10,39,&tmp_tty,3);
  if(atoi(tmp_tty)==99)
    return(0);
  n=strlen(tmp_tty);
  system("who|sed 's/       tty/tty/' >./tmp/tmp_tty 2>/dev/null");
  fp=fopen("./tmp/tmp_tty","r");
  if(fp==(FILE *)NULL)
  {
    dis_err("临时文件tmp/tmp_tty打开错!");
    return(1);
  }
  flag=0;
  while( !feof(fp) )
  {
    fgets(tty_str,40, fp);
    strncpy(sub_str,&tty_str[13],n);//freebsd,20
	  sub_str[n]='\0';
    if (strcmp(tmp_tty,sub_str)==0)
    {
      flag=1;
      break;
    }
  }
  fclose(fp);
  if (flag==0)
  {
    dis_err("无指定的活动终端!");
    return(1);
  }
  sprintf(tmp_str,"ps -t tty%s >./tmp/tmp_tty 2>/dev/null",tmp_tty);
  system(tmp_str);
  fp=fopen("./tmp/tmp_tty","r");
  if(fp==(FILE *)NULL)
  {
    dis_err("无指定的活动终端!");
    return(1);
  }
  while( !feof(fp) )
  {
    fgets(tty_str,40, fp);
    strncpy(pid_str,tty_str,6);
    pid=atoi(pid_str);
    if (pid==0)
      continue;
    sprintf(tmp_str," kill -9 %4d 1>/dev/null 2>/dev/null",pid);
    system(tmp_str);
  }
  fclose(fp);

	clear_l22();
  dis_err("指定终端已杀死,按任一键返回!"); 
  return(0);
}
