
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>

#include <time.h>
#include <signal.h>

#include "../include/apt.h"


rtoday(long *pl)
{
  time_t tt;
	time(&tt);
	*pl=tt;
}

rfmtdate(long ld,char *fm,char *dest)
{
	char fm2[26]="";
  struct tm *stm;
  time_t tt=ld;
  stm=localtime(&tt);
  int i=0,len=0;
  while(fm[i])
  {
  	if(fm[i+1]&&fm[i+2]&&fm[i+3]
  		&&fm[i]=='y'&&fm[i+1]=='y'&&fm[i+2]=='y'&&fm[i+3]=='y')
  	{
  		strcat(fm2,"%Y");
  		i+=4;
  	}
  	else if(fm[i+1]&&fm[i]=='y'&&fm[i+1]=='y')
  	{
  		strcat(fm2,"%y");
  		i+=2;
  	}
  	else if(fm[i+1]&&fm[i]=='m'&&fm[i+1]=='m')
  	{
  		strcat(fm2,"%m");
  		i+=2;
  	}
  	else if(fm[i+1]&&fm[i]=='d'&&fm[i+1]=='d')
  	{
  		strcat(fm2,"%d");//0d?
  		i+=2;
  	}
  	else
  	{
  		len=strlen(fm2);
  		fm2[len]=fm[i];
  		fm2[len+1]='\0';
  		i++;
  	}
  }
  
	strftime(dest,25,fm2,stm);
}

rmdyjul(short ymd[],long *pl)
{
  struct tm stm;
  time_t tt;
  memset(&stm,0,sizeof(stm));
  stm.tm_mon=ymd[0]-1;
  stm.tm_mday=ymd[1];
  stm.tm_year=ymd[2]-1900;
  tt=mktime(&stm);
	*pl=tt; 
}

rjulmdy(long ld,short ymd[])
{
  struct tm *stm;
  time_t tt=ld;
  stm=localtime(&tt);
	
	ymd[0]=stm->tm_mon+1;
	ymd[1]=stm->tm_mday;
	ymd[2]=stm->tm_year+1900;
}


note(str)
char *str;
{
 ;
}

get_date(x,y,flag,str)
short x,y,flag;
char *str;
{
  struct tm *stm;
  //long ttt,gtime;
  time_t ttt;
  long sysdate,systime;
  char ymdhms[30];

  //gtime=time(&ttt);
  time(&ttt);
  stm=localtime(&ttt);

  systime=0;
  
  //?
  if ( stm->tm_year > 50)
    stm->tm_year+=1900;
  else
    stm->tm_year+=2000;

  sysdate+=(long)stm->tm_year*10000;
  sysdate+=(stm->tm_mon+1)*100;
  sysdate+=stm->tm_mday;

  systime+=(long)stm->tm_hour*10000;
  systime+=stm->tm_min*100;
  systime+=stm->tm_sec;

  move(x,y);
  if (flag==1)
    sprintf(ymdhms,"系统日:%04d年%02d月%02d日%02d时%02d分",stm->tm_year,stm->tm_mon+1,stm->tm_mday,stm->tm_hour,stm->tm_min);
  else
    sprintf(ymdhms,"系统日:%08ld%06ld",sysdate,systime);
  addstr(ymdhms);
  refresh();
}

get_mon_day(year,month)
int year,month;
{
  switch(month)
  {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      return(31);
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      return(30);
      break;
    case 2:
      if ( year%4==0 && year%100!=0)
        return(29);
      else
        return(28);
      break;
    default:
      return(-1);
  }    
}

dis_str(x,y,s,l,flag)
int  x,y,l,flag;
char *s;
{
    int  i,e=78,k;
    char *p;

    if (l==0)
      l=strlen(s);
    if (y==99)
      y=(80-l)/2;
    k=76-y;
    clear_line(x,12,77);
    move(x,y);
    //addstr("注意:");
    p=s;
    for(i=7;i<k;i++)
    {
        move(x,y+i);
        addch(SPACE);
    }
    refresh();
    if (flag==1)
	    standout();
    move(x,y+7);
    addstr(s);
    refresh();
    if (flag==1)
	    standend();
}

acc_num(x,y,l,l1,str)
short x,y;
short l,l1;
char *str;
{
  short i,l0,point,l_point;
  char c;

  note("l0 实际输入长度!");
  l0=0;
  note("l_point 实际输入小数长度!");
  l_point=0;
  note("point 输入小数点标志!");
  point=0;

  mvprintw(x,y,"%s",str);
  refresh();
  for (i=0;i<l;i++)
  {
    while(1)
    { 
      move(x,y+i);
      c=getch();

      note("退格键的处理:将前一字符删除,光标退一格!");
      if (c==BS)
      {
        note("开始不能退格");
        if (i==0)
        {
          beep();
          continue;
        }
        move(x,y+i-1);
        addstr(" ");
        i=i-1;
        l0=l0-1;
        continue;
      }

      note("输入小数点的处理!");
      if (c=='.' && point==1)
      {
        note("已输入了小数点!");
        beep();
        continue;
      }
      else if (c=='.' && point==0)
        point=1;

      note("全长-小数长-2(小数位)=整数长,提示该输小数部分!");
      if (i>(l-l1-2) && point==0 && c!=ENTER)
      {
        beep();
        beep();
        continue;
      }
       
      note("输入内容为:数字/回车/小数!");
      if ( !isdigit(c) && c!=ENTER && c!='.')
      {
        beep();
        beep();
        continue;
      }
      note("统计小数输入长度!");
      if(point==1)
       l_point++;
      break;
    } 
    addch(c);
    refresh();
    if  (c==ENTER)
      break;
    str[l0]=c;
    l0=l0+1;
    note("控制小数输入长度!");
    if (l_point==l1+1)
      break;
  }
  str[l0]='\0';
}

str_left(str)
char *str;
{
	int  i,j,l;
	char *ps,*pt;

	ps=str;
	pt=str;
	i=strlen(str);
	for(j=0,l=0;j<i;j++,ps++)
	{
		if(isspace((*ps)&0x7f))
		{	l++;
			continue;
		}
		*pt=*ps;
		pt++;
	}
	for(;l>=1;l--,pt++)
		*pt=' ';
	*pt=0;
}


acc1_str(x,y,str,l)
short x,y;
short l;
char *str;
{
  short i,l0;
  char c;

  note("l0 实际输入长度!");
  l0=0;

  mvprintw(x,y,"%s",str);
  refresh();
  for (i=0;i<l;i++)
  {
    while(1)
    { 
      move(x,y+i);
      c=getch();

      note("退格键的处理:将前一字符删除,光标退一格!");
      if (c==BS)
      {
        note("开始不能退格");
        if (i==0)
        {
          beep();
          continue;
        }
        move(x,y+i-1);
        addstr(" ");
        i=i-1;
        l0=l0-1;
        continue;
      }

      note("输入内容为:数字/回车!");
      if ( !isascii(c) && c!=ENTER )
      {
        beep();
        beep();
        continue;
      }
      break;
    } 
    addch(c);
    refresh();
    if  (c==ENTER)
      break;
    str[l0]=c;
    l0=l0+1;
  }
  str[l0]='\0';
}


acc_str(i,j,str,l)
int i,j,l;
char *str;
{
  short c,m,x;
  char  *p,s[3];

  p=str;
  s[2]=0;
  for(m=0;m<l;m++)
  {
    while(1)
    {
      move(i,j+m);
      refresh();
      c=kk();
      if(c==CTRL_Q) 
      {
                touchwin(stdscr);
                wrefresh(stdscr);
                touchwin(curscr);
                wrefresh(curscr);
        continue;
      }
      note("F1,←,↑,→,↓ ENter返回控制符");
      if(c==ESC||c==KEY_F(1)||c==KEY_LEFT||c==KEY_UP||c==ENTER||c==KEY_DOWN||c==KEY_RIGHT)
      {
        move(i,j);
        printw("%s",str);
        refresh();
        if(c==ENTER) 
          return(Ok);
        else
          return(c); 
      }
      if(c>0400) 
        continue;
      if(c==BS) 
      {
        if(m==0)
        {
          for(;m<l;m++)
          {
            mvaddch(i,j+m,SPACE);
            refresh();
          }
          *p=0;
          m=-1;
        }
        else
        {
          p--;
          if((*p)&0x80) 
          {
            p--;
            m-=3;
            mvaddstr(i,j+m+1,"  ");
          }
          else
          { 
            m-=2;
            mvaddch(i,j+m+1,SPACE);
          }
          refresh();
          *p=0;
        }
        break;
      }
      else  if(!iscntrl(c&0x7f))
      { 
        if(m==0) 
        {
          for(x=0;x<l;x++)
            mvaddch(i,j+x,SPACE);
          refresh();
        }
        *p=c;
        p++;
        *p=0;
        move(i,j+m);
        refresh();
        if(c&0x80) 
        {
          s[0]=c;
          c=kk();
          s[1]=c;
          addstr(s);
          *p=c;
          p++;
          *p=0;
          m++;
        }
        else 
          addch(c);
        refresh();
        break;
      }
    }
  }
  *p=0;
  return(Ok);
}
      
go_left(str)
char *str;
{
  int  i,j,l;
  char *ps,*pt;

  ps=str;
  pt=str;
  i=strlen(str);
  for(j=0,l=0;j<i;j++,ps++)
  {
    if(isspace((*ps)&0x7f))
    {
      l++;
      continue;
    }
    *pt=*ps;
    pt++;
  }
  for(;l>=1;l--,pt++)
    *pt=' ';
  *pt=0;
  return(Ok);
}

rm_left(str)
char *str;
{
  int  i,j,l;
  char *ps,*pt;
  l=0;
  i=strlen(str);
  note("找到第一个非空格的位置l!");
  for(j=0;j<i;j++)
  {
    l=j;
    if(isspace(str[j]))
      continue;
    else
      break;
  }
  note("将第一个非空格后的所有字符复制!");
  for(j=l;j<i;j++)
  {
    str[j-l]=str[j];
  }
  str[i-l]='\0';
  return(Ok);
}

rm_blan(str)
char *str;
{
  int  i,j,l;
  char *ps,*pt;

  ps=str;
  pt=str;
  i=strlen(str);
  for(j=0,l=0;j<i;j++,ps++)
  {
    if(isspace((*ps)&0x7f))
    {
      l++;
      continue;
    }
    *pt=*ps;
    pt++;
  }
  *pt=0;
  return(Ok);
}

move_blan(str)
char *str;
{
  char *name2;
  short i,j,k,l;
  l=strlen(str);
  for(i=0;i<l;i++)
  {
    if (str[i]!=' ')
    {
      k=0;
      for(j=i;j<l;j++)
      {
        name2[k]=str[j];
        k=k+1;
      }
      for(j=k;j<l;j++)
        name2[j]=' ';
      break;
    }
  }
  for(j=0;j<l;j++)
   str[j]=name2[j];
  return(Ok);
}
go_right(str) 
char *str;
{
  int i,l;
  char *ps,*pt;

  ps=str;
  pt=str;
  i=strlen(str);
  pt+=i;
  ps+=i;
  for(l=i;i>=0;i--,ps--)
  {
    if(isspace((*ps)&0x7f))
    {
      l--;
      continue;
    }
    *pt=*ps;
    pt--;
  }
  for(;l>=0;l--,pt--)
    *pt=' ';
  return(Ok);
}

rm_right(str) 
char *str;
{
  int k,l,i;
  
  l=strlen(str);
  i=0;
  note("找到最后空格中第一个空格的位置l!");
  for(k=0;k<l;k++)
  {
    if(!(isspace(str[k])) && str[k]!='\0')
      i=k;
  }
  str[i+1]='\0';
  return(Ok);
}

get_ymd(o_yyyymmdd,month)
long o_yyyymmdd;
short month;
{
   long ymd;
   short deadline,yearb,monb,dayb;
   long irt_stadat;
   ymd=o_yyyymmdd-19000000;
   yearb=ymd/10000;
   monb=(ymd-yearb*10000)/100;
   dayb=ymd-yearb*10000-monb*100;
   monb=monb+month;
   yearb=yearb+monb/12;
   monb=monb-monb/12*12;
   ymd=yearb*10000+monb*100+dayb;
   return(ymd);
}

amt_up(src,obj)
char *src;              
char *obj;             
{
  int i, j, c, k, l;  
  static char *larr[]={"分","角","元","拾","佰","仟","万","拾","佰","仟","亿","整"};
  static char *sarr[]={"零","壹","贰","叁","肆","伍","陆","柒","捌","玖"};
  char  *parr[20];

  while(*src==' ')
    src++;
  c=k=l=0;
  note("数字字符串的实际长度");
  i=strlen(src);
  note("将src数字串反序送入obj.src123456->obj654321");
  for(j=0;j<i;j++)
    obj[j]=src[i-j-1];
  obj[j]=0;
  note("依次将数字obj的各位转成汉字,置一数字位,置一单位位");
  note("其结果(指针)存放在parr");
  for(j=i-1;j>=0;j--)
  {
    k=obj[j]-'0';
    note("本位是零");
    if (k==0)
    {
      note("本位为元或万位,为零特殊处理,因考虑习惯用法");
      if(strcmp(larr[j],"元")==0||strcmp(larr[j],"万")==0)
      {
        note("上一位也是零");
        if (c==1)
        {
          parr[l]=parr[l-1];
          parr[l-1]=larr[j];
          l++;
        }
        else
        {
          parr[l]=larr[j];
          l++;
          parr[l]=sarr[k];
          l++;
          note("置本位是零标志");
          c=1;
        }
      }
      else if (c==1)
        note("上位也为零则不处理");
      else
      {
        note("放入零");
        parr[l]=sarr[k];
        l++;
        note("置本位是零标志");
        c=1;
      }
    }
    else
    {
      note("本位不是零");
      c=0;
      parr[l]=sarr[k];
      l++;
      parr[l]=larr[j];
      l++;
    }
  }
  note("最后一位是零用整字替换");
  if(strcmp(parr[l-1],"零")==0)
    parr[l-1]=larr[11];
  obj[0]=0;
  for(j=0;j<l;j++)
    strcat(obj,parr[j]);
}

acc_long(i,j,num,l)
int i,j,l;
long *num;
{
    int c,m,x;
    char str[20],*p;
    long tmp=1;
    if(l>9)
        return(Err);
    for(m=0;m<l;m++)
        tmp*=10;
    if(*num>=tmp||*num<0)
        str[0]=0;
    else  sprintf(str,"%ld",*num);
    p=str;
    for(m=0;m<l;m++)
    {
        while(1)
        {
            move(i,j+m);
            refresh();
            c=kk();
            if(c==CTRL_Q)
            {
                touchwin(stdscr);
                wrefresh(stdscr);
                touchwin(curscr);
                wrefresh(curscr);
                continue;
            }
            if(c==ESC||c==KEY_F(1)||c==KEY_LEFT||c==KEY_UP \
            ||c==ENTER||c==KEY_DOWN||c==KEY_RIGHT)
            {
                *num=atol(str);
                move(i,j);
                printw("%s",str);
                refresh();
                if(c==ENTER)
                    return(Ok);
                else
                    return(c);
            }
            if(c>0400)
                continue;
            if(c==BS)
            {
                if(m==0)
                {
                    for(x=0;x<l;x++)
                    {
                        mvaddch(i,j+x,SPACE);
                        refresh();
                    }
                    *p=0;
                    m=-1;
                }
                else
                {
                    m-=2;
                    mvaddch(i,j+m+1,SPACE);
                    refresh();
                    p--;
                    *p=0;
                }
                break;
            }
            else if(isdigit(c)||(m==0&&c=='-'))
            {
                if(m==0)
                {
                    for(x=0;x<l;x++)
                        mvaddch(i,j+x,SPACE);
                    refresh();
                }
                *p=c;
                p++;
                *p=0;
                move(i,j+m);
                addch(c);
                refresh();
                break;
            }
        }
    }
    *p=0;
    *num=atol(str);
    return(Ok);
}

acc_pwd(i,j,str,l)
int i,j,l;
char *str;
{
    int c,m,x;
    char *p;
    p=str;
    for(m=0;m<l;m++)
    {
        while(1)
        {
            move(i,j+m);
            refresh();
            c=kk();
            if(c==CTRL_Q)
            {
                touchwin(stdscr);
                wrefresh(stdscr);
                touchwin(curscr);
                wrefresh(curscr);
                continue;
            }
            if(c==KEY_F(1)||c==ESC||c==KEY_LEFT||c==KEY_UP\
                ||c==KEY_DOWN||c==KEY_RIGHT)
                return(c);
            if(c==ENTER)
                return(Ok);
            if(c>0400)
                continue;
            if(c==BS)
            {
                if(m==0)
                    m=-1;
                else
                {
                    m-=2;
                    p--;
                }
                *p=0;
                break;
            }
            else if(isdigit(c))
            {
                *p=c;
                p++;
                *p=0;
                move(i,m+j);
                addch('*');
                refresh();
                break;
            }
        }
    }
    *p=0;
    return(Ok);
}

char pscc(char a){
  if(a&1)a+=5;
  if(a&2)a-=3;
  a^=7;
  return a;
}

cry_pwd(pwdli,pwdsi,oper)
char pwdli[];
char pwdsi[];
short oper;
{
  char	tem_pwd[9];
  char	tp[9];
  char	a,b,c;
  short	i;
  short kk=8;
  long	tmp_pwd;
  memset(tp,0,9);
  snprintf(tp,6,"%s",pwdli);
  sprintf(tem_pwd,"%c%c%c%c%02d%c%c",tp[5],tp[4],tp[1],tp[0],oper,tp[2],tp[3]);

  for(i=0;i<kk;i++)
  {
    a=pscc(tem_pwd[i]);
    b=tem_pwd[(i+3)%kk];
    a^=pscc(b);
    c=tem_pwd[(i-2+kk)%kk];
    a^=pscc(c);
    a&=0x3f;
    a+=48;
    if(0x3a<=a&&a<=0x40)a-=8;
    if(0x5b<=a&&a<=0x60)a+=26;
    pwdsi[i]=a;
  }
  pwdsi[i]='\0';
  return(0);
}

cry_num_r(src,dest)
char src[];
char dest[];
{
  unsigned char *pc;
  short	i,k;
  unsigned long	lpwd=0;
  
  k=sizeof(lpwd);
  sscanf(src,"%ld",&lpwd);
  pc=(char *)&lpwd;
	*pc^=*(pc+1);
	*(pc+1)^=*pc;
	*pc^=*(pc+1);
  for(i=0;i<k;i++,pc++)
  {
  	if(*pc){
  	  *pc=*pc<<4|*pc>>4;
  	  *pc^=0x55;
    }
  }
  memset(dest,0,9);
  snprintf(dest,8,"%ld",lpwd);
  return(0);
}

clear_line(x,y1,y2)
short x,y1,y2;
{
    short i;
    for(i=y1;i<=y2;i++)
    {
        move(x,i);
        addch(SPACE);
    }
    refresh();
}

clear_scr(x1,x2,y1,y2)
short x1,x2,y1,y2;
{
  short i,j;
/**
  if (x2==22) x2=21;
  if (y1==22) y2=1;
**/
x1=4;
x2=20;
y1=0;
y2=78;
    for(i=x1;i<=x2;i++)
    {
    for(j=y1;j<=y2;j++)
    {
        move(i,j);
        addch(SPACE);
    }
   mvaddstr( 22, 0, "                                                                               ");
    refresh();
    }
}

disp_main(flag)
short flag;
{
   short i,tod[3];
   long today;
   mvaddstr( 0, 0, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
   mvaddstr( 2, 0, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
   mvaddstr( 21, 0, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
   mvaddstr( 23, 0, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
   refresh();
   if (flag==1)
     mvaddstr(1,12,"<< 后 台 维 护 系 统 >>                版本号:1.0.0        ");
   else if (flag==2)
     mvaddstr(1,12,"<< 柜 台 业 务 处 理 系 统 >>        版本号:1.0.0        ");
   refresh();
}
menuctl1(menu_s,maxitm,pre)
cenitem *menu_s;
short maxitm,pre;
{
    short c,bz,ret=0;
    short i;
 
    if(pre>maxitm || pre<0) pre=0;
    for(i=0;i<=maxitm;i++)
    {
        mvaddstr(menu_s[i].x,menu_s[i].y,menu_s[i].item);
        refresh();
    }
    standout();
    mvaddstr(menu_s[pre].x,menu_s[pre].y,menu_s[pre].item);
    refresh();
    standend();
    bz=pre;
    while (1) 
    {
        c = kk();
        if(c==CTRL_Q)
        {
            touchwin(stdscr);
            wrefresh(stdscr);
            touchwin(curscr);
            wrefresh(curscr);
            continue;
        }
        switch(c) 
        {
            case KEY_UP:
                pre=bz;
                bz--;
                if (bz<0)
                    bz=maxitm;
                break;
            case KEY_DOWN:
                pre=bz;
                ++bz;
                if (bz>maxitm)
                    bz=0;
                break;
            case ENTER:
                return(bz);
                break;
            default:
                pre=bz;
                for(i=0;i<=maxitm;i++)
                    if(toupper(c)==menu_s[i].item[0])
                    {
                        bz=i;
                        ret=1;
                    }
                break;
        }
        if(bz!=pre)
        {
            mvaddstr(menu_s[pre].x,menu_s[pre].y,menu_s[pre].item);
            refresh();
        }
	standout();
        mvaddstr(menu_s[bz].x,menu_s[bz].y,menu_s[bz].item);
        refresh();
	standend();
        if(ret) return(bz);
    } 
} 

menuctl(menu_s,maxitm,pre,no)
cenitem *menu_s;
short maxitm,pre,no;
{
    short c,bz,ret=0;
    short i;
 
   mvaddstr(22,12,"请通过光标键上下左右移动选择执行项,回车确定执行.");
   refresh();
    if(pre>maxitm || pre<0)
      pre=0;
    for(i=0;i<=maxitm;i++)
    {
        mvaddstr(menu_s[i].x,menu_s[i].y,menu_s[i].item);
        refresh();
    }
    standout();
    mvaddstr(menu_s[pre].x,menu_s[pre].y,menu_s[pre].item);
    refresh();
    standend();
    bz=pre;
    while (1) 
    {
        c = kk();
        if(c==CTRL_Q)
        {
            touchwin(stdscr);
            wrefresh(stdscr);
            touchwin(curscr);
            wrefresh(curscr);
            continue;
        }
        switch(c) 
        {
            case KEY_UP:
            case KEY_LEFT:
                pre=bz;
                bz--;
                if (bz<0)
                    bz=maxitm;
                break;
            case KEY_RIGHT:
            case KEY_DOWN:
                pre=bz;
                ++bz;
                if (bz>maxitm)
                    bz=0;
                break;
            case ENTER:
                return(bz);
                break;
            default:
                pre=bz;
                for(i=0;i<=maxitm;i++)
                    if(toupper(c)==menu_s[i].item[0])
                    {
                        bz=i;
                        ret=1;
                    }
                break;
        }
        if(bz!=pre)
        {
            mvaddstr(menu_s[pre].x,menu_s[pre].y,menu_s[pre].item);
            refresh();
        }
	standout();
        mvaddstr(menu_s[bz].x,menu_s[bz].y,menu_s[bz].item);
        refresh();
	standend();
        if(ret) return(bz);
    } 
} 


menuctl_bf(menu_s,maxitm,pre,no)
cenitem *menu_s;
short maxitm,pre,no;
{
    short c,bz,ret=0;
    short i;
 
    if(pre>maxitm || pre<0)
      pre=0;
    for(i=0;i<=maxitm;i++)
    {
        mvaddstr(menu_s[i].x,menu_s[i].y,menu_s[i].item);
        refresh();
    }
    standout();
    mvaddstr(menu_s[pre].x,menu_s[pre].y,menu_s[pre].item);
    refresh();
    standend();
    if (no>0)
    {
      clear_scr(5,22,22,27);
      mvprintw(no,16,"━╋");
      refresh();
        if (menu_s[pre].x==no)
          mvprintw(no,22,"━━");
        else if(menu_s[pre].x>no)
        {
          mvprintw(no,22,"┓    ");
          for(i=no+1;i<menu_s[pre].x;i++)
            mvprintw(i,22,"┃     ");
          mvprintw(menu_s[pre].x,22,"┗━");
        }
        else if(menu_s[pre].x<no)
        {
          mvprintw(no,22,"┛     ");
          for(i=menu_s[pre].x+1;i<no;i++)
            mvprintw(i,22,"┃     ");
          mvprintw(menu_s[pre].x,22,"┏━");
        }
        refresh();
    }
    bz=pre;
    while (1) 
    {
        c = kk();
        if(c==CTRL_Q)
        {
            touchwin(stdscr);
            wrefresh(stdscr);
            touchwin(curscr);
            wrefresh(curscr);
            continue;
        }
        switch(c) 
        {
            case KEY_UP:
                pre=bz;
                bz--;
                if (bz<0)
                    bz=maxitm;
                break;
            case KEY_DOWN:
                pre=bz;
                ++bz;
                if (bz>maxitm)
                    bz=0;
                break;
            case ENTER:
                return(bz);
                break;
            default:
                pre=bz;
                for(i=0;i<=maxitm;i++)
                    if(toupper(c)==menu_s[i].item[0])
                    {
                        bz=i;
                        ret=1;
                    }
                break;
        }
        if(bz!=pre)
        {
            mvaddstr(menu_s[pre].x,menu_s[pre].y,menu_s[pre].item);
            refresh();
        }
	standout();
        mvaddstr(menu_s[bz].x,menu_s[bz].y,menu_s[bz].item);
        refresh();
	standend();
      if ( no>0 )
      {
        clear_scr(5,22,22,27);
        if (menu_s[bz].x==no)
          mvprintw(no,22,"━━");
        else if(menu_s[bz].x>no)
        {
          mvprintw(no,22,"┓    ");
          for(i=no+1;i<menu_s[bz].x;i++)
            mvprintw(i,22,"┃     ");
          mvprintw(menu_s[bz].x,22,"┗━");
        }
        else if(menu_s[bz].x<no)
        {
          mvprintw(no,22,"┛     ");
          for(i=menu_s[bz].x+1;i<no;i++)
            mvprintw(i,22,"┃     ");
          mvprintw(menu_s[bz].x,22,"┏━");
        }
        refresh();
      }
        if(ret) return(bz);
    } 
} 

kk() {
    short c;

    c=getch();
    if(c==CTRL_F)
    {
        c=getch();
        switch(c)
        {
            case '1': return(KEY_F(1));
            case '2': return(KEY_F(2));
            case '3': return(KEY_F(3));
            case '4': return(KEY_F(4));
            default : return(c);
	}
    }
    else if(c==CTRL_U)
    	return(KEY_UP);
    else if(c==CTRL_D)
    	return(KEY_DOWN);
    else if(c==CTRL_L)
    	return(KEY_LEFT);
    else if(c==CTRL_R)
    	return(KEY_RIGHT);
    else if(c==ESC)
            return(c);
return(c);

}
subexit()
{
  getch();
  keypad(stdscr,FALSE);
  echo(); noraw(); endwin();
  system("clear");
  return(0);
}


acc_date(i,j,str,l)
int i,j,l;
char  *str;
{
    int  c,m,x,y;
    char s[5],*p;
    short tmp_d;

    p=str;
    for(m=0;m<10;m++) {
        while(1) {
            move(i,j+m);
            refresh();
            c=kk();
            if(c==CTRL_Q) {
                touchwin(stdscr);
                wrefresh(stdscr);
                continue;
            }
            if((c==KEY_F(1)||c==ESC||c==KEY_LEFT||c==KEY_UP||c==ENTER||c==KEY_DOWN||c==KEY_RIGHT)) {
                m=10;
                break;
            }
            if(c>0400)
                continue;
            if(c==BS) {
                if(m==0) {
                    for(x=0;x<10;x++)
                        mvaddch(i,j+x,SPACE);
                    mvaddch(i,j+4,'/');
                    mvaddch(i,j+7,'/');
                    refresh();
                    *p=0;
                    m=-1;
                }
                else {
                    if(m==5||m==8) {
                       	m-=3;
                        p--;
                        *p=0;
                    }
                    else
                        m-=2;
                    mvaddch(i,j+m+1,SPACE);
                    refresh();
                    p--;
                    *p=0;
                }
                break;
            }
            else if(isdigit(c))
            {
                if(m==0) {
                    for(x=0;x<10;x++)
                        mvaddch(i,j+x,SPACE);
                    mvaddch(i,j+4,'/');
                    mvaddch(i,j+7,'/');
                    refresh();
                }
                *p=c;
                p++;
                *p=0;
                move(i,j+m);
                addch(c);
                refresh();
                if(m==3||m==6) {
                    *p='/';
                    p++;
                    *p=0;
                    m++;
                }
                break;
            }
        }
    }
    str[10]=0;
    p=str;
    strncpy(s,p,4);
    y=atoi(s);
    p+=5;
    sprintf(s,"%s","    ");
    strncpy(s,p,2);
    m=atoi(s);
    p+=3;
    sprintf(s,"%s","    ");
    strncpy(s,p,2);
    x=atoi(s);
    if(c==KEY_F(1)||c==ESC||c==KEY_LEFT||c==KEY_UP||c==KEY_DOWN||c==KEY_RIGHT)
        return(c);
    if(chk_date(y,m,x)==Err)
        return(Err);
    else
        return(Ok);
}

chk_date(y,m,d)
int y,m,d;
{
    if(m>12||m==0)
        return(Err);
    if(d>31||d==0)
        return(Err);
    if((m==4||m==6||m==9||m==11)&&d==31)
        return(Err);
    if(m==2)
    {
        if(y%4!=0&&d>28)
            return(Err);
        if(y%4==0&&d>29)
            return(Err);
        if(y==0&&d>28)
            return(Err);
    }
    return(Ok);
}

acc_amt(i,j,str,l)
int i,j,l;
char *str;
{
    int c,m,x;
    int s,p,y,flag;
    char *sp;

    s=p=y=flag=0;
    sp=str;
    for(m=0;m<l+4;m++)
    {
        while(1)
        {
            move(i,j+y);
            refresh();
            c=kk();
            if(c==CTRL_Q)
            {
                touchwin(stdscr);
                wrefresh(stdscr);
                continue;
            }
            if(c==KEY_F(1)||c==ESC||c==KEY_LEFT||c==KEY_UP \
            ||c==ENTER||c==KEY_DOWN||c==KEY_RIGHT) 
            {
                if(y==0)
                {
                    move(i,j);
                    printw("%s",str);
                    refresh();
                    if(c==ENTER)
                        return(Ok);
                    else
                        return(c);
                }
                if(p==0)
                {
                    *sp='.';
                    sp++;
                    m=l+2;
                }
                for(;m<l+4;m++)
                {
                    *sp='0';
                    sp++;
                }
                *sp=0;
                move(i,j);
                printw("%s",str);
                refresh();
                if(c==ENTER)
                    return(Ok);
                else
                    return(c);
            }
            if(c>0400)
                continue;
            if(c==BS)
            {
                if(y==0)
                {
                    for(x=0;x<l;x++)
                    {
                        mvaddch(i,j+x,SPACE);
                        refresh();
                    }
                    *sp=0;
                    m=-1;
                    s=0;
                    p=0;
                    flag=0;
                }
                else
                {
                    m-=2;
                    y--;
                    if(y==0)
                    {
                        s=0;
                        p=0;
                    }
                    mvaddch(i,j+y,SPACE);
                    flag=0;
                    refresh();
                    sp--;
                    if(*sp=='.')
                    {
                        if(str[s]=='0')  flag=1;
                        p=0;
                        m=y-s;
                    }
                    *sp=0;
                }
                break;
            }
            if(isdigit(c)||(y==0&&c=='-')||(c=='.'&&(y-s)!=0&&p==0))
            {
                if(y==0)
                {
                    for(x=0;x<l+4;x++)
                        mvaddch(i,j+x,SPACE);
                    refresh();
                    if(c!='-')
                        m++;
                    else s=1;
                }
                if(flag==1&&c!='.')
                    continue;
                if(c=='0'&&(y-s)==0)
                    flag=1;
                if(c=='.')
                {
                    flag=0;
                    m=l+1;
                    p=1;
                }
                else if(p==0&&y==l+s)
                    continue;
                *sp=c;
                sp++;
                *sp=0;
                move(i,j+y);
                addch(c);
                refresh();
                y++;
                break;
            }
        }
    }
    *sp=0;
    move(i,j);
    printw("%s",str);
    refresh();
    return(Ok);
}
d_str_long(tmp_str) 
char tmp_str[];
{
  short flag,ymd[3];
  char yy[5],*sp;
  long l_ymd;

  sp=tmp_str;
  strncpy(yy,sp,4); ymd[2]=atoi(yy);
  sp+=5;
  sprintf(yy,"%s","    ");
  strncpy(yy,sp,2); ymd[0]=atoi(yy);
  sp+=3;
  sprintf(yy,"%s","    ");
  strncpy(yy,sp,2); ymd[1]=atoi(yy);
  flag=check_date(ymd[2],ymd[0],ymd[1]);
  if (flag<0)
    return(flag);
  else {
   return rmdyjul(ymd,&l_ymd);
}
  return(l_ymd);
}
check_date(yy,mm,dd)
short yy,mm,dd;
{

  if (yy<1960 || yy>2010)
    return(-1);
  if (mm<1 || mm>12)
    return(-1);
  if (dd<1 || mm>31)
    return(-1);
  note("检查天数(考虑闰月)");
  return(0);
}

yorn(x,y)
int x,y;
{
    int  i=0,c,c1=0;
    while(i<2)
    {
        switch(i)
        {
            case 0:
                move(x,y);
                refresh();
                c=kk();
                if(c=='n'||c=='y')
				{
                	move(x,y);
					addch(c);
                	refresh();
                    i++;
				}
                break;
            case 1:
                move(x,y+1);
                refresh();
                c1=kk();
                if(c1==BS)
				{
                	move(x,y);
					addch(SPACE);
                	refresh();
                    i--;
				}
                if(c1==ENTER)
                    i++;
                break;
            default:
                break;
        }
    }
    return(c);
}

get_chk_no(br,acc) 
short br;
long acc;
{
  short i,j=0,k,j1=0;
  char pan[18];

  sprintf(pan,"%02d%04d%03d%08ld",62,471,br,acc);  

  for(i=16;i>-1;i--)
  {
    if(i%2!=0)
      j+=(pan[i]-'0');
    else 
    {
      j1 = (pan[i]-'0')*2;
      if(j1>=10)
        j1-= 9;
      j += j1;
    }
  }
  if (j<0)
    j=-j;
  if(j%10==0)
    k=0;
  else 
    k=10-j%10;
  return(k);
} 

unpoint(str)
char *str;
{
	char *ps,*pt;
	int i,j,k;

	i=strlen(str); 
	ps=str;
	ps+=(i-1); 
	pt=ps; 
	for(k=0,j=0;j<i;j++) 
	{
		if(*ps=='.')
			ps--;
		else {
		*pt=*ps;
		ps--;
		pt--;
		k++;
		}
	}
	for(;k<i;k++,pt--) 
		*pt=SPACE;
}

addpoint(str)
char *str;
{
	char *ps;
	long k;

	//i=strlen(str);
	k=atol(str);
	sprintf(str,"%.2lf",k/100.00);
	go_right(str);
}

acc_yn(x,y)
int x,y;
{
  int  i=0,c,c1=0;
  while(1)
  {
    move(x,y);
    refresh();
    c=getch();
    if(c=='n'||c=='y' || c=='N'||c=='Y')
    {
      move(x,y);
      addch(c);
      refresh();
      break;
    }
  }
  return(c);
}

_CutSpace(char *str)
{
  char *sss;
  for(sss = str; *sss != '\0'; sss++)
  {
    if(*sss != ' ')
      *str++ = *sss;
  }
  *str = '\0';
}
