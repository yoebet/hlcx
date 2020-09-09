
/****************************************************************
  程序名:pid_disp.c                                          
  函数名:pid_disp()                                         
  功  能:显示活动进程
****************************************************************/
#include<stdio.h>
#include<curses.h>
pid_disp()
{
  FILE *fp;
  short i;
  char pid_str[40];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《显示活动进程》");  
  refresh();
  system("ps -u hlcx >./tmp/tmp_pid 2>/dev/null");
  fp=fopen("./tmp/tmp_pid","r");
  if(fp==(FILE *)NULL)
  {
    dis_err("无活动进程!");
    return(1);
  }
  mvaddstr( 8,25,"  进程  终端     时间      命令");
  mvaddstr( 9,25,"======================================");
  refresh();
  i=0;
  while( !feof(fp) )
  {
    fgets(pid_str,40, fp);
    mvaddstr(i+10,25,pid_str);
    refresh();
    i++;
    if (i==6 && !feof(fp) )
    {
      dis_err("按任一键继续!"); 
      clear_scr(10,22,22,77);
  mvaddstr(5,30,"《显示活动进程》");  
  mvaddstr( 8,25,"  进程  终端     时间      命令");
  mvaddstr( 9,25,"======================================");
  refresh();
      i=0;
    }  
  }
  fclose(fp);
  dis_err("活动终端显示完毕,按任一键返回!"); 
  return(0);
}
