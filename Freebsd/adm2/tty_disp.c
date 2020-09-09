
/****************************************************************
  程序名:tty_disp.c  
  函数名:tty_disp() 
  功  能:显示活动终端
****************************************************************/
#include<stdio.h>
#include<curses.h>
tty_disp()
{
  FILE *fp;
  short i;
  char tty_str[60];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《显示活动终端》");  
  refresh();
  system("who|sed 's/       tty/tty/' >./tmp/tmp_tty 2>./tmp/tmp_core");
  fp=fopen("./tmp/tmp_tty","r");
  if(fp==(FILE *)NULL)
  {
    dis_err("无指定的活动终端!");
    return(1);
  }
  mvaddstr( 8,15,"  登录用户  登录终端        登录时间");
  mvaddstr( 9,15,"===========================================");
  refresh();
  i=0;
  while( !feof(fp) )
  {
    fgets(tty_str,60, fp);
    mvaddstr(i+10,18,tty_str);
    refresh();
    i++;
    if (i==9 && !feof(fp) )
    {
      mvprintw(22,22,"按任一键继续!");
      i=0;
		  clear_scr(5,22,22,77);
		  mvaddstr(5,35,"《显示活动终端》");  
		  mvaddstr( 8,15,"  登录用户  登录终端        登录时间");
		  mvaddstr( 9,15,"===========================================");
		  refresh();
    }  
  }
  fclose(fp);
  mvprintw(22,22,"显示完毕,按任一键返回!");
    refresh();
  getchar();
  
  return(0);
}
