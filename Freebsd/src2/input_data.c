#include <stdio.h>
#include <string.h>
#include <curses.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"
/****************************************************************
 ����һ�������漰����:
   1.���������� case
   2.������init_in(in_da)���ӳ�ʼ��
   3.include/sh_table.h�� $struct in_data ˵��
 ���ٲ���:
   printf("n=%d\n\r", n);
   for (flag=1;flag<=num;flag++)
   {
     printf("i=%d,in[i]=%d\n\r", flag,in[flag]);
     getch();
   }

 1:��    ��   2:�� �� ��   3:ժ    Ҫ   4:��    ��   5:ƾ֤����
 6:��ѯ�ʺ�   7:�� �� ��   8:ȷ    ��   9:��λ����  10:��    ��
11:�� �� ��  12:ҵ������  13:��Ŀ����  14:��ϸ����  15:����Ա��
16:��    ��  17:��Ա����  18:��ԱȨ��  19:ԭ �� ��  20:�� �� ��
21:�� �� ��  22:��ʼ�ʺ�  23:��ֹ�ʺ�  24:�� Ŀ ��  25:��    ��
26:��������  27:ҵ������  28:��    ַ  29:�� �� ��  30:��    ��
31:ҵ���Ա  32:�ʻ�����  33:��    Ϣ  34:�� ֵ Ϣ  35:��ѯ��ʽ
36:���ʷ���  37:ƾ֤����  38:ҵ�����  39:��¼��־  40:��������
41:ҵ������  42:���׺�1   43:���׺�2   44:��    ��  45:�ලԱ��
46:ͳ�Ʊ�־  47:��ӡ˳��  48:ƾ֤����  49:ƾ֤����  50:��������
51:��    ��  52:ѡ������  53:��������  54:��    ��  55:�������
56:�跽���  57:�� �� ��  58:�� �� ��  59:��������  60:�跽����
61:��    ��  62:�� Ϣ ˰  63:֤������  64:֤������  65:�����ʺ�
66:��    ��  67:��ʼ����  68:��ֹ����  69:��ѯ��ʽ  70:ȡ�����ʺ�
71:ȡ����    72:��������  73:ȷ������
*****************************************************************/


extern MYSQL *conn;
extern char sql[];

//ȡ����(����֪������,��ɷֿ�,����ֻ�ܻ���)
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
    mvprintw(8,50,"����:[%s]",customer);
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
          mvaddstr( line,30,"��    ��:[         ]");
          dis_str(22,22,"�������ʺ�(��1λУ��λ).",0,0); 
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
            dis_str(22,22,"�����ʺŴ�!",0,0); 
            getch();
**/
          }
          old_chk=da->account%10;
          da->account=da->account/10;
          chk=get_chk_no(da->open_br,da->account);
          if (chk<0)
          {
            dis_str(22,22,"����У��λ��!",0,0);  
            getch();
            break;
          }
          if (chk!=old_chk)
          {
            dis_str(22,22,"�����ʺŴ�!",0,0);  
            getch();
            break;
          }

          mvprintw(line,40,"%8ld_%1d]",da->account,chk);
          refresh();

          //ȡ����,��ʾ�� (8,50)
          //get_acc_name(da->open_br,da->account);

          kk++;
          break;
   	case 2:
          mvaddstr( line,30,"�� �� ��:[              ]");
          dis_str(22,22,"��������.",0,0); 
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
          mvaddstr(line,30,"ժ    Ҫ:[0]        ");
          dis_str(22,22,"������ժҪ:(0�ֽ� 1ת�� 2ת�� 3���� 4����).",0,0); 
          if (da->abstract[0]=='0')
            mvaddstr(line,40,"�ֽ�]");
          else if (da->abstract[0]=='1')
            mvaddstr(line,40,"ת��]  ");
          else if (da->abstract[0]=='2')
            mvaddstr(line,40,"ת��]  ");
          else if (da->abstract[0]=='3')
            mvaddstr(line,40,"����]  ");
          else if (da->abstract[0]=='4')
            mvaddstr(line,40,"����]  ");
          else
            mvaddstr(line,40,"δ֪]  ");
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
            mvaddstr(line,40,"�ֽ�]");
          else if (da->abstract[0]=='1')
            mvaddstr(line,40,"ת��]  ");
          else if (da->abstract[0]=='2')
            mvaddstr(line,40,"ת��]  ");
          else if (da->abstract[0]=='3')
            mvaddstr(line,40,"����]  ");
          else if (da->abstract[0]=='4')
            mvaddstr(line,40,"����]  ");
          else
          {
            dis_str(22,22,"����ժҪ��!",0,0); 
            break;
          }
          refresh();
          refresh();
          kk++;
          break;
   	case 4:
          mvaddstr(line,30,"��    ��:[    ]        ");
          dis_str(22,22,"�����봢��(1 ����  2 ���� ).",0,0); 
          strcpy(da->ltype,"1");
          if (da->ltype[0]=='2')
            mvaddstr(line,40,"����]");
          else if (da->ltype[0]=='1')
            mvaddstr(line,40,"����]");
          else
            mvaddstr(line,40,"δ֪]");
	
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
            mvaddstr(line,40,"����]");
          else if (da->ltype[0]=='2')
            mvaddstr(line,40,"����");
          else
          {
            dis_str(22,22,"���봢�ִ�!",0,0); 
            break;
          }
          kk++;
          break;
   	case 5:
          mvaddstr(line,30,"ƾ֤����:[ ]        ");
          dis_str(22,22,"ƾ֤����(2_���� 3_�� )",0,0);
          if (da->vou_type[0]=='2')
            mvaddstr(line,40,"����]");
          else if (da->vou_type[0]=='3')
            mvaddstr(line,40,"��]  ");
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
            mvaddstr(line,40,"����]");
          else if (da->vou_type[0]=='3')
            mvaddstr(line,40,"��]  ");
          else
          {
            dis_str(22,22,"����ƾ֤���ʹ�!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 6:
          mvaddstr(line,30,"��    ��:[        ]");
          dis_str(22,22,"�������ʺ�(����У��λ).",0,0); 
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
            dis_str(22,22,"�����ʺŴ�!",0,0); 
            getch();
            break;
          }
          kk++;
          break;
   	case 7:
          mvaddstr(line,30,"�»��ʺ�:[         ]");
          dis_str(22,22,"�������¿������ʺ�(��1λУ��λ).",0,0); 
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
            dis_str(22,22,"�����ʺŴ�!",0,0); 
            getch();
            break;
          }
          old_chk=da->new_acc%10;
          da->new_acc=da->new_acc/10;
          chk=get_chk_no(da->open_br,da->new_acc);
          if (chk<0)
          {
            dis_str(22,22,"����У��λ��!",0,0);  
            getch();
            break;
          }
          if (chk!=old_chk)
          {
            dis_str(22,22,"�����ʺŴ�!",0,0);  
            getch();
            break;
          }
          mvprintw(line,40,"%8ld_%1d]",da->new_acc,chk);
          refresh();
          kk++;
          break;
        case 8:
          mvaddstr(line,30,"ȷ    ��:[ ]");
          dis_str(22,22,"������ y �� n.",0,0); 
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
          mvaddstr(line,30,"��λ����:[                    ]");
          dis_str(22,22,"�����뵥λ����.",0,0); 
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
          mvaddstr(line,30,"��    ��:[              ]");
          dis_str(22,22,"�������ʻ����.",0,0); 
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
          mvaddstr(line,30,"�� �� ��:[    ]");
          dis_str(22,22,"������ԭҵ���׺�.",0,0); 
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
            dis_str(22,22,"���뽻�׺Ŵ�!",0,0); 
            break;
          }
          mvprintw(line,40,"%-4d",da->ser_num);
          refresh();
         
          kk++;
          break;
        case 12:
          mvaddstr(line,30,"ҵ������:[          ]");
          dis_str(22,22,"������ԭҵ������(��������/����/����).",0,0); 
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
            dis_str(22,22,"�������ڴ�!",0,0); 
            break;
          }
          kk++;
          break;
   	case 13:
          mvaddstr(line,30,"��Ŀ����:[                    ]");
          dis_str(22,22,"�������Ŀ����.",0,0); 
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
          mvaddstr(line,30,"��ϸ����:[      ]");
          dis_str(22,22,"������ԭҵ����ϸ����.",0,0); 
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
          mvaddstr(line,30,"����Ա��:[  ]");
          dis_str(22,22,"���������Ա��.",0,0); 
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
          mvaddstr(line,30,"��    ��:[            ]");
          dis_str(22,22,"����������ʻ�������.",0,0); 
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
          mvaddstr(line,30,"��Ա����:[        ]");
          dis_str(22,22,"�������Ա����.",0,0); 
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
          mvaddstr(line,30,"��ԱȨ��:[ ]");
          dis_str(22,22,"������Ȩ��( 1����Ա 7����Ա 4������).",0,0); 
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
            mvaddstr(line,40,"����Ա]");
          else if (da->level[0]=='7')
            mvaddstr(line,40,"����Ա]");
          else if (da->level[0]=='4')
            mvaddstr(line,40,"������]");
          else
          {
            dis_str(22,22,"�����!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 19:
          mvaddstr(line,30,"ԭ �� ��:[      ]");
          dis_str(22,22,"���������Աԭ����.",0,0); 
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
          mvaddstr(line,30,"�� �� ��:[      ]");
          dis_str(22,22,"���������Ա������.",0,0); 
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
          mvaddstr(line,30,"�� �� ��:[      ]");
          dis_str(22,22,"���������Ա������.",0,0); 
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
          mvaddstr( line,30,"��ʼ�ʺ�:[        ]");
          dis_str(22,22,"��������ʼ�ʺ�(������ĩλ����).",0,0); 
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
          mvaddstr( line,30,"��ֹ�ʺ�:[        ]");
          dis_str(22,22,"��������ֹ�ʺ�(������ĩλ��.",0,0); 
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
          mvaddstr( line,30,"�� Ŀ ��:[      ]");
          mvprintw(line,40,"%-6ld",da->item);
          refresh();
          tmp_var=da->item;
          dis_str(22,22,"�������Ŀ��.",0,0); 
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
            dis_str(22,22,"�����Ŀ�Ŵ�!",0,0); 
            break;
          }
          mvprintw(line,40,"%-6ld",da->item);
          refresh();
          kk++;
          break;
   	case 25:
          mvaddstr( line,30,"��    ��:[    ]");
          tmp_var=da->year;
          mvprintw(line,40,"%-4d",da->year);
          refresh();
          dis_str(22,22,"���������(YYYY).",0,0); 
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
          mvaddstr(line,30,"��������:[   ]");
          dis_str(22,22,"�����뱾ҵ�񿪻�����.",0,0); 
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
          mvaddstr(line,30,"ҵ������:[   ]");
          dis_str(22,22,"������ҵ������.",0,0); 
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
          mvaddstr(line,30,"��    ַ:[                              ]");
          dis_str(22,22,"�����뿪���˵ĵ�ַ.",0,0); 
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
          mvaddstr(line,30,"��������:[          ]");
          dis_str(22,22,"�����뱾����������(��������/����/����).",0,0); 

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
            dis_str(22,22,"�������ڴ�!",0,0); 
            break;
          }
          kk++;
          break;
   	case 30:
          mvaddstr(line,30,"��    ��:[  ]");
          dis_str(22,22,"���������(����Ϊ��λ,����Ϊ0).",0,0); 
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
            dis_str(22,22,"������ڴ�!",0,0); 
            break;
          }
          mvprintw(line,40,"%-2d",da->term);
          refresh();
          kk++;
          break;
   	case 31:
          mvaddstr(line,30,"ҵ���Ա:[  ]");
          dis_str(22,22,"������ҵ�����Ա��.",0,0); 
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
          mvaddstr(line,30,"�ʻ�����:[ ]        ");
          dis_str(22,22,"�������ʻ�����(1 ���� 2 ���� 3 ��).",0,0); 
          refresh();
          if (da->acc_type[0]=='D'||da->acc_type[0]=='2')
            mvprintw(line,40,"����]      ");
          else if (da->acc_type[0]=='H'|| da->acc_type[0]=='1')
            mvprintw(line,40,"���ڴ���]  ");
          else if (da->acc_type[0]=='K'|| da->acc_type[0]=='3')
            mvprintw(line,40,"��]        ");
          else
            mvprintw(line,40,"δ    ֪]  ");
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
            mvprintw(line,40,"����]      ");
            refresh();
            strcpy(da->acc_type,"D");
          }
          else if (da->acc_type[0]=='H'|| da->acc_type[0]=='1')
          {
            mvprintw(line,40,"���ڴ���]  ");
            refresh();
            strcpy(da->acc_type,"H");
          }
          else if (da->acc_type[0]=='K'|| da->acc_type[0]=='3')
          {
            mvprintw(line,40,"��]        ");
            refresh();
            strcpy(da->acc_type,"K");
          }
          else
          {
            dis_str(22,22,"�����ʻ����ʴ�!",0,0); 
            refresh();
          }
          kk++;
          break;
   	case 33:
          mvaddstr( line,30,"��    Ϣ:[              ]");
          dis_str(22,22,"������������Ϣ����Ϣ����.",0,0); 
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
          mvaddstr( line,30,"��ֵ��Ϣ:[              ]");
          dis_str(22,22,"������������ֵ��Ϣ.",0,0); 
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
          mvaddstr(line,30,"��ѯ��ʽ:[0]        ");
          dis_str(22,22,"�������ʻ���ѯ��ʽ:(1_���ʺ� 2_����� 3_������).",0,0); 
          if (da->q_flag[0]=='1')
            mvaddstr(line,40,"���ʺ�]");
          else if (da->q_flag[0]=='2')
            mvaddstr(line,40,"�����]");
          else if (da->q_flag[0]=='3')
            mvaddstr(line,40,"������]");
          else
            mvaddstr(line,40,"δ  ֪]");
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
            mvaddstr(line,40,"���ʺ�]");
          else if (da->q_flag[0]=='2')
            mvaddstr(line,40,"�����]");
          else if (da->q_flag[0]=='3')
            mvaddstr(line,40,"������]");
          else
          {
            dis_str(22,22,"�����ѯ��ʽ��!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 36:
          mvaddstr(line,30,"���ʷ���:[    ]        ");
          dis_str(22,22,"������������ʵķ���:(1�跽 2����).",0,0); 
          if (da->sum_flag==1)
            mvaddstr(line,40,"�跽]");
          else if (da->sum_flag==2)
            mvaddstr(line,40,"����]  ");
          else
            mvaddstr(line,40,"δ֪]  ");
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
            mvaddstr(line,40,"�跽]");
          else if (da->sum_flag==2)
            mvaddstr(line,40,"����]  ");
          else
          {
            dis_str(22,22,"�������ʷ����!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 37:
          mvaddstr(line,30,"ƾ֤����:[    ]        ");
          dis_str(22,22,"���������ƾ֤����:(1��� 2ʹ�� 3���� 4�Ͻ� 5����).",0,0); 
          if (da->vou_flag==1)
            mvaddstr(line,40,"���]");
          else if (da->vou_flag==2)
            mvaddstr(line,40,"ʹ��]  ");
          else if (da->vou_flag==3)
            mvaddstr(line,40,"����]  ");
          else if (da->vou_flag==4)
            mvaddstr(line,40,"�Ͻ�]  ");
          else if (da->vou_flag==5)
            mvaddstr(line,40,"����]  ");
          else
            mvaddstr(line,40,"δ֪]  ");
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
            mvaddstr(line,40,"���]");
          else if (da->vou_flag==2)
            mvaddstr(line,40,"ʹ��]  ");
          else if (da->vou_flag==3)
            mvaddstr(line,40,"����]  ");
          else if (da->vou_flag==4)
            mvaddstr(line,40,"�Ͻ�]  ");
          else if (da->vou_flag==5)
            mvaddstr(line,40,"����]  ");
          else
          {
            dis_str(22,22,"����ƾ֤�����!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 38:
          mvaddstr(line,30,"ҵ�����:[    ]        ");
          dis_str(22,22,"�������������Ŀ��ҵ�����.",0,0); 
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
          mvaddstr(line,30,"��¼��־:[   ]        ");
          dis_str(22,12,"������:0_ȥ�� 1-12ĳ�� 13_���� 14_���� 101-131_Ϊ1-31ĳ��.",0,0); 
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
          mvaddstr(line,30,"��������:[    ]        ");
          dis_str(22,22,"�������������Ŀ�Ŀ�������������.",0,0); 
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
          mvaddstr(line,30,"ҵ������:[      ]");
          dis_str(22,12,"������ҵ������:(1���� 2���� 3���� 4֧ȡ 5���� 6���� 7������ 8��ƾ֤).",0,0); 
          if (da->tr_code==1)
            mvaddstr(line,40,"����]");
          else if (da->tr_code==2)
            mvaddstr(line,40,"����]  ");
          else if (da->tr_code==3)
            mvaddstr(line,40,"����]  ");
          else if (da->tr_code==4)
            mvaddstr(line,40,"֧ȡ]  ");
          else if (da->tr_code==5)
            mvaddstr(line,40,"����]  ");
          else if (da->tr_code==6)
            mvaddstr(line,40,"����]  ");
          else if (da->tr_code==7)
            mvaddstr(line,40,"������]  ");
          else if (da->tr_code==8)
            mvaddstr(line,40,"��ƾ֤]  ");
          else
            mvaddstr(line,40,"δ֪]  ");
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
            mvaddstr(line,40,"����]");
          else if (da->tr_code==2)
            mvaddstr(line,40,"����]  ");
          else if (da->tr_code==3)
            mvaddstr(line,40,"����]  ");
          else if (da->tr_code==4)
            mvaddstr(line,40,"֧ȡ]  ");
          else if (da->tr_code==5)
            mvaddstr(line,40,"����]  ");
          else if (da->tr_code==6)
            mvaddstr(line,40,"����]  ");
          else if (da->tr_code==7)
            mvaddstr(line,40,"������]  ");
          else if (da->tr_code==8)
            mvaddstr(line,40,"��ƾ֤]  ");
          else
          {
            dis_str(22,22,"����ҵ�����ʹ�!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 42:
          mvaddstr(line,30,"��ʼ����:[    ]");
          dis_str(22,22,"���������ѯ����ʼ���׺�.",0,0); 
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
            dis_str(22,22,"���뽻�׺Ŵ�!",0,0); 
            break;
          }
          mvprintw(line,40,"%-4d",da->ser_num1);
          refresh();
          kk++;
          break;
   	case 43:
          mvaddstr(line,30,"��ֹ����:[    ]");
          dis_str(22,22,"���������ѯ����ֹ���׺�.",0,0); 
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
            dis_str(22,22,"���뽻�׺Ŵ�!",0,0); 
            break;
          }
          mvprintw(line,40,"%-4d",da->ser_num2);
          refresh();
         
          kk++;
          break;
   	case 44:
          mvaddstr(line,30,"��    ��:[          ]");
          dis_str(22,22,"����������.",0,0); 
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
          mvaddstr(line,30,"�ලԱ��:[  ]");
          dis_str(22,22,"������ලԱ��.",0,0); 
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
          mvaddstr( line,30,"ͳ�Ʊ�־:[      ]");
          mvprintw(line,40,"%-6ld",da->sum_flag);
          refresh();
          tmp_var=da->sum_flag;
          dis_str(22,22,"������ͳ�Ʊ�־.",0,0); 
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
          mvaddstr( line,30,"��ӡ˳��:[   ]");
          mvprintw(line,40,"%-3ld",da->prt_ser);
          refresh();
          tmp_var=da->prt_ser;
          dis_str(22,22,"�������ӡ˳��.",0,0); 
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
          mvaddstr(line,30,"ƾ֤����:[          ]");
          dis_str(22,22,"������ƾ֤����.",0,0); 
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
          mvaddstr(line,30,"ƾ֤����:[  ]");
          dis_str(22,22,"������ƾ֤����(��λ).",0,0); 
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
          mvaddstr(line,30,"��������:[ ]        ");
          dis_str(22,22,"�����봢��(1 ���� 2 ����).",0,0); 
          if (da->rate_type[0]=='1')
            mvaddstr(line,40,"����]");
          else if (da->rate_type[0]=='2')
            mvaddstr(line,40,"����]");
          else
            mvaddstr(line,40,"δ֪]");
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
            mvaddstr(line,40,"����]");
          else if (da->rate_type[0]=='2')
            mvaddstr(line,40,"����]");
          else
          {
            dis_str(22,22,"�����������ʹ�!",0,0); 
            break;
          }
    refresh();
          kk++;
          break;
   	case 51:
          mvaddstr( line,30,"��    ��:[        ]");
          mvprintw(line,40,"%-8ld",da->rate);
          refresh();
          tmp_var=da->rate;
          dis_str(22,22,"����������(������*1000000)",0,0); 
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
          mvaddstr(line,30,"��    ��:[   ]");
          dis_str(22,22,"����������(999_ȫ���� ABC_ָ���� 888_ȫ��).",0,0); 
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
          mvaddstr(line,30,"��������:[   ]");
          dis_str(22,22,"�����뱨������(1_�ձ� 2_�±� 3_�걨).",0,0); 
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
          mvaddstr( line,30,"��    ��:[    ]");
          tmp_var=da->month;
          mvprintw(line,40,"%-2d",da->month);
          refresh();
          dis_str(22,22,"�������·�(MM).",0,0); 
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
          mvaddstr( line,30,"�������:[              ]");
          dis_str(22,22,"������������.",0,0); 
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
          mvaddstr( line,30,"�跽���:[              ]");
          dis_str(22,22,"������跽���.",0,0); 
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
          mvaddstr(line,30,"�� �� ��:[    ]");
          dis_str(22,22,"�����뿪����.",0,0); 
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
          mvaddstr(line,30,"�� �� ��:[    ]");
          dis_str(22,22,"������������.",0,0); 
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
          mvaddstr(line,30,"��������:[    ]");
          dis_str(22,22,"�������������.",0,0); 
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
          mvaddstr(line,30,"�跽����:[    ]");
          dis_str(22,22,"������跽����.",0,0); 
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
          mvaddstr(line,30,"�� �� ��:[    ]");
          dis_str(22,22,"�������໧��.",0,0); 
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
          mvaddstr( line,30,"�� Ϣ ˰:[              ]");
          dis_str(22,22,"��������Ϣ˰.",0,0); 
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
          mvaddstr(line,30,"֤������:[      ]");
          dis_str(22,18,"��ѡ��:0_���֤ 1_�绰 2_���� 3_��ʱ֤ 4_����");
          if (da->zj_kind[0]=='0')
            mvaddstr(line,40,"���֤]");
          else if (da->zj_kind[0]=='1')
            mvaddstr(line,40,"�绰]");
          else if (da->zj_kind[0]=='2')
            mvaddstr(line,40,"����]");
          else if (da->zj_kind[0]=='3')
            mvaddstr(line,40,"��ʱ֤]");
          else if (da->zj_kind[0]=='4')
            mvaddstr(line,40,"����]");
          else
            dis_str(22,22,"����֤�����ʹ�!",0,0); 
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
            mvaddstr(line,40,"���֤]");
          else if (da->zj_kind[0]=='1')
            mvaddstr(line,40,"�绰]");
          else if (da->zj_kind[0]=='2')
            mvaddstr(line,40,"����]");
          else if (da->zj_kind[0]=='3')
            mvaddstr(line,40,"��ʱ֤]");
          else if (da->zj_kind[0]=='4')
            mvaddstr(line,40,"����]");
          else
          {
            dis_str(22,22,"����֤�����ʹ�!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
	case 64:
          mvaddstr(line,30,"֤������:[                     ]");
          dis_str(22,22,"������֤������.");
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
          mvaddstr(line,30,"�����ʺ�:[        ]");
          dis_str(22,22,"��������ۿ����ʺ�(����У��λ).",0,0); 
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
            dis_err("�����ʺŴ�,����������!"); 
            break;
          }
          chk=get_chk_no(da->open_br,da->new_acc);
          if (chk<0)
          {
            dis_err("����У��λ��!");  
            break;
          }
          mvprintw(line,40,"%8ld_%1d]",da->new_acc,chk);
          refresh();
          kk++;
          break;
   	case 66:
          mvaddstr(line,30,"��    ��:[                   ]");
          refresh();
          dis_str(22,22,"�����뿪������.",0,0); 
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
            dis_err("���Ų���19λ!"); 
            break;
          }	
          kk++;
          break;
   	case 67: 
          mvaddstr(line,30,"��ʼ����:[                   ]");
          mvprintw(line,40,"%s",da->card_acc1);
          refresh();
          dis_str(22,22,"��������ʼ����.",0,0); 
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
          mvaddstr(line,30,"��ֹ����:[                   ]");
          refresh();
          dis_str(22,22,"��������ֹ����.",0,0); 
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
          mvaddstr(line,30,"��ѯ��ʽ:[0]        ");
          dis_str(22,22,"�������ѯ��ʽ:(1_������ 2_����� 3_������).",0,0); 
	        refresh();
          if (da->q_flag[0]=='1')
            mvaddstr(line,40,"������]");
          else if (da->q_flag[0]=='2')
            mvaddstr(line,40,"�����]");
          else if (da->q_flag[0]=='3')
            mvaddstr(line,40,"������]");
          else
            mvaddstr(line,40,"δ  ֪]");
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
            mvaddstr(line,40,"������]");
          else if (da->q_flag[0]=='2')
            mvaddstr(line,40,"�����]");
          else if (da->q_flag[0]=='3')
            mvaddstr(line,40,"������]");
          else
          {
            dis_str(22,22,"�����ѯ��ʽ��!",0,0); 
            break;
          }
          refresh();
          kk++;
          break;
   	case 70:
 		  mvprintw(22,22,"�Ƿ���Ҫ�����۴���(y/n)? ");
  		refresh();
  		qq=getch();
  		if (qq!='y' && qq!='Y')
		  {
          mvaddstr( line,30,"��    ��:[         ]");
          dis_str(22,22,"�������ʺ�(��1λУ��λ).",0,0); 
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
            dis_str(22,22,"����У��λ��!",0,0);  
            getch();
            break;
          }
          if (chk!=old_chk)
          {
            dis_str(22,22,"�����ʺŴ�!",0,0);  
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
	        dis_err("����������!"); 
	        break;
	      }
				strncpy(zh,&str2[0],9);
				zh[9]='\0';
				da->account=atol(zh);
	      chk=get_chk_no(da->open_br,da->account);
	      mvprintw(7,30,"��    ��:[%ld_%d]",da->account,chk);
      }
      kk++;
      break;
   	case 71:
 		  mvprintw(22,22,"�Ƿ���Ҫ��������(y/n)? ");
  		refresh();
  		qq=getch();
  		if (qq!='y' && qq!='Y')
		  {
          mvaddstr(line,30,"��    ��:[                   ]");
          refresh();
          dis_str(22,22,"�����뿪������.",0,0); 
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
	        dis_err("����������!"); 
	        break;
	      }
				strncpy(da->card_acc,&str2[0],19);
				da->card_acc[19]='\0';
				
				  mvprintw(7,30,"��    ��:[%s]",da->card_acc);
      }
      kk++;
      break;
   	case 72:
	   dis_str(22,22,"����������.",0,0); 
		 flag=STAR_pinpadReadPin(1,2,1,da->pwd_new1,60);
		 if (flag!=0) 
	  	{
	    		dis_err("�����������!");
	       kk--;
	       break;
	  	}
	    kk++;
	    break;
	   case 73:
	          dis_str(22,22,"��������һ������.",0,0); 
		 flag=STAR_pinpadReadPin(1,2,2,da->pwd_new2,60);
		 if (flag!=0) 
	  	{
	    		dis_err("�����������!");
	        kk--;
	       	break;
	  	}
	  if (strcmp(da->pwd_new1,da->pwd_new2)!=0)
	  {
	    dis_err("�����������벻��!");
	            kk--;
	            break;
	  }
          kk++;
	  break;
    default:
          dis_str(22,22,"����Ĳ����룡",0,0);
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
