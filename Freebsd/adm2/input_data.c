#include <stdio.h>
#include <string.h>
#include <curses.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"
/****************************************************************
 增加一输入项涉及内容:
   1.本程序增加 case
   2.本程序init_in(in_da)增加初始化
   3.include/sh_table.h中 $struct in_data 说明
 跟踪参数:
   printf("n=%d\n\r", n);
   for (flag=1;flag<=num;flag++)
   {
     printf("i=%d,in[i]=%d\n\r", flag,in[flag]);
     getch();
   }

 1:帐    号   2:发 生 额   3:摘    要   4:储    种   5:凭证类型
 6:查询帐号   7:新 帐 号   8:确    认   9:单位名称  10:余    额
11:交 易 号  12:业务日期  13:科目名称  14:明细笔数  15:操作员号
16:户    名  17:柜员姓名  18:柜员权限  19:原 密 码  20:新 密 码
21:新 密 码  22:起始帐号  23:终止帐号  24:科 目 号  25:年    份
26:开户所号  27:业务所号  28:地    址  29:开 户 日  30:存    期
31:业务柜员  32:帐户性质  33:利    息  34:保 值 息  35:查询方式
36:总帐方向  37:凭证方向  38:业务笔数  39:记录标志  40:开销户数
41:业务类型  42:交易号1   43:交易号2   44:所    名  45:监督员号
46:统计标志  47:打印顺序  48:凭证名称  49:凭证代码  50:利率类型
51:利    率  52:选择所号  53:报表类型  54:月    份  55:贷方金额
56:借方金额  57:开 户 数  58:销 户 数  59:贷方笔数  60:借方笔数
61:余    户  62:利 息 税  63:证件类型  64:证件号码  65:开户帐号
66:卡    号  67:起始卡号  68:终止卡号  69:查询方式  70:取开户帐号
71:取卡号    72:输入密码  73:确认密码
*****************************************************************/


extern MYSQL *conn;
extern char sql[];

//取户名(若能知道储种,则可分开,否则只能活期)
get_acc_name(short br, long acc)
{

  short branch;
  long account;
  char customer[13];
  
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  branch=br;
  account=acc;
  
  sprintf(sql,"SELECT customer FROM s_mast WHERE  open_br= %d AND account = %d",branch,account);
  	
  mysql_query(conn,sql);
  res=mysql_store_result(conn);

  if((row=mysql_fetch_row(res))!=NULL){
	strcpy(customer,row[0]);
    mvprintw(8,50,"户名:[%s]",customer);
  }
  mysql_free_result(res);
  
  refresh();

  return(0);
}


input_data(n,in,da)
struct in_data *da;
short n,in[];
{
  short line,kk=0,flag;
  short ymd[3],chk,old_chk;
  long tmp_var;
  char tmp_str[15],yn[2];
  char name[13];
  char zh[10];
  char str2[100];
  char qq;

  kk=0;
  while (kk<n)
  {
    if (in[kk]==99)
      break;
    clear_line(22,2,77);
    tmp_var=0;
    line=8+kk;
    switch(in[kk])
    {
   	case 1:
          mvaddstr( line,30,"帐    号:[         ]");
          dis_str(22,22,"请输入帐号(含1位校验位).",0,0); 
          mvprintw(line,40,"%-9ld]",da->account);
          refresh();
          tmp_var=da->account; 
          while((flag=acc_long( line,40,&tmp_var,9))==Err);

          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->account=tmp_var;
 
          if (da->account==99999999)
            return(-1);   
          if (da->account==0)
          {
            kk++;
            break;
/**
            dis_str(22,22,"输入帐号错!",0,0); 
            getch();
**/
          }
          old_chk=da->account%10;
          da->account=da->account/10;
          chk=get_chk_no(da->open_br,da->account);
          if (chk<0)
          {
            dis_str(22,22,"计算校验位错!",0,0);  
            getch();
            break;
          }
          if (chk!=old_chk)
          {
            dis_str(22,22,"输入帐号错!",0,0);  
            getch();
            break;
          }

          mvprintw(line,40,"%8ld_%1d]",da->account,chk);
          refresh();

          //取户名,显示在 (8,50)
          //get_acc_name(da->open_br,da->account);

          kk++;
          break;
   	case 2:
          mvaddstr( line,30,"发 生 额:[              ]");
          dis_str(22,22,"请输入金额.",0,0); 
          sprintf(tmp_str,"%-10.2lf",da->amount/100.00);
          mvprintw(line,40,"%s",tmp_str);
          refresh();
          while((flag=acc_amt(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          unpoint(tmp_str);
          da->amount=atol(tmp_str);
          mvprintw(line,40,"%-10.2lf",da->amount/100.00);
          refresh();
          kk++;
          break;
   	case 3:
          mvaddstr(line,30,"摘    要:[0]        ");
          dis_str(22,22,"请输入摘要:(0现金 1转收 2转付 3托收 4划付).",0,0); 
          if (da->abstract[0]=='0')
            mvaddstr(line,40,"现金]");
          else if (da->abstract[0]=='1')
            mvaddstr(line,40,"转收]  ");
          else if (da->abstract[0]=='2')
            mvaddstr(line,40,"转付]  ");
          else if (da->abstract[0]=='3')
            mvaddstr(line,40,"托收]  ");
          else if (da->abstract[0]=='4')
            mvaddstr(line,40,"划付]  ");
          else
            mvaddstr(line,40,"未知]  ");
          refresh();
          while((flag=acc_str(line,40,da->abstract,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (da->abstract[0]=='0')
            mvaddstr(line,40,"现金]");
          else if (da->abstract[0]=='1')
            mvaddstr(line,40,"转收]  ");
          else if (da->abstract[0]=='2')
            mvaddstr(line,40,"转付]  ");
          else if (da->abstract[0]=='3')
            mvaddstr(line,40,"托收]  ");
          else if (da->abstract[0]=='4')
            mvaddstr(line,40,"划付]  ");
          else
          {
            dis_str(22,22,"输入摘要错!",0,0); 
            break;
          }
          refresh();
          refresh();
          kk++;
          break;
   	case 4:
          mvaddstr(line,30,"储    种:[    ]        ");
          dis_str(22,22,"请输入储种(1 活期  2 定期 ).",0,0); 
          strcpy(da->ltype,"1");
          if (da->ltype[0]=='2')
            mvaddstr(line,40,"定期]");
          else if (da->ltype[0]=='1')
            mvaddstr(line,40,"活期]");
          else
            mvaddstr(line,40,"未知]");
	
          while((flag=acc_str(line,40,da->ltype,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (da->ltype[0]=='1')
            mvaddstr(line,40,"活期]");
          else if (da->ltype[0]=='2')
            mvaddstr(line,40,"定期");
          else
          {
            dis_str(22,22,"输入储种错!",0,0); 
            break;
          }
          kk++;
          break;
   	case 5:
          mvaddstr(line,30,"凭证类型:[ ]        ");
          dis_str(22,22,"凭证类型(2_活折 3_卡 )",0,0);
          if (da->vou_type[0]=='2')
            mvaddstr(line,40,"活折]");
          else if (da->vou_type[0]=='3')
            mvaddstr(line,40,"卡]  ");
          else
            mvaddstr(line,40,da->vou_type);
          while((flag=acc_str(line,40,da->vou_type,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (da->vou_type[0]=='2')
            mvaddstr(line,40,"活折]");
          else if (da->vou_type[0]=='3')
            mvaddstr(line,40,"卡]  ");
          else
          {
            dis_str(22,22,"输入凭证类型错!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 6:
          mvaddstr(line,30,"帐    号:[        ]");
          dis_str(22,22,"请输入帐号(不含校验位).",0,0); 
          mvprintw(line,40,"%-8ld]",da->que_acc);
          refresh();
          tmp_var=da->que_acc;
          while((flag=acc_long( line,40,&tmp_var,8))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->que_acc=tmp_var;

          if (da->que_acc==0)
          {
            dis_str(22,22,"输入帐号错!",0,0); 
            getch();
            break;
          }
          kk++;
          break;
   	case 7:
          mvaddstr(line,30,"新户帐号:[         ]");
          dis_str(22,22,"请输入新开户的帐号(含1位校验位).",0,0); 
          mvprintw(line,40,"%-9ld",da->new_acc);
          refresh();
          tmp_var=da->new_acc;
          while((flag=acc_long( line,40,&tmp_var,9))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->new_acc=tmp_var;
          if (da->new_acc==0)
          {
            dis_str(22,22,"输入帐号错!",0,0); 
            getch();
            break;
          }
          old_chk=da->new_acc%10;
          da->new_acc=da->new_acc/10;
          chk=get_chk_no(da->open_br,da->new_acc);
          if (chk<0)
          {
            dis_str(22,22,"计算校验位错!",0,0);  
            getch();
            break;
          }
          if (chk!=old_chk)
          {
            dis_str(22,22,"输入帐号错!",0,0);  
            getch();
            break;
          }
          mvprintw(line,40,"%8ld_%1d]",da->new_acc,chk);
          refresh();
          kk++;
          break;
        case 8:
          mvaddstr(line,30,"确    认:[ ]");
          dis_str(22,22,"请输入 y 或 n.",0,0); 
          yn[0]=0;
          while((flag=acc_str(line,40,yn,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(yn[0]==0)
            break;
          if(yn[0]!='y'&&yn[0]!='Y'&& yn[0]!='n'&&yn[0]!='N')
            break;
          if(yn[0]=='y'||yn[0]=='Y')
            kk++;
          else
            kk=0;
          break;
        case 9:
          mvaddstr(line,30,"单位名称:[                    ]");
          dis_str(22,22,"请输入单位名称.",0,0); 
          mvprintw(line,40,"%s",da->open_name);
          refresh();
          while((flag=acc_str(line,40,da->open_name,20))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->open_name[20]='\0';
          str_left(da->open_name);
          kk++;
          break;
        case 10:
          mvaddstr(line,30,"余    额:[              ]");
          dis_str(22,22,"请输入帐户余额.",0,0); 
          sprintf(tmp_str,"%-10.2lf",da->balance/100.00);
          mvprintw(line,40,"%s",tmp_str);
          refresh();
          while((flag=acc_amt(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          unpoint(tmp_str);
          da->balance=atol(tmp_str);
          mvprintw(line,40,"%-10.2lf",da->balance/100.00);
          refresh();
          kk++;
          break;
   	case 11:
          mvaddstr(line,30,"交 易 号:[    ]");
          dis_str(22,22,"请输入原业务交易号.",0,0); 
          mvprintw(line,40,"%-4d",da->ser_num);
          refresh();
          tmp_var=da->ser_num;
          while((flag=acc_long( line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->ser_num=tmp_var;
          if ( da->ser_num<0 || da->ser_num>9999)
          {
            dis_str(22,22,"输入交易号错!",0,0); 
            break;
          }
          mvprintw(line,40,"%-4d",da->ser_num);
          refresh();
         
          kk++;
          break;
        case 12:
          mvaddstr(line,30,"业务日期:[          ]");
          dis_str(22,22,"请输入原业务日期(年年年年/月月/日日).",0,0); 
          rfmtdate(da->k_date,"yyyy/mm/dd",tmp_str);
          mvprintw(line,40,"%s",tmp_str);
          refresh();
          while((flag=acc_date(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(tmp_str[0]==0)
            break;
          da->k_date=d_str_long(tmp_str);
          if(da->k_date<0)
          {
            dis_str(22,22,"输入日期错!",0,0); 
            break;
          }
          kk++;
          break;
   	case 13:
          mvaddstr(line,30,"科目名称:[                    ]");
          dis_str(22,22,"请输入科目名称.",0,0); 
          mvprintw(line,40,"%s",da->item_name);
          refresh();
          while((flag=acc_str(line,40,da->item_name,20))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->item_name[20]='\0';
          str_left(da->item_name);
          kk++;
          break;
   	case 14:
          mvaddstr(line,30,"明细笔数:[      ]");
          dis_str(22,22,"请输入原业务明细笔数.",0,0); 
          tmp_var=da->detail_num;
          while((flag=acc_long( line,40,&tmp_var,3))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->detail_num=(short)tmp_var;
          kk++;
          break;
   	case 15:
          mvaddstr(line,30,"操作员号:[  ]");
          dis_str(22,22,"请输入操作员号.",0,0); 
          mvprintw(line,40,"%-2d",da->operator);
          refresh();
          tmp_var=da->operator;
          while((flag=acc_long( line,40,&tmp_var,2))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->operator=(short)tmp_var;
          kk++;
          break;
   	case 16:
          mvaddstr(line,30,"户    名:[            ]");
          dis_str(22,22,"请输入存折帐户的姓名.",0,0); 
          mvprintw(line,40,"%s",da->customer);
          refresh();
          while((flag=acc_str(line,40,da->customer,12))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->customer[12]='\0';
          str_left(da->customer);
          kk++;
          break;
   	case 17:
          mvaddstr(line,30,"柜员姓名:[        ]");
          dis_str(22,22,"请输入柜员姓名.",0,0); 
          da->name[0]=0;
          while((flag=acc_str(line,40,da->name,8))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->name[8]='\0';
          str_left(da->name);
          kk++;
          break;
   	case 18:
          mvaddstr(line,30,"柜员权限:[ ]");
          dis_str(22,22,"请输入权限( 1记帐员 7主机员 4所主任).",0,0); 
          da->level[0]=0;
          while((flag=acc_str(line,40,da->level,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (da->level[0]=='1')
            mvaddstr(line,40,"记帐员]");
          else if (da->level[0]=='7')
            mvaddstr(line,40,"主机员]");
          else if (da->level[0]=='4')
            mvaddstr(line,40,"所主任]");
          else
          {
            dis_str(22,22,"输入错!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 19:
          mvaddstr(line,30,"原 密 码:[      ]");
          dis_str(22,22,"请输入操作员原密码.",0,0); 
          while((flag=acc_pwd(line,40,da->pwd_old,6))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(da->pwd_old[0]=='\0') 
            break;
          kk++;
          break;
   	case 20:
          mvaddstr(line,30,"新 密 码:[      ]");
          dis_str(22,22,"请输入操作员新密码.",0,0); 
          while((flag=acc_pwd(line,40,da->pwd_new1,6))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(da->pwd_new1[0]=='\0') 
            break;
          kk++;
          break;
   	case 21:
          mvaddstr(line,30,"新 密 码:[      ]");
          dis_str(22,22,"请输入操作员新密码.",0,0); 
          while((flag=acc_pwd(line,40,da->pwd_new2,6))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(da->pwd_new2[0]=='\0') 
            break;
          kk++;
          break;
   	case 22:
          mvaddstr( line,30,"起始帐号:[        ]");
          dis_str(22,22,"请输入起始帐号(不含最末位数）).",0,0); 
          mvprintw(line,40,"%8ld",da->account1);
          refresh();
          tmp_var=da->account1;
          while((flag=acc_long( line,40,&tmp_var,8))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->account1=tmp_var;
          mvprintw(line,40,"%8ld",da->account1);
          refresh();
          kk++;
          break;
   	case 23:
          mvaddstr( line,30,"终止帐号:[        ]");
          dis_str(22,22,"请输入终止帐号(不含最末位）.",0,0); 
          da->account2=da->account1;
          mvprintw(line,40,"%8ld",da->account2);
          refresh();
          tmp_var=da->account2;
          while((flag=acc_long( line,40,&tmp_var,8))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->account2=tmp_var;
          mvprintw(line,40,"%8ld",da->account2);
          refresh();
          kk++;
          break;
   	case 24:
          mvaddstr( line,30,"科 目 号:[      ]");
          mvprintw(line,40,"%-6ld",da->item);
          refresh();
          tmp_var=da->item;
          dis_str(22,22,"请输入科目号.",0,0); 
          while((flag=acc_long(line,40,&tmp_var,6))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->item=tmp_var;
          if (da->item<100000 || da->item>1000000)
          {
            dis_str(22,22,"输入科目号错!",0,0); 
            break;
          }
          mvprintw(line,40,"%-6ld",da->item);
          refresh();
          kk++;
          break;
   	case 25:
          mvaddstr( line,30,"年    份:[    ]");
          tmp_var=da->year;
          mvprintw(line,40,"%-4d",da->year);
          refresh();
          dis_str(22,22,"请输入年份(YYYY).",0,0); 
          while((flag=acc_long( line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->year=tmp_var;
          mvprintw(line,40,"%-4d",da->year);
          refresh();
          kk++;
          break;
   	case 26:
          mvaddstr(line,30,"开户所号:[   ]");
          dis_str(22,22,"请输入本业务开户所号.",0,0); 
          mvprintw(line,40,"%3d",da->open_br);
          refresh();
          tmp_var=da->open_br;
          while((flag=acc_long( line,40,&tmp_var,3))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->open_br=tmp_var;
          mvprintw(line,40,"%3d",da->open_br);
          refresh();
          kk++;
          break;
   	case 27:
          mvaddstr(line,30,"业务所号:[   ]");
          dis_str(22,22,"请输入业务所号.",0,0); 
          mvprintw(line,40,"%3d",da->tr_br);
          refresh();
          tmp_var=da->tr_br;
          while((flag=acc_long( line,40,&tmp_var,3))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->tr_br=tmp_var;
          mvprintw(line,40,"%3d",da->tr_br);
          refresh();
          kk++;
          break;
   	case 28:
          mvaddstr(line,30,"地    址:[                              ]");
          dis_str(22,22,"请输入开户人的地址.",0,0); 
          mvprintw(line,40,"%s",da->address);
          refresh();
          while((flag=acc_str(line,40,da->address,30))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->address[30]='\0';
          str_left(da->address);
          kk++;
          break;
        case 29:
          mvaddstr(line,30,"开户日期:[          ]");
          dis_str(22,22,"请输入本户开户日期(年年年年/月月/日日).",0,0); 

          rjulmdy(da->open_d,ymd); 

          sprintf(tmp_str,"%04d/%02d/%02d",ymd[2],ymd[0],ymd[1]);
          mvprintw(line,40,"%10s",tmp_str);
          refresh();
          while((flag=acc_date(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(tmp_str[0]==0)
            break;
          da->open_d=d_str_long(tmp_str);
          if(da->open_d<0)
          {
            dis_str(22,22,"输入日期错!",0,0); 
            break;
          }
          kk++;
          break;
   	case 30:
          mvaddstr(line,30,"存    期:[  ]");
          dis_str(22,22,"请输入存期(以月为单位,活期为0).",0,0); 
          mvprintw(line,40,"%-2d",da->term);
          refresh();
          tmp_var=da->term;
          while((flag=acc_long( line,40,&tmp_var,2))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(tmp_var!=0)
            da->term=tmp_var;
          if (da->term!=3 && da->term!=6 &&da->term!=9 &&
          da->term!=12 && da->term!=24 &&da->term!=36 &&
          da->term!=60 && da->term!=96 && da->term!=0 && da->term!=99)
          {
            dis_str(22,22,"输入存期错!",0,0); 
            break;
          }
          mvprintw(line,40,"%-2d",da->term);
          refresh();
          kk++;
          break;
   	case 31:
          mvaddstr(line,30,"业务柜员:[  ]");
          dis_str(22,22,"请输入业务操作员号.",0,0); 
          mvprintw(line,40,"%-2d",da->tr_op);
          refresh();
          tmp_var=da->tr_op;
          while((flag=acc_long( line,40,&tmp_var,2))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(tmp_var!=0)
            da->tr_op=tmp_var;
          mvprintw(line,40,"%-2d",da->tr_op);
          refresh();
          kk++;
          break;
   	case 32:
          mvaddstr(line,30,"帐户性质:[ ]        ");
          dis_str(22,22,"请输入帐户性质(1 活折 2 定期 3 卡).",0,0); 
          refresh();
          if (da->acc_type[0]=='D'||da->acc_type[0]=='2')
            mvprintw(line,40,"定期]      ");
          else if (da->acc_type[0]=='H'|| da->acc_type[0]=='1')
            mvprintw(line,40,"活期存折]  ");
          else if (da->acc_type[0]=='K'|| da->acc_type[0]=='3')
            mvprintw(line,40,"卡]        ");
          else
            mvprintw(line,40,"未    知]  ");
          refresh();
          while((flag=acc_str(line,40,da->acc_type,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          mvprintw(line,40,"[      ");
          refresh();
          if (da->acc_type[0]=='2'|| da->acc_type[0]=='D')
          {
            mvprintw(line,40,"定期]      ");
            refresh();
            strcpy(da->acc_type,"D");
          }
          else if (da->acc_type[0]=='H'|| da->acc_type[0]=='1')
          {
            mvprintw(line,40,"活期存折]  ");
            refresh();
            strcpy(da->acc_type,"H");
          }
          else if (da->acc_type[0]=='K'|| da->acc_type[0]=='3')
          {
            mvprintw(line,40,"卡]        ");
            refresh();
            strcpy(da->acc_type,"K");
          }
          else
          {
            dis_str(22,22,"输入帐户性质错!",0,0); 
            refresh();
          }
          kk++;
          break;
   	case 33:
          mvaddstr( line,30,"利    息:[              ]");
          dis_str(22,22,"请输入销户利息或利息积数.",0,0); 
          sprintf(tmp_str,"%-10.2lf",da->interest/100.00);
          mvprintw(line,40,"%s",tmp_str);
          refresh();
          while((flag=acc_amt(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          unpoint(tmp_str);
          da->interest=atol(tmp_str);
          mvprintw(line,40,"%-10.2lf",da->interest/100.00);
          refresh();
          kk++;
          break;
   	case 34:
          mvaddstr( line,30,"保值利息:[              ]");
          dis_str(22,22,"请输入销户保值利息.",0,0); 
          sprintf(tmp_str,"%-10.2lf",da->keep_int/100.00);
          mvprintw(line,40,"%s",tmp_str);
          refresh();
          while((flag=acc_amt(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          unpoint(tmp_str);
          da->keep_int=atol(tmp_str);
          mvprintw(line,40,"%-10.2lf",da->keep_int/100.00);
          refresh();
          kk++;
          break;
   	case 35:
          mvaddstr(line,30,"查询方式:[0]        ");
          dis_str(22,22,"请输入帐户查询方式:(1_按帐号 2_按金额 3_按姓名).",0,0); 
          if (da->q_flag[0]=='1')
            mvaddstr(line,40,"按帐号]");
          else if (da->q_flag[0]=='2')
            mvaddstr(line,40,"按金额]");
          else if (da->q_flag[0]=='3')
            mvaddstr(line,40,"按姓名]");
          else
            mvaddstr(line,40,"未  知]");
          refresh();
          while((flag=acc_str(line,40,da->q_flag,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (da->q_flag[0]=='1')
            mvaddstr(line,40,"按帐号]");
          else if (da->q_flag[0]=='2')
            mvaddstr(line,40,"按金额]");
          else if (da->q_flag[0]=='3')
            mvaddstr(line,40,"按姓名]");
          else
          {
            dis_str(22,22,"输入查询方式错!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 36:
          mvaddstr(line,30,"总帐方向:[    ]        ");
          dis_str(22,22,"请输入调整总帐的方向:(1借方 2贷方).",0,0); 
          if (da->sum_flag==1)
            mvaddstr(line,40,"借方]");
          else if (da->sum_flag==2)
            mvaddstr(line,40,"贷方]  ");
          else
            mvaddstr(line,40,"未知]  ");
          refresh();
          tmp_var=da->sum_flag;
          while((flag=acc_long(line,40,&tmp_var,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->sum_flag=tmp_var;
          if (da->sum_flag==1)
            mvaddstr(line,40,"借方]");
          else if (da->sum_flag==2)
            mvaddstr(line,40,"贷方]  ");
          else
          {
            dis_str(22,22,"输入总帐方向错!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 37:
          mvaddstr(line,30,"凭证方向:[    ]        ");
          dis_str(22,22,"请输入调整凭证方向:(1领回 2使用 3作废 4上缴 5换折).",0,0); 
          if (da->vou_flag==1)
            mvaddstr(line,40,"领回]");
          else if (da->vou_flag==2)
            mvaddstr(line,40,"使用]  ");
          else if (da->vou_flag==3)
            mvaddstr(line,40,"作废]  ");
          else if (da->vou_flag==4)
            mvaddstr(line,40,"上缴]  ");
          else if (da->vou_flag==5)
            mvaddstr(line,40,"换折]  ");
          else
            mvaddstr(line,40,"未知]  ");
          refresh();
          tmp_var=da->vou_flag;
          while((flag=acc_long(line,40,&tmp_var,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->vou_flag=tmp_var;

          if (da->vou_flag==1)
            mvaddstr(line,40,"领回]");
          else if (da->vou_flag==2)
            mvaddstr(line,40,"使用]  ");
          else if (da->vou_flag==3)
            mvaddstr(line,40,"作废]  ");
          else if (da->vou_flag==4)
            mvaddstr(line,40,"上缴]  ");
          else if (da->vou_flag==5)
            mvaddstr(line,40,"换折]  ");
          else
          {
            dis_str(22,22,"输入凭证方向错!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 38:
          mvaddstr(line,30,"业务笔数:[    ]        ");
          dis_str(22,22,"请输入调整本科目的业务笔数.",0,0); 
          mvprintw(line,40,"%-4d",da->sum_que);
          refresh();
          tmp_var=da->sum_que;
          while((flag=acc_long(line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->sum_que=tmp_var;
          mvprintw(line,40,"%-4d",da->sum_que);
          refresh();
          refresh();
          kk++;
          break;
   	case 39:
          mvaddstr(line,30,"记录标志:[   ]        ");
          dis_str(22,12,"请输入:0_去年 1-12某月 13_昨日 14_本年 101-131_为1-31某日.",0,0); 
          mvprintw(line,40,"%-3d",da->item_kind);
          refresh();
          tmp_var=da->item_kind;
          while((flag=acc_long(line,40,&tmp_var,3))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->item_kind=tmp_var;
          if (da->item_kind==13)
            da->item_kind=91;
          if (da->item_kind==14)
            da->item_kind=99;
          mvprintw(line,40,"%-3d",da->item_kind);
          refresh();
          kk++;
          break;
   	case 40:
          mvaddstr(line,30,"开销户数:[    ]        ");
          dis_str(22,22,"请输入调整本科目的开户或销户户数.",0,0); 
          mvprintw(line,40,"%-4d",da->home_que);
          refresh();
          tmp_var=da->home_que;
          while((flag=acc_long(line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->home_que=tmp_var;
          mvprintw(line,40,"%-4d",da->home_que);
          refresh();
          refresh();
          kk++;
          break;
   	case 41:
          mvaddstr(line,30,"业务类型:[      ]");
          dis_str(22,12,"请输入业务类型:(1开户 2销户 3续存 4支取 5部提 6补开 7调总帐 8调凭证).",0,0); 
          if (da->tr_code==1)
            mvaddstr(line,40,"开户]");
          else if (da->tr_code==2)
            mvaddstr(line,40,"销户]  ");
          else if (da->tr_code==3)
            mvaddstr(line,40,"续存]  ");
          else if (da->tr_code==4)
            mvaddstr(line,40,"支取]  ");
          else if (da->tr_code==5)
            mvaddstr(line,40,"部提]  ");
          else if (da->tr_code==6)
            mvaddstr(line,40,"补开]  ");
          else if (da->tr_code==7)
            mvaddstr(line,40,"调总帐]  ");
          else if (da->tr_code==8)
            mvaddstr(line,40,"调凭证]  ");
          else
            mvaddstr(line,40,"未知]  ");
          refresh();
          tmp_var=da->tr_code;
          while((flag=acc_long(line,40,&tmp_var,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->tr_code=tmp_var;
          if (da->tr_code==1)
            mvaddstr(line,40,"开户]");
          else if (da->tr_code==2)
            mvaddstr(line,40,"销户]  ");
          else if (da->tr_code==3)
            mvaddstr(line,40,"续存]  ");
          else if (da->tr_code==4)
            mvaddstr(line,40,"支取]  ");
          else if (da->tr_code==5)
            mvaddstr(line,40,"部提]  ");
          else if (da->tr_code==6)
            mvaddstr(line,40,"补开]  ");
          else if (da->tr_code==7)
            mvaddstr(line,40,"调总帐]  ");
          else if (da->tr_code==8)
            mvaddstr(line,40,"调凭证]  ");
          else
          {
            dis_str(22,22,"输入业务类型错!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 42:
          mvaddstr(line,30,"起始交易:[    ]");
          dis_str(22,22,"请输入需查询的起始交易号.",0,0); 
          mvprintw(line,40,"%-4d",da->ser_num1);
          refresh();
          tmp_var=da->ser_num1;
          while((flag=acc_long( line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->ser_num1=tmp_var;
          if ( da->ser_num1<0 || da->ser_num1>9999)
          {
            dis_str(22,22,"输入交易号错!",0,0); 
            break;
          }
          mvprintw(line,40,"%-4d",da->ser_num1);
          refresh();
          kk++;
          break;
   	case 43:
          mvaddstr(line,30,"终止交易:[    ]");
          dis_str(22,22,"请输入需查询的终止交易号.",0,0); 
          mvprintw(line,40,"%-4d",da->ser_num2);
          refresh();
          tmp_var=da->ser_num2;
          while((flag=acc_long(line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->ser_num2=tmp_var;
          if ( da->ser_num2<da->ser_num1 || da->ser_num2>9999)
          {
            dis_str(22,22,"输入交易号错!",0,0); 
            break;
          }
          mvprintw(line,40,"%-4d",da->ser_num2);
          refresh();
         
          kk++;
          break;
   	case 44:
          mvaddstr(line,30,"所    名:[          ]");
          dis_str(22,22,"请输入所名.",0,0); 
          mvprintw(line,40,"%s",da->br_name);
          refresh();
          while((flag=acc_str(line,40,da->br_name,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->br_name[10]='\0';
          str_left(da->br_name);
          kk++;
          break;
   	case 45:
          mvaddstr(line,30,"监督员号:[  ]");
          dis_str(22,22,"请输入监督员号.",0,0); 
          mvprintw(line,40,"%-2d",da->oper);
          refresh();
          tmp_var=da->oper;
          while((flag=acc_long( line,40,&tmp_var,2))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(tmp_var!=0)
            da->oper=tmp_var;
          mvprintw(line,40,"%-2d",da->oper);
          refresh();
          kk++;
          break;
   	case 46:
          mvaddstr( line,30,"统计标志:[      ]");
          mvprintw(line,40,"%-6ld",da->sum_flag);
          refresh();
          tmp_var=da->sum_flag;
          dis_str(22,22,"请输入统计标志.",0,0); 
          while((flag=acc_long(line,40,&tmp_var,6))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->sum_flag=tmp_var;
          mvprintw(line,40,"%-6ld",da->sum_flag);
          refresh();
          kk++;
          break;
   	case 47:
          mvaddstr( line,30,"打印顺序:[   ]");
          mvprintw(line,40,"%-3ld",da->prt_ser);
          refresh();
          tmp_var=da->prt_ser;
          dis_str(22,22,"请输入打印顺序.",0,0); 
          while((flag=acc_long(line,40,&tmp_var,3))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->prt_ser=tmp_var;
          mvprintw(line,40,"%-3ld",da->prt_ser);
          refresh();
          kk++;
          break;
   	case 48:
          mvaddstr(line,30,"凭证名称:[          ]");
          dis_str(22,22,"请输入凭证名称.",0,0); 
          mvprintw(line,40,"%s",da->vou_name);
          refresh();
          while((flag=acc_str(line,40,da->vou_name,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->vou_name[10]='\0';
          str_left(da->vou_name);
          kk++;
          break;
   	case 49:
          mvaddstr(line,30,"凭证代码:[  ]");
          dis_str(22,22,"请输入凭证代码(两位).",0,0); 
          mvprintw(line,40,"%s",da->vou_code);
          refresh();
          while((flag=acc_str(line,40,da->vou_code,2))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->vou_code[2]='\0';
          str_left(da->vou_code);
          kk++;
          break;
   	case 50:
          mvaddstr(line,30,"利率类型:[ ]        ");
          dis_str(22,22,"请输入储种(1 活期 2 定期).",0,0); 
          if (da->rate_type[0]=='1')
            mvaddstr(line,40,"活期]");
          else if (da->rate_type[0]=='2')
            mvaddstr(line,40,"定期]");
          else
            mvaddstr(line,40,"未知]");
    refresh();
          while((flag=acc_str(line,40,da->rate_type,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (da->rate_type[0]=='1')
            mvaddstr(line,40,"活期]");
          else if (da->rate_type[0]=='2')
            mvaddstr(line,40,"定期]");
          else
          {
            dis_str(22,22,"输入利率类型错!",0,0); 
            break;
          }
    refresh();
          kk++;
          break;
   	case 51:
          mvaddstr( line,30,"利    率:[        ]");
          mvprintw(line,40,"%-8ld",da->rate);
          refresh();
          tmp_var=da->rate;
          dis_str(22,22,"请输入利率(年利率*1000000)",0,0); 
          while((flag=acc_long(line,40,&tmp_var,8))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->rate=tmp_var;
          mvprintw(line,40,"%-8ld",da->rate);
          refresh();
          kk++;
          break;
   	case 52:
          mvaddstr(line,30,"所    号:[   ]");
          dis_str(22,22,"请输入所号(999_全部所 ABC_指定所 888_全机).",0,0); 
          mvprintw(line,40,"%3d",da->open_br);
          refresh();
          tmp_var=da->open_br;
          while((flag=acc_long( line,40,&tmp_var,3))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->open_br=tmp_var;
          mvprintw(line,40,"%3d",da->open_br);
          refresh();
          kk++;
          break;
   	case 53:
          mvaddstr(line,30,"报表类型:[   ]");
          dis_str(22,22,"请输入报表类型(1_日报 2_月报 3_年报).",0,0); 
          mvprintw(line,40,"%1d",da->bal_flag);
          refresh();
          tmp_var=da->bal_flag;
          while((flag=acc_long( line,40,&tmp_var,3))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->bal_flag=tmp_var;
          mvprintw(line,40,"%1d",da->bal_flag);
          refresh();
          kk++;
          break;
   	case 54:
          mvaddstr( line,30,"月    份:[    ]");
          tmp_var=da->month;
          mvprintw(line,40,"%-2d",da->month);
          refresh();
          dis_str(22,22,"请输入月份(MM).",0,0); 
          while((flag=acc_long( line,40,&tmp_var,2))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->month=tmp_var;
          mvprintw(line,40,"%-2d",da->month);
          refresh();
          kk++;
          break;
   	case 55:
          mvaddstr( line,30,"贷方金额:[              ]");
          dis_str(22,22,"请输入贷方金额.",0,0); 
          sprintf(tmp_str,"%-14.2lf",da->det_amt/100.00);
          mvprintw(line,40,"%s",tmp_str);
          refresh();
          while((flag=acc_amt(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          unpoint(tmp_str);
          da->det_amt=atol(tmp_str);
          mvprintw(line,40,"%-14.2lf",da->det_amt/100.00);
          refresh();
          kk++;
          break;
   	case 56:
          mvaddstr( line,30,"借方金额:[              ]");
          dis_str(22,22,"请输入借方金额.",0,0); 
          sprintf(tmp_str,"%-14.2lf",da->cre_amt/100.00);
          mvprintw(line,40,"%s",tmp_str);
          refresh();
          while((flag=acc_amt(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          unpoint(tmp_str);
          da->cre_amt=atol(tmp_str);
          mvprintw(line,40,"%-14.2lf",da->cre_amt/100.00);
          refresh();
          kk++;
          break;
   	case 57:
          mvaddstr(line,30,"开 户 数:[    ]");
          dis_str(22,22,"请输入开户数.",0,0); 
          tmp_var=da->open_num;
          while((flag=acc_long( line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->open_num=(short)tmp_var;
          kk++;
          break;
   	case 58:
          mvaddstr(line,30,"销 户 数:[    ]");
          dis_str(22,22,"请输入销户数.",0,0); 
          tmp_var=da->close_num;
          while((flag=acc_long( line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->close_num=(short)tmp_var;
          kk++;
          break;
   	case 59:
          mvaddstr(line,30,"贷方笔数:[    ]");
          dis_str(22,22,"请输入贷方笔数.",0,0); 
          tmp_var=da->det_num;
          while((flag=acc_long( line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->det_num=(short)tmp_var;
          kk++;
          break;
   	case 60:
          mvaddstr(line,30,"借方笔数:[    ]");
          dis_str(22,22,"请输入借方笔数.",0,0); 
          tmp_var=da->cre_num;
          while((flag=acc_long( line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->cre_num=(short)tmp_var;
          kk++;
          break;
   	case 61:
          mvaddstr(line,30,"余 户 数:[    ]");
          dis_str(22,22,"请输入余户数.",0,0); 
          tmp_var=da->rest_num;
          while((flag=acc_long( line,40,&tmp_var,4))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          da->rest_num=(short)tmp_var;
          kk++;
          break;
   	case 62:
          mvaddstr( line,30,"利 息 税:[              ]");
          dis_str(22,22,"请输入利息税.",0,0); 
          sprintf(tmp_str,"%-10.2lf",da->tax_int/100.00);
          mvprintw(line,40,"%s",tmp_str);
          refresh();
          while((flag=acc_amt(line,40,tmp_str,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          unpoint(tmp_str);
          da->tax_int=atol(tmp_str);
          mvprintw(line,40,"%-10.2lf",da->tax_int/100.00);
          refresh();
          kk++;
          break;
	case 63:
          mvaddstr(line,30,"证件类型:[      ]");
          dis_str(22,18,"请选择:0_身份证 1_电话 2_护照 3_临时证 4_其他");
          if (da->zj_kind[0]=='0')
            mvaddstr(line,40,"身份证]");
          else if (da->zj_kind[0]=='1')
            mvaddstr(line,40,"电话]");
          else if (da->zj_kind[0]=='2')
            mvaddstr(line,40,"护照]");
          else if (da->zj_kind[0]=='3')
            mvaddstr(line,40,"临时证]");
          else if (da->zj_kind[0]=='4')
            mvaddstr(line,40,"其他]");
          else
            dis_str(22,22,"输入证件类型错!",0,0); 
          refresh();
          while((flag=acc_str(line,40,da->zj_kind,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (da->zj_kind[0]=='0')
            mvaddstr(line,40,"身份证]");
          else if (da->zj_kind[0]=='1')
            mvaddstr(line,40,"电话]");
          else if (da->zj_kind[0]=='2')
            mvaddstr(line,40,"护照]");
          else if (da->zj_kind[0]=='3')
            mvaddstr(line,40,"临时证]");
          else if (da->zj_kind[0]=='4')
            mvaddstr(line,40,"其他]");
          else
          {
            dis_str(22,22,"输入证件类型错!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
	case 64:
          mvaddstr(line,30,"证件号码:[                     ]");
          dis_str(22,22,"请输入证件号码.");
          mvprintw(line,40,"%s",da->zj_num);
          refresh();
          while((flag=acc_str(line,40,da->zj_num,21))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          kk++;
          break;
   	case 65:
          mvaddstr(line,30,"开户帐号:[        ]");
          dis_str(22,22,"请输入存折开户帐号(不含校验位).",0,0); 
          mvprintw(line,40,"%-8ld]",da->new_acc);
          refresh();
          tmp_var=da->new_acc;
          while((flag=acc_long( line,40,&tmp_var,8))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->new_acc=tmp_var;

          if (da->new_acc==0)
          {
            dis_err("输入帐号错,请重新输入!"); 
            break;
          }
          chk=get_chk_no(da->open_br,da->new_acc);
          if (chk<0)
          {
            dis_err("计算校验位错!");  
            break;
          }
          mvprintw(line,40,"%8ld_%1d]",da->new_acc,chk);
          refresh();
          kk++;
          break;
   	case 66:
          mvaddstr(line,30,"卡    号:[                   ]");
          refresh();
          dis_str(22,22,"请输入开户卡号.",0,0); 
          mvprintw(line,40,"%s",da->card_acc);
          refresh();
          while((flag=acc_str(line,40,da->card_acc,19))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          _CutSpace(da->card_acc);
          if (strlen(da->card_acc)!= 19)
          {
            dis_err("卡号不是19位!"); 
            break;
          }	
          kk++;
          break;
   	case 67: 
          mvaddstr(line,30,"起始卡号:[                   ]");
          mvprintw(line,40,"%s",da->card_acc1);
          refresh();
          dis_str(22,22,"请输入起始卡号.",0,0); 
          strcpy(da->card_acc2,da->card_acc1);
          mvprintw(line,40,"%s",da->card_acc2);
          refresh();
          while((flag=acc_str(line,40,da->card_acc1,19))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          kk++;
          break;
   	case 68:
          mvaddstr(line,30,"终止卡号:[                   ]");
          refresh();
          dis_str(22,22,"请输入终止卡号.",0,0); 
          mvprintw(line,40,"%s",da->card_acc2);
          refresh();
          while((flag=acc_str(line,40,da->card_acc2,19))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          kk++;
          break;
   	case 69:
          mvaddstr(line,30,"查询方式:[0]        ");
          dis_str(22,22,"请输入查询方式:(1_按卡号 2_按金额 3_按姓名).",0,0); 
	        refresh();
          if (da->q_flag[0]=='1')
            mvaddstr(line,40,"按卡号]");
          else if (da->q_flag[0]=='2')
            mvaddstr(line,40,"按金额]");
          else if (da->q_flag[0]=='3')
            mvaddstr(line,40,"按姓名]");
          else
            mvaddstr(line,40,"未  知]");
          refresh();
          while((flag=acc_str(line,40,da->q_flag,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (da->q_flag[0]=='1')
            mvaddstr(line,40,"按卡号]");
          else if (da->q_flag[0]=='2')
            mvaddstr(line,40,"按金额]");
          else if (da->q_flag[0]=='3')
            mvaddstr(line,40,"按姓名]");
          else
          {
            dis_str(22,22,"输入查询方式错!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 70:
 		  mvprintw(22,22,"是否需要读存折磁条(y/n)? ");
  		refresh();
  		qq=getch();
  		if (qq!='y' && qq!='Y')
		  {
          mvaddstr( line,30,"帐    号:[         ]");
          dis_str(22,22,"请输入帐号(含1位校验位).",0,0); 
          mvprintw(line,40,"%-9ld]",da->account);
          refresh();
          tmp_var=da->account; 
          while((flag=acc_long( line,40,&tmp_var,9))==Err);

          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if (tmp_var!=0)
            da->account=tmp_var;
 
          if (da->account==99999999)
            return(-1);   
          if (da->account==0)
          {
            kk++;
            break;
          }
          old_chk=da->account%10;
          da->account=da->account/10;
          chk=get_chk_no(da->open_br,da->account);
          if (chk<0)
          {
            dis_str(22,22,"计算校验位错!",0,0);  
            getch();
            break;
          }
          if (chk!=old_chk)
          {
            dis_str(22,22,"输入帐号错!",0,0);  
            getch();
            break;
          }
          mvprintw(line,40,"%8ld_%1d]",da->account,chk);
          refresh();
		  }
		  else
		  {
				flag=read_msr(&str2,0);
				if (flag != 0 )
				{
	        dis_err("读磁条错误!"); 
	        break;
	      }
				strncpy(zh,&str2[0],9);
				zh[9]='\0';
				da->account=atol(zh);
	      chk=get_chk_no(da->open_br,da->account);
	      mvprintw(7,30,"帐    号:[%ld_%d]",da->account,chk);
      }
      kk++;
      break;
   	case 71:
 		  mvprintw(22,22,"是否需要读卡磁条(y/n)? ");
  		refresh();
  		qq=getch();
  		if (qq!='y' && qq!='Y')
		  {
          mvaddstr(line,30,"卡    号:[                   ]");
          refresh();
          dis_str(22,22,"请输入开户卡号.",0,0); 
          mvprintw(line,40,"%s",da->card_acc);
          refresh();
          while(flag=acc_str(line,40,da->card_acc,19)==Err);
          if(flag==KEY_F(1)||flag==ESC)
          {
            kk=99;
            break;
          }
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          kk++;
          break;
		  }
		  else
		  {
				flag=read_msr(&str2,1);
				if (flag != 0 )
				{
	        dis_err("读磁条错误!"); 
	        break;
	      }
				strncpy(da->card_acc,&str2[0],19);
				da->card_acc[19]='\0';
				
				  mvprintw(7,30,"卡    号:[%s]",da->card_acc);
      }
      kk++;
      break;
   	case 72:
	   dis_str(22,22,"请输入密码.",0,0); 
		 flag=STAR_pinpadReadPin(1,2,1,da->pwd_new1,60);
		 if (flag!=0) 
	  	{
	    		dis_err("输入密码出错!");
	       kk--;
	       break;
	  	}
	    kk++;
	    break;
	   case 73:
	          dis_str(22,22,"请再输入一次密码.",0,0); 
		 flag=STAR_pinpadReadPin(1,2,2,da->pwd_new2,60);
		 if (flag!=0) 
	  	{
	    		dis_err("输入密码出错!");
	        kk--;
	       	break;
	  	}
	  if (strcmp(da->pwd_new1,da->pwd_new2)!=0)
	  {
	    dis_err("两次输入密码不符!");
	            kk--;
	            break;
	  }
          kk++;
	  break;
    default:
          dis_str(22,22,"错误的操作码！",0,0);
	        getch(); 
          kk=99;
          break;
    }
    if (kk<0)
      kk=0;
  }
  if (kk==99)
    return(-1);
  else
    return(0);
}

init_in(in_da)
struct in_data *in_da;
{
  in_da->open_br=100;
  in_da->open_op=0;
  in_da->tr_br=100;
  in_da->tr_op=0;
  in_da->operator=0;
  strcpy(in_da->level,"1");
  in_da->ser_num=0;  
  in_da->detail_num=1;
  in_da->code=0;   
  strcpy(in_da->pwd_old,"000000");
  strcpy(in_da->pwd_new1,"000000");
  strcpy(in_da->pwd_new2,"000000");
  strcpy(in_da->correct,"A");
  strcpy(in_da->ltype,"S");
  strcpy(in_da->acc_type,"P");
  strcpy(in_da->ltype," ");
  strcpy(in_da->acc_flag,"A");
  strcpy(in_da->abstract,"0");
  strcpy(in_da->vou_type,"2");
  in_da->vou_num=0;
  in_da->account=0;
  in_da->new_acc=0; 
  in_da->que_acc=0; 
  in_da->account1=0;
  in_da->account2=0;
  in_da->item=0;    
  in_da->year=0;     
  strcpy(in_da->name," ");
  strcpy(in_da->vou_name," ");
  strcpy(in_da->customer," ");
  strcpy(in_da->open_name," ");
  strcpy(in_da->address," ");
  in_da->transfer=0; 
  in_da->other_item=0;
  in_da->other_br=100; 
  in_da->det_amount=0;
  in_da->cre_amount=0; 
  in_da->amount=0;
  in_da->balance=0;
  in_da->interest=0;
  in_da->keep_int=0;
  in_da->t_date=0;
  in_da->open_d=0;
  in_da->k_date=0; 
  in_da->term=0;    
  strcpy(in_da->int_flag," ");
  strcpy(in_da->q_flag,"1");
  in_da->sum_flag=1;    
  in_da->vou_flag=1;    
  in_da->sum_que=1;    
  in_da->item_kind=13;    
  in_da->home_que=0;    
  in_da->tr_code=1;
  in_da->ser_num1=0;    
  in_da->ser_num2=0;    
  strcpy(in_da->br_name," ");
  in_da->oper=0;    
  strcpy(in_da->item_name," ");
  in_da->sum_flag=0;    
  in_da->prt_ser=0;    
  strcpy(in_da->vou_code," ");
  strcpy(in_da->rate_type,"1");
  strcpy(in_da->zj_kind,"0");
  strcpy(in_da->zj_num," ");
  in_da->rate=0;    
  in_da->bal_flag=1;    
  in_da->month=1;    
  in_da->det_amt=0;    
  in_da->cre_amt=0;    
  in_da->det_num=0;    
  in_da->cre_num=0;    
  in_da->open_num=0;    
  in_da->close_num=0;    
  in_da->rest_num=0;    
  in_da->tax_int=0;    
  strcpy(in_da->card_acc," ");
  strcpy(in_da->card_acc1,"0");
  strcpy(in_da->card_acc2,"0");
}
