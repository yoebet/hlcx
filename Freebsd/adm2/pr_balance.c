/**************************************************************
 程序名:pr_balance.ec 
 函数名:pr_balance() 
 功  能:生成各所/全机器余额表 
**************************************************************/       
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;
extern MYSQL *conn;
extern char sql[];

MYSQL_RES *res;
MYSQL_ROW row;

struct bal_s
{   
  long km;
  char flag;
  char kmname[17];
  long home_num; /* 户数 */
  double sbal; /*总余额*/
  short lcount; /*挂失户数 */
  short fcount; /*冻结户数 */
  long  dcvou;  /*借方笔数 */
  double dcre;   /*借方发生额*/
  long  ddvou;  /*贷方笔数 */
  double ddet;   /*贷方发生额*/
  long  dcls;  /*销户数*/
  long  dopen;  /*开户数*/
} bal[100];  /** 总科目号=100 **/

extern short br_id[50];
short k,cen_id,branch_id,item_count,b_count,biser=0;
short sflag=0,sh,ymd[3];
char c,tabletype;
char br_name[11];
long rectmp,ltoday;
  
pr_balance()
{
  char yy;
  struct in_data da;

  static short in[2]={27,8};
  short num=2,flag;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《打印余额表》");  
  refresh();

  dis_str(22,22,"正在处理,请稍侯......",0,0);  

  rtoday(&ltoday);
  rjulmdy(ltoday,ymd);

  da.tr_br=100;
  branch_id=da.tr_br;
  if (branch_id==0)
    return(0);

  note("余额表定义文件读入bal数组!");
  if(read_bal()==-1)
    return(-1);

  note("生成某所branch_id余额表");
  note("将余额表数组bal[100]中统计变量清0"); 
  init_bal();

  note("取所名!");
	
	sprintf(sql,"SELECT br_name FROM br_file WHERE  branch= %d ",branch_id);
  	
  mysql_query(conn,sql);

  res=mysql_store_result(conn);

  if((row=mysql_fetch_row(res))!=NULL){
    strcpy(br_name,row[0]);
  }
  mysql_free_result(res);

  if(row==NULL)
  {
    sprintf(sys_err_str,"取所记录错!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  note("取总帐中本所,各科目的本日借/贷方笔数和发生额!");
  if(read_sumtable()!=0)
  {
    sprintf(sys_err_str,"取总帐数据错!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  note("将branch_id所的各储种余额分别统计到bal[]!");
	if(sum_bal()!=0)
  {
    sprintf(sys_err_str,"统计余额错!.");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  note("根据统计的结果及表的类型将余额表生成!");
  pr_bal();
  
  return(0);
}

/**************************************************
函数名:read_bal()             
作  用:将余额表定义文件data/def_bal
       读入余额表数组中bal[100]
参  数:
       全局变量bal[100]
**************************************************/
read_bal()
{
  FILE *fp;
  int j,k;
  if((fp=fopen("./data/def_bal","r"))==NULL)
  {	
    dis_str(22,22,"余额表定义文件/usr/hlcx/data/def_bal打不开!",60,0);
    getch();
    return(-1);
  }
  note("读 def_bal 给数组bal赋值!");
  for(k=0;;k++)
  {
  	//211102 0 消 费 卡，到空格结束
    j=fscanf(fp,"%ld %c %s",&bal[k].km,&bal[k].flag,bal[k].kmname);
    if(j!=3) 
      break;
    if (bal[k].km==999999)
      break;
  }
  item_count=k-1;
  return(0);
}

/**************************************************
函数名:init_bal()         
作  用:将余额表数组bal[100]中统计变量清0 
参  数:
       全局变量
         bal[100]
         item_count  科目数
**************************************************/
init_bal()
{
  int j;
  for(j=0;j<=item_count;j++)
  {
    bal[j].lcount=0;
    bal[j].fcount=0;
    bal[j].home_num=0L;
    bal[j].dcvou=0L;
    bal[j].ddvou=0L;
    bal[j].dcls=0L;
    bal[j].dopen=0L;
    bal[j].sbal=0;
    bal[j].dcre=0;
    bal[j].ddet=0;
  }
  return(0);
}

/**************************************************
函数名:read_sumtable()          
作  用:取总帐中本所,各科目的本日(kind=90)
       借/贷方笔数和借/贷方发生额
参  数:
       全局变量
         bal[100]
         item_count  科目数
**************************************************/
read_sumtable()
{
  int i;
  double lcre,ldet;
  long  lcvou,ldvou,lopen,lcls;
  for(i=0;i<=item_count;i++)
  {
    lcvou=ldvou=lopen=lcls=0;
    lcre=ldet=0.00;
	
	  /****************/
	
	  sprintf(sql,"SELECT cvou_que,cre_amt,dvou_que,det_amt,open_que,cls_que\
  	    FROM sumtable WHERE  branch= %d AND kind=90 AND item_num=%ld",branch_id,bal[i].km);
  	
    mysql_query(conn,sql);
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
		  lcvou=atol(row[0]);
		  lcre=atof(row[1]);
		  ldvou=atol(row[2]);
		  ldet=atof(row[3]);
		  lopen=atol(row[4]);
		  lcls=atol(row[5]);
    }
    mysql_free_result(res);
  
    if(row==NULL)
    {
      sprintf(sys_err_str,"取科目[%ld]的本日总帐错!.",bal[i].km);
      dis_str(22,22,sys_err_str,60,0);
      getch();
    }
	
	/****************/
	
    bal[i].dcvou+=lcvou;
    bal[i].ddvou+=ldvou;
    bal[i].dopen+=lopen;
    bal[i].dcls+=lcls;
    bal[i].dcre+=lcre; 
    bal[i].ddet+=ldet; 
  }
  return(0);//?
}
/********************************************************
函数名:pr_bal()                                       
作用:根据统计的结果及表的类型将余额表生成出来        
********************************************************/
pr_bal()
{
  FILE *fp;
  short balpoint,ii;
  char lin[150],balstr[15],branch_name[11];
  char balstr1[15],balstr2[15];
  char bal_file[55];
  char cmd_str[90];
  char flag;
  char sql[96];

  if(branch_id==888)
  {
    sprintf(bal_file,"./data/t,mp_all_bal");
    if((fp=fopen(bal_file,"w"))==NULL)
    {	
      dis_str(22,22,"余额表文件不能生成!",60,0);
      getch();
      return(-1);
    }
    fprintf(fp,"%40s全机器余额表"," " );
    fprintf(fp,"%20s 日期:%4d年%02d月%02d日\n"," ",ymd[2],ymd[0],ymd[1]);
  }
  else
  {
    sprintf(bal_file,"./tmp/tmp_br_bal_%d",branch_id);
    if((fp=fopen(bal_file,"w"))==NULL)
    {	
      dis_str(22,22,"余额表文件不能生成!",60,0);
      getch();
      return(-1);
    }
	
	/*********************/
	
	snprintf(sql,95,"SELECT br_name FROM br_file WHERE  branch= %d ",branch_id);
  	
    mysql_query(conn,sql);
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
	  strcpy(br_name,row[0]);
    }
    mysql_free_result(res);
  
    if(row==NULL)
    {
      sprintf(sys_err_str,"取所号[%d]的名称错!%s.",branch_id,mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
    }
	
	/*********************/
	
    fprintf(fp,"%40s  余额表\n"," ");
    fprintf(fp,"%20s 日期:%4d年%02d月%02d日        \n"," ",ymd[2],ymd[0],ymd[1]);
  }

  fprintf(fp,"┏━━━┳━━━━━━━━┳━━━━━━━━━━┳━━━━━━━━━━┳━━━━━━━┳━━┳━━┳━━━┳━━┳━━┓\n");
  fprintf(fp,"┃      ┃                ┃     借      方     ┃     贷      方     ┃              ┃    ┃    ┃      ┃    ┃    ┃\n");
  fprintf(fp,"┃科目号┃    科目名称    ┣━━┳━━━━━━━╋━━┳━━━━━━━┫    余  额    ┃销户┃开户┃ 余户 ┃挂失┃冻结┃\n");
  fprintf(fp,"┃      ┃                ┃笔数┃   发 生 额   ┃笔数┃   发 生 额   ┃              ┃    ┃    ┃      ┃    ┃    ┃\n");
  fprintf(fp,"┣━━━╋━━━━━━━━╋━━╋━━━━━━━╋━━╋━━━━━━━╋━━━━━━━╋━━╋━━╋━━━╋━━╋━━┫\n");
      
  ii=0;
  while (1)
  {
    if(bal[ii].sbal+bal[ii].home_num+bal[ii].dcre
      +bal[ii].ddet+bal[ii].dcvou+bal[ii].ddvou==0.0 )
    {
      ii++;
      if(ii>item_count)
        break;
      continue;
    }
    fprintf(fp,"┃%6ld┃%16s┃%4ld┃%14.2lf┃%4ld┃%14.2lf┃%14.2lf┃%4ld┃%4ld┃%6ld┃%4ld┃%4ld┃\n",bal[ii].km,bal[ii].kmname,bal[ii].dcvou,bal[ii].dcre/100.00,bal[ii].ddvou,bal[ii].ddet/100.00,bal[ii].sbal/100.00,bal[ii].dcls,bal[ii].dopen,bal[ii].home_num,bal[ii].lcount,bal[ii].fcount);
    if(bal[ii].flag=='1')
      fprintf(fp,"┣━━━╋━━━━━━━━╋━━╋━━━━━━━╋━━╋━━━━━━━╋━━━━━━━╋━━╋━━╋━━━╋━━╋━━┫\n");
    ii++;
    if(ii>item_count)
      break;
  }
  fprintf(fp,"┗━━━┻━━━━━━━━┻━━┻━━━━━━━┻━━┻━━━━━━━┻━━━━━━━┻━━┻━━┻━━━┻━━┻━━┛\n");
  fclose(fp);
    
	/*****************/
	
	sprintf(sql,"UPDATE br_file SET pr_flag = (pr_flag+1) WHERE branch= %d ",branch_id);
	
    if(mysql_query(conn,sql)||mysql_errno(conn)){
      sprintf(sys_err_str,"修改本所文件错!%s\n",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
	
	/*****************/
	
  //_DyFuntion(bal_file,3);

    dis_str(22,22,"是否打印(y/n):[ ]",60,0);
    flag=yorn(22,44);
    if(flag=='y'||flag=='Y') 
    {
      dis_str(22,22,"请准备好打印机后按任一键开始打印",60,0);
      getch();       
      sprintf(cmd_str,"pr %s > "Printer, bal_file);
      system(cmd_str);
    }
  return(0);
}
/*******************************************************
函数名:sum_bal() 
作  用:将branch_id所的各储种余额分别统计出来 
*******************************************************/
sum_bal()
{
  sprintf(sys_err_str,"正在统计各储种余额...");
  dis_str(22,22,sys_err_str,60,0);
  note("统计活期余额!");
  s_bal('s');
  note("统计定期余额!");
  s_bal('f');
  note("汇总各储种余额!");
  to_bal(); 
  return(0);
}

/****************************************************
函数名:fkm(item) 
作  用:根据科目号找它在余额表中位置 
输  入:科目号 item                    
返  回:此科目在余额表中的位置 
****************************************************/
fkm(item)
long item;
{
  short m,k;
  for(k=0;k<=item_count;k++)
  {
    if(bal[k].km==item)
      break;
  }
  if(k>item_count)
    return(-1);
  else
    return(k);
} 

/*****************************************************
函数名:s_bal(type)
作  用:统计各主储种余额 
参  数:
       char type; 需统计的储种
*****************************************************/
s_bal(type)
char type;
{
  char ss_amt[15],ss_flag[2],s_flag,sbr[150];
  long skm,sbalance,s_close_d;
  int rcode,j;

  note("赋 sbr[]初值!");
  for(j=0;j<150;j++)
    sbr[j]=' ';

  switch(type)
  {
    case 's':
      sprintf(sbr,"select balance,item_num,acc_flag from %c_mast where open_br = %d",type,branch_id);
      break;
    default:
      note("定期!");
      sprintf(sbr,"select amount,item_num,acc_flag from %c_mast where open_br = %d",type,branch_id);
      break;
  }
  
  mysql_query(conn,sbr);
  
  if(mysql_errno(conn))
  {
    sprintf(sys_err_str,"%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  res=mysql_use_result(conn);

  for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
      break;
		sbalance=atol(row[0]);
		skm=atol(row[1]);
		strcpy(ss_flag,row[2]);
	
    s_flag=ss_flag[0];
    note("作废K 销户C 部提B 挂失补开G!");
    if(s_flag=='K'||s_flag=='C'|| s_flag=='B'||s_flag=='G')
      continue;
    else
    {
      note("正常/挂失/冻结!");
      note("找本户所属科目在数组中的下标!");
      j=fkm(skm);
      if(j!=-1)
      {
        bal[j].home_num++; /*总户数 */
        if((s_flag=='L')||(s_flag=='l'))
          bal[j].lcount++;
        if(s_flag=='F')
          bal[j].fcount++;
        bal[j].sbal+=sbalance;
      }
    }
  }
  mysql_free_result(res);

  return(0);
}

/*****************************************************
函数名:to_bal()                               
作  用:统计各小计,总计科目
说  明:子科目号后两位为非零,小计科目后两位须为00
*****************************************************/
to_bal()
{
  FILE *fp;
  long saccno,acc1[20];
  short s1,s2;
  long tohome_num;
  long todcvou,toddvou,todcls,todopen;
  double tobal;
  double todcre,toddet;
  char balstr[14];
  char balstr1[14],balstr2[14];
  short tolcount,tofcount,i,j,k;
  short subkm;

  fp=fopen("./data/def_sbal","r");
  if(fp==NULL) 
  {
    dis_str(22,22,"汇总定义文件/usr/hlcx/data/def_sbal打不开!",60,0);
    getch();
    return(-1);
  }
  tohome_num=tolcount=tofcount=0L;
  todcvou=toddvou=todopen=todcls=0L;
  tobal=todcre=toddet=0.0;
  s1=s2=0;
  while(fscanf(fp,"%ld",&saccno)==1)
  {
    note("小计行未结束!");
    if(saccno != 0L)
    {
      note("将科目号放入数组acc1[]中!");
      acc1[s1]=saccno;
      note("累计小计科目数!");
      s1++;
    }
    else
    {
      note("小计行已结束!");
      if(s1<2) 
      {
        note("无小计科目,汇总文件格式错!");
        s1=s2=0;
        continue;
      }
      note("小计科目号在余额表中的位置!");
      j=fkm(acc1[0]);

      note("小计项数组置初值!");
      bal[j].home_num=bal[j].lcount=bal[j].fcount=0L;
      bal[j].dcvou=bal[j].ddvou=bal[j].dopen=bal[j].dcls=0L;
      bal[j].sbal=0.0;
      bal[j].dcre=0.0;
      bal[j].ddet=0.0;

      for(s2=1;s2<s1;s2++)
      {
        note("累加科目号在余额表中的位置!");
        i=fkm(acc1[s2]);

        bal[j].home_num+=bal[i].home_num;
        bal[j].lcount  +=bal[i].lcount;
        bal[j].fcount  +=bal[i].fcount;
        bal[j].dcvou   +=bal[i].dcvou;
        bal[j].ddvou   +=bal[i].ddvou;
        bal[j].dcls    +=bal[i].dcls;
        bal[j].dopen   +=bal[i].dopen;
        bal[j].sbal     +=bal[i].sbal;
        bal[j].dcre    +=bal[i].dcre;
        bal[j].ddet    +=bal[i].ddet;
      }
      s1=s2=0;
    }
  }
  fclose(fp);
  return(0);
}

/*****************************************************
函数名:str_right(s)
作  用:将字符串s右对齐
参  数:
       char s[]; 需处理的字符串
*****************************************************/
str_right(s)
char s[];
{
  int i,j;
  j=i=strlen(s);
  i--;
  while(j>0) 
  {
    j--;
    if(s[j] !=' ') 
    { 
      s[i]=s[j];
      i--;
    }
    if(i !=j)
      s[j]=' ';
  }
  return(0);
}

