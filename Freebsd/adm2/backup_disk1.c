/****************************************************************
  程序名:backup_disk1.ec                                             
  函数名:backup_disk1()                                              
  功  能:备份硬盘
  代  码:2708  
  流  程:
****************************************************************/              
#include<stdio.h>
#include<curses.h>
extern long sys_today;
backup_disk1()
{
  char kk,_Inform[200];
  char str_date[9];
  //static int bknum=0;
  extern char dbpass[];
  char dbpass2[10];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《硬盘备份》");  
  refresh();

  dis_str(22,22,"回车开始备份,按 q 键退出!",0,0);
  kk=getch();
  if (kk=='q' || kk=='Q')
    return(0);
  keypad(stdscr,FALSE);echo();noraw();endwin();
  system("clear");
  system("echo;echo;echo 正在备份,请稍侯...");

  //输入备份日期
  rfmtdate(sys_today, "yyyymmdd",str_date);
  cry_num_r(dbpass,dbpass2);
  
  //sprintf(_Inform,"mysqldump -uhlcx -p%s hlcxdb --add-drop-table |\
  //   gzip -c - > ./backup/hlcxdb-%s.gz",dbpass2,str_date); 
  sprintf(_Inform,"./bk.sh %s %s",dbpass2,str_date); 
  if(system(_Inform) != 0)
  {
    system("echo;echo;echo 备份出错!");
    getch();
    system("stty sane");
    initscr();raw();noecho();keypad(stdscr,TRUE); 
    main_menu(7);
    return(-1);
  }

  system("echo;echo;echo 备份完毕,按任一键返回.");
  getch();
  system("stty sane");
  initscr();raw();noecho();keypad(stdscr,TRUE); 
  
  //恢复：gzip -d ./backup/hlcxdb-%s.gz; mysql -uhlcx -pxxxx hlcxdb < ./backup/hlcxdb-%s
	
  main_menu(7);
  return(1);
}
