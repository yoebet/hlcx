/**************************************************************
  程序名:op_main.c 
  函数名:main()    
**************************************************************/              
#include<stdio.h>
#include<string.h>
#include<curses.h>
#include<ctype.h>
#include<time.h>
#include<signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/apt.h"
#include "../include/sh_table.h"
#include "mysql.h"

char sys_err_str[1024];
char sys_op_name[9],sys_op_level[2];
char in_flag;

MYSQL *conn=NULL;
char sql[1024];

char database[7]="hlcxdb";
char dbserv[16]="localhost";
char dbuser[10]="h";
char dbpass[10]="0";

short sys_branch,sys_operator,sys_code,sys_sernum;    
long sys_today,sys_work_d,sys_loc_no;
struct in_data da;


MYSQL *connectdb(MYSQL *conn){

  char dbpass2[10];
  cry_num_r(dbpass,dbpass2);
  mysql_real_connect(conn,dbserv,dbuser,dbpass2,database,0,NULL,0);
  
  if(mysql_query(conn,"set names gb2312")){
    fprintf(stderr,"%s\n",mysql_error(conn));
    return(0);
  }
  return conn;
}

main() 
{
  struct f_operator op;
  struct f_branch br;

  short op_br,op_no;
  char pwdsi[9],pwdli[7],pwdso[9];
  char ttyno[7],op_d[11];
  char *tty_name,yn[2],tmp_ymd[11];
  long sys_d,tmp_var;
  short ymd[3],flag,kk,l;
  long pid;
  char yy;
  char scname[12],scvalue[20];
  FILE *scfp;
  
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  conn=mysql_init(NULL);

  system("stty sane");
  signal(SIGINT,SIG_IGN);
  initscr();raw();noecho();keypad(stdscr,TRUE); 
  rtoday(&sys_today);
  
  scfp=fopen("./sc","r");
  if(scfp==(FILE *)NULL)
  {
    dis_str(22,22,"读配置错!",0,0);
    keypad(stdscr,FALSE);echo();noraw();endwin();
    return(1);
  }
  while(fscanf(scfp,"%6s=%s\n",&scname,&scvalue)==2)
  {
  	if(strcmp(scname,"dbserv")==0){
  		strcpy(dbserv,scvalue);
  	}else if(strcmp(scname,"dbuser")==0){
  		strcpy(dbuser,scvalue);
  	}else if(strcmp(scname,"dbpass")==0){
  		strcpy(dbpass,scvalue);
  	}
  }
  fclose(scfp);
  
  if(!connectdb(conn)){
    sprintf(sys_err_str,"打开库错!%s",mysql_error(conn));
    dis_str(22,21,sys_err_str,60,0);
    keypad(stdscr,FALSE);echo();noraw();endwin();
    return(1);
  }
  
	op_br=100;
  while(1)
  {
    clear_scr(0,24,0,79);
    mvaddstr(6,15,"快乐星期银行柜台业务系统签到画面");
    mvaddstr(11,24,"操作员号:[  ]");
    mvaddstr(12,24,"密    码:[      ]");
    mvaddstr(13,24,"业务日期:[          ]");
    mvaddstr(15,24,"确    认:[ ]");
    refresh();
    kk=1;
    while(kk!=5&&kk!=0)/** 柜员签到 **/
    {
      tmp_var=0;
      switch(kk)
      {
   	case 1:
          mvaddstr(11,24,"操作员号:[  ]");
          dis_str(21,18,"请输入操作员号(99退出).",0,0); 
          while((flag=acc_long(11,34,&tmp_var,2))==Err);
          if(flag==KEY_F(1)||flag==ESC)
            break;
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          op_no=(short)tmp_var;
          if (op_no==99)
          {
            keypad(stdscr,FALSE);echo();noraw();endwin();
            system("clear");
            exit(0);
          }
          if (op_no==0)
          {
            dis_str(22,18,"输入操作员号错!",0,0); 
            getch();
            break;
          }
          sys_operator=op_no;
          kk++;
          break;
   	case 2:
          mvaddstr(12,24,"密    码:[      ]");
          dis_str(21,18,"请输入操作员密码.",0,0); 
          pwdli[0]='\0';
          while((flag=acc_pwd(12,34,pwdli,6))==Err);
          if(flag==KEY_F(1)||flag==ESC)
            break;
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(pwdli[0]=='\0') 
            break;
          kk++;
          break;
        case 3:
          mvaddstr(13,24,"业务日期:[          ]");
          dis_str(21,18,"请输入业务日期(年年年年/月月/日日).",0,0); 
          op_d[0]=0;
          while((flag=acc_date(13,34,op_d,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
            break;
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(op_d[0]==0)
            break;
          kk++;
          break;
        case 4:
          mvaddstr(15,24,"确    认:[ ]");
          dis_str(21,18,"请输入 y 或 n.",0,0); 
          yn[0]=0;
          while((flag=acc_str(15,34,yn,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
            break;
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(yn[0]==0)
            break;
          if(yn[0]=='y'||yn[0]=='Y')
            kk++;
          else
          {
            mvaddstr(11,24,"操作员号:[  ]");
            mvaddstr(12,24,"密    码:[      ]");
            mvaddstr(13,24,"业务日期:[          ]");
            mvaddstr(15,24,"确    认:[ ]");
            kk=1;
          }
      }
      if(flag==KEY_F(1)||flag==ESC)
      {
        keypad(stdscr,FALSE);echo();noraw();endwin();
        system("clear");
        exit(0);
      }
    }
    if(kk!=5)
      continue;

    note("检查所号合法性");
    
    
		sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",op_br);
		
  	if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"select branch err,%s",mysql_error(conn));
      dis_str(22,18,sys_err_str,60,0);
      getch();
      continue;
	  }
		
		res=mysql_store_result(conn);
    row=mysql_fetch_row(res);

		if(row==NULL)
		{
      dis_str(22,10,"指定所不存在!",0,0);  
      getch();
      continue;
		}
		else {
		  set_f_branch(&br,row);
		}
		mysql_free_result(res);
    
    if(br.br_flag[0]!='A') 
    {
      dis_str(22,18,"本所已作废!",0,0); 
      getch();
      continue;
    }
    if(br.init_flag==1) 
    {
      dis_str(22,18,"本日已初始化!",0,0); 
      getch();
      continue;
    }
    rjulmdy(br.work_d,ymd); 
    sprintf(tmp_ymd,"%04d/%02d/%02d",ymd[2],ymd[0],ymd[1]);
    if(strcmp(tmp_ymd,op_d)!=0) 
    {
      dis_str(21,18,"输入业务日与本所工作日不符!",0,0); 
      sprintf(sys_err_str,"现工作日为:%04d年%02d月%02d日",ymd[2],ymd[0],ymd[1]);
      dis_str(22,18,sys_err_str,0,0);
      getch();
      continue;
    }
    sys_work_d=br.work_d;

    note("检查操作员合法性");
    
    
	  sprintf(sql,"SELECT * FROM operfile WHERE  operator= %d ",op_no);
  	
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      mysql_close(conn);//程序的其他地方不要关闭连接
      return(-2);
    }
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
	    set_f_operator(&op,row);
    }

    mysql_free_result(res);
  
    if(row==NULL) 
    {
      dis_str(22,10,"指定操作员不存在!",0,0);  
      getch();
      continue;
    }
    if(mysql_errno(conn)!=0) 
    {
      sprintf(sys_err_str,"操作员不存在");
      dis_str(22,18,sys_err_str,60,0);
      getch();
      continue;
    }
    if(op.op_flag[0]!='A') 
    {
      dis_str(22,18,"本操作员已作废!",0,0); 
      getch();
      continue;
    }
    if(op.op_level[0]!='1' &&  op.op_level[0]!='4') 
    {
      dis_str(22,18,"本操作员不能办理业务!",0,0); 
      getch();
      continue;
    }
    cry_pwd(pwdli,pwdsi,op_no); 
    if(strncmp(op.password,pwdsi,8)!=0) 
    {
      dis_str(22,18,"密码错误!",0,0); 
      getch();
      continue;
    }
    
    note("更新本操作员的有关内容"); 
    pid=getpid();
    tty_name = ttyname(1);
    l=strlen(tty_name);
    //strncpy(ttyno,&tty_name[5],l-5);
    strncpy(ttyno,&tty_name[5],sizeof(ttyno)-1);
	  ttyno[sizeof(ttyno)-1]='\0';
    op_no=sys_operator;
    
	
	  sprintf(sql,"UPDATE operfile SET tty_no = '%s',tty_pid= %d WHERE operator= %d ",ttyno,pid,op_no);
	
    if(mysql_query(conn,sql)||mysql_errno(conn)){
      sprintf(sys_err_str,"修改操作员错!%s",mysql_error(conn));
      dis_str(22,18,sys_err_str,60,0);
      getch();
      continue;
    }
    
    sys_branch=op_br;
    sys_operator=op_no;
    strcpy(sys_op_name,op.op_name);
    strcpy(sys_op_level,op.op_level);
    sys_today=br.work_d;
    sys_sernum=br.ser_num+1;
    clear_scr(5,22,18,77);
    main_menu();
  }
}

main_menu() 
{
  static cenitem mainmenu[]=
  {
     3,70,"退出", 
     3, 4,"存折业务", 
     3,14,"卡业务", 
     3,24,"综合业务", 
     3,34,"业务轧帐",
     3,44,"流水查询",
     3,54,"服务功能",
  };
  short choice,ymd[3];
  short op_br,op_no,sernum;

  disp_main(2);
  op_br=sys_branch;
  op_no=sys_operator;
  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    
    sernum=1;
    
    mvprintw(1,68,"流水号:%d",sys_sernum);
    refresh();
    init_in(&da);
    choice=menuctl(mainmenu,6,choice,0);
    switch(choice)
    { 
      case 1:
        ss_p(mainmenu[choice].x);
        break;
      case 2:
        card(mainmenu[choice].x);
        break;
      case 3:
        zh_p(mainmenu[choice].x);
        break;
      case 4:
        total_p(mainmenu[choice].x);
        break;
      case 5:
        query(mainmenu[choice].x);
        break;
      case 6:
        server(mainmenu[choice].x);
        break;
      case 0:
        op_no=sys_operator;
        sprintf(sql,"UPDATE operfile SET tty_no= '%s' ,tty_pid = %d WHERE operator = %d","无",0,op_no);
  
        if(mysql_query(conn,sql)){
          sprintf(sys_err_str,"修改操作员错! %s",mysql_error(conn));
          dis_str(22,18,sys_err_str,60,0);
          getch();
          continue;
        }
        mysql_close(conn);//程序的其他地方不要关闭连接
        keypad(stdscr,FALSE); echo(); noraw(); endwin();
        system("clear");
        exit(0);
      default:
        dis_str(22,18,"错误选择!",0,0);
        getch();
        break;
    }
  }
}


ss_p(no)
short no;
{
  static cenitem s_menu[]=
  {
   19,30,"0_退    出",
    8,30,"1_开    户",
    9,30,"2_存    款",
   10,30,"3_取    款",
   11,30,"4_销    户",
   12,30,"5_挂    失",
   13,30,"6_解    挂",
   14,30,"7_换    折",
   15,30,"8_密码重置",
   16,30,"9_本日冲正",
   17,30,"10查    询",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(s_menu,10,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      case 1:
        sys_code=1501;
        s_open();
        break;
      case 2:
        sys_code=1503;
        s_dep();
        break;
      case 3:
        sys_code=1504;
        s_withdraw();
        break;
      case 4:
        sys_code=1502;
        s_close();
        break;
      case 5:
        sys_code=1511;
        s_lost();
        break;
      case 6:
        sys_code=1521;
        s_unlost();
        break;
      case 7:
        sys_code=1592;
        s_chghb();
        break;
      case 8:
        s_pwreset();
        break;
      case 9:
        sys_code=1581;
        s_correct();
        break;
      case 10:
        sys_code=1561;
        s_query();
        break;
      default:
        dis_str(22,18,"错误选择!",0,0);
        getch();
        break;
    }
  }
}

query(no)
short no;
{
  static cenitem query_menu[]=
  {
   11,30,"0_返          回",
    8,30,"1_按交易号查流水",
    9,30,"2_按操作员查流水",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(query_menu,2,choice,no);
    switch(choice)
    { 
      case 1:
        sys_code=1611;
        qtrade_ser();
        break;
      case 2:
        sys_code=1613;
        qtrade_op();
        break;
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      default:
        dis_str(22,18,"错误选择!",0,0);
        getch();
        break;
    }
  }
}

zh_p(no)
short no;
{
  static cenitem zh_menu[]=
  {
   15,30,"0_退    出",
    8,30,"1_调整总帐",
    9,30,"2_总帐冲正",
   10,30,"3_调整凭证",
   11,30,"4_凭证冲正",
   12,30,"5_查询总帐",
   13,30,"6_查询凭证",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(zh_menu,6,choice,no);
    switch(choice)
    { 
      case 1:
        zh_pro_sum();
        break;
      case 2:
        zh_cro_sum();
        break;
      case 3:
        zh_pro_vou();
        break;
      case 4:
        zh_cro_vou();
        break;
      case 5:
        zh_quer_sum();
        break;
      case 6:
        zh_quer_vou();
        break;
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      default:
        dis_str(22,18,"错误选择!",0,0);
        getch();
        break;
    }
  }
}

server(no)
short no;
{
  static cenitem server_menu[]=
  {
   12,30,"0_退    出",
    8,30,"1_查校验位",
    9,30,"2_查 日 历",
   10,30,"3_修改密码",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(server_menu,3,choice,no);
    switch(choice)
    { 
      case 1:
        query_chk();
        break;
      case 2:
        server_cal();
        break;
      case 3:
        update_pwd();
        break;
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      default:
        dis_str(22,18,"错误选择!",0,0);
        getch();
        break;
    }
  }
}


total_p(no)
short no;
{
  static cenitem server_menu[]=
  {
   11,30,"0_退        出",
    8,30,"1_按全机构轧帐",
    9,30,"2_按操作员轧帐",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(server_menu,2,choice,no);
    switch(choice)
    { 
      case 1:
        sys_code=1811;
        total_br();
        break;
      case 2:
        sys_code=1812;
        total_op();
        break;
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      default:
        dis_str(22,18,"错误选择!",0,0);
        getch();
        break;
    }
  }
}



card(no)
short no;
{
  static cenitem s_menu[]=
  {
   18,30,"0_退    出",
    8,30,"1_开    户",
    9,30,"2_存折补卡",
   10,30,"3_存    款",
   11,30,"4_取    款",
   12,30,"5_销    户",
   13,30,"6_挂    失",
   14,30,"7_解    挂",
   15,30,"8_本日冲正",
   16,30,"9_查    询",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(s_menu,9,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      case 1:
        sys_code=1501;
        c_open();
        break;
      case 2:
        sys_code=1591;
        c_scard();
        break;
      case 3:
        sys_code=1503;
        c_dep();
        break;
      case 4:
        sys_code=1504;
        c_withdraw();
        break;
      case 5:
        sys_code=1502;
        c_close();
        break;
      case 6:
        sys_code=1511;
        c_lost();
        break;
      case 7:
        sys_code=1521;
        c_unlost();
        break;
      case 8:
        sys_code=1581;
        c_correct();
        break;
      case 9:
        sys_code=1561;
        c_query();
        break;
      default:
        dis_str(22,18,"错误选择!",0,0);
        getch();
        break;
    }
  }
}
