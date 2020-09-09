/****************************************************************                                           
  功  能:备份到备机
****************************************************************/              
#include<stdio.h>
#include<curses.h>
extern long sys_today;
rbk()
{
  char kk,_Inform[200];
  char str_date[9];
  extern char dbpass[];
  char dbpass2[10];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《异机备份》");  
  refresh();

  dis_str(22,18,"要求先执行硬盘备份。回车开始异机备份,按 q 键退出!",0,0);
  kk=getch();
  if (kk=='q' || kk=='Q')
    return(0);
  keypad(stdscr,FALSE);echo();noraw();endwin();
  system("clear");
  system("echo;echo;echo 正在进行,请稍侯...");

  //输入备份日期
  rfmtdate(sys_today, "yyyymmdd",str_date);
  cry_num_r(dbpass,dbpass2);
  
  sprintf(_Inform,"./rbk.sh %s",str_date); 
  if(system(_Inform) != 0)
  {
    system("echo;echo;echo 异机备份出错!");
    getch();
    system("stty sane");
    initscr();raw();noecho();keypad(stdscr,TRUE); 
    main_menu(7);
    return(-1);
  }

  system("echo;echo;echo 异机备份完毕,按任一键返回.");
  getch();
  system("stty sane");
  initscr();raw();noecho();keypad(stdscr,TRUE);
	
  main_menu(7);
  return(1);
}
