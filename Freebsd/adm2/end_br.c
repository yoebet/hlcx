/****************************************************************
程序名:end_br.ec
函数名:end_br()
功  能:根据日志记指定所号的总帐
****************************************************************/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <sys/ipc.h>
#include "mysql.h"
#include "../include/sh_table.h"

extern MYSQL *conn;
extern char sql[];

MYSQL *conn2;

void _clscon_e(){
	if(conn2!=NULL)mysql_close(conn2);
}

end_br()
{
  
  struct f_branch br;
  struct f_sumtable sum,mysum;
  struct f_vouch vou,myvou;
  short debug;
  char yy,sys_err_str[100];
  long  item;
  long  ldate;
  double  balance;
  short  kind,rest_que; 
  short mdy_today[3],mdy_today1[3],ymd[3];
	char cond[100];
	char *vouch_u_bkt="update vouch set branch=%d,kind=%d,flag='%s',vou_type='%s',\
	    rest_que=%ld,use_que=%ld,cancel=%ld,chg_book=%ld,takeback=%ld\
	     where branch=%d and kind=%d and vou_type='%s'";

  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_RES *res2;
  MYSQL_ROW row2;

  static short in[2]={27,8};
  short num=2,flag;
  struct in_data da;

  init_in(&da);

  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"《结束本日业务》");
    refresh();

    dis_str(22,22,"正在处理,请稍侯......",0,0);  

	  da.tr_br=100;
	
    
    sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",da.tr_br);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"取所记录错!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      dis_str(22,22,"本所不存在!",0,0);  
      getch();
      return(-1);
		}
    set_f_branch(&br,row);
    mysql_free_result(res);
	
    if (br.init_flag>0)
    {
      dis_str(22,22,"本所本日已结束!",60,0);
      refresh();
      getch();
      return(0);
    }
    if (br.sum_flag==0)
    {
      dis_str(22,22,"本所未记总帐!",60,0);
      refresh();
      getch();
      return(0);
    }
    rjulmdy(br.work_d,mdy_today);

    //转换总帐昨日数据
    sprintf(sql,"SELECT * FROM sumtable WHERE kind=90 and branch=%d",da.tr_br);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"修改总帐文件错!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      sprintf(sys_err_str,"修改总帐文件错!");
      dis_str(22,22,sys_err_str,60,0); 
      getch();
      return(-1);
		}
    
    if(conn2==NULL){
    	conn2=mysql_init(NULL);
      atexit(_clscon_e);
	    if(!connectdb(conn2)){
	      sprintf(sys_err_str,"%s",mysql_error(conn2));
	      dis_str(22,22,sys_err_str,60,0);
	      getch();
	      return(-1);
	    }
    }
    
    /***********************************/
    
    if(mysql_query(conn2,"START TRANSACTION")){
      sprintf(sys_err_str,"不能启动事务");
      dis_err(sys_err_str);
      return(-1);
    }
    
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      set_f_sumtable(&sum,row);
      
      note("保存本日记录 本年的第几日+200");
      ymd[2]=mdy_today[2];
      ymd[0]=1;
      ymd[1]=1;
      rmdyjul(ymd,&ldate);
      //ldate=br.work_d-ldate+200;
      //br.work_d不是天数
      ldate=(br.work_d-ldate)/(24*60*60)+200;
      note("写本科目的本日总帐记录");
      //mysum.kind=ldate;
      sum.kind=ldate;
      
      //直接insert即可
      set_f_sumtable_i_sql(&sum,sql);
      if(mysql_query(conn2,sql))
      {
        sprintf(sys_err_str,"保存本日总帐错!%s",mysql_error(conn2));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn2);
        mysql_free_result(res);
        getch();
        return(-1);
      }

      note("修改昨日总帐");
      sum.kind=91;
      
      set_f_sumtable_u_sql(&sum,sql);
      sprintf(cond,"branch=%d and kind=%d and item_num=%ld",da.tr_br,sum.kind,sum.item_num);
      strcat(sql,cond);
      if(mysql_query(conn2,sql))
      {
        sprintf(sys_err_str,"修改昨日总帐错!%s",mysql_error(conn2));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn2);
        mysql_free_result(res);
        getch();
        return(-1);
      }
	
	  note("取本科目的本月总帐记录");
	  sum.kind=mdy_today[0];
	  
    sprintf(sql,"SELECT * FROM sumtable WHERE branch=%d and kind=%d and item_num=%ld",
      sum.branch,sum.kind,sum.item_num);
	
    if(mysql_query(conn2,sql)){
      sprintf(sys_err_str,"取本月总帐错!%s.",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
    res2=mysql_store_result(conn2);
    row2=mysql_fetch_row(res2);
    if(row2==NULL){
      sprintf(sys_err_str,"取本月总帐错!");
      dis_str(22,22,sys_err_str,60,0);
    	mysql_rollback(conn2);
    	mysql_free_result(res);
      getch();
    	return(1);
    }
    set_f_sumtable(&mysum,row2);
	
	  note("累加本日贷方发生额到本月");
	  mysum.det_amt+=sum.det_amt;
	
	  note("累加本日借方发生额到本月");
	  mysum.cre_amt+=sum.cre_amt;
	
	  note("复制余额");
	  mysum.balance=sum.balance;
	
	  note("累计笔数到本月");
	  mysum.open_que+=sum.open_que;
	  mysum.cls_que+=sum.cls_que;
	  mysum.dvou_que+=sum.dvou_que;
	  mysum.cvou_que+=sum.cvou_que;
	  mysum.rest_que=sum.rest_que;//?
	  note("修改本月XX记录(将本日90累加入本月XX");
	  
    set_f_sumtable_u_sql(&mysum,sql);
    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",mysum.branch,mysum.kind,mysum.item_num);
    strcat(sql,cond);
    if(mysql_query(conn2,sql))
    {
      sprintf(sys_err_str,"修改本月总帐错!%s",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
	
	  note("将本日总帐90累加入本年总帐99记录");
	  sum.kind=99;
	  
    sprintf(sql,"SELECT * FROM sumtable WHERE branch=%d and kind=%d and item_num=%ld",
      sum.branch,sum.kind,sum.item_num);
	
    if(mysql_query(conn2,sql)){
      sprintf(sys_err_str,"取本年总帐错!%s.",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
    res2=mysql_store_result(conn2);
    row2=mysql_fetch_row(res2);
    if(row2==NULL){
      sprintf(sys_err_str,"取本年总帐错!");
      dis_str(22,22,sys_err_str,60,0);
    	mysql_rollback(conn2);
      mysql_free_result(res);
    	return(0);
    }
    set_f_sumtable(&mysum,row2);
	  
	  mysum.det_amt+=sum.det_amt;
	  mysum.cre_amt+=sum.cre_amt;
	  mysum.balance=sum.balance;
	  mysum.open_que+=sum.open_que;
	  mysum.cls_que+=sum.cls_que;
	  mysum.dvou_que+=sum.dvou_que;
	  mysum.cvou_que+=sum.cvou_que;
	  mysum.rest_que=sum.rest_que;
	  
    set_f_sumtable_u_sql(&mysum,sql);
    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",mysum.branch,mysum.kind,mysum.item_num);
    strcat(sql,cond);
    if(mysql_query(conn2,sql))
    {
      sprintf(sys_err_str,"修改本年总帐错!%s",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }	
	
	  note("将本日总帐90清零");
	  sum.det_amt=0;
	  sum.cre_amt=0;
	  sum.open_que=0;
	  sum.cls_que=0;
	  sum.dvou_que=0;
	  sum.cvou_que=0;
	  sum.kind=90;
	  
    set_f_sumtable_u_sql(&sum,sql);
    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",sum.branch,sum.kind,sum.item_num);
    strcat(sql,cond);
    if(mysql_query(conn2,sql))
    {
      sprintf(sys_err_str,"修改本日总帐错!%s",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
	
		  note("年底12月31日");
		  if(mdy_today[1]==31&&mdy_today[0]==12) 
		  {
		    note("年底12月31日时,将本年99写入上年0记录");
		    mysum.kind=0;
		    
	    set_f_sumtable_u_sql(&mysum,sql);
	    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",mysum.branch,mysum.kind,mysum.item_num);
	    strcat(sql,cond);
	    if(mysql_query(conn2,sql))
	    {
	      sprintf(sys_err_str,"修改本年总帐错!%s",mysql_error(conn2));
	      dis_str(22,22,sys_err_str,60,0);
	      mysql_rollback(conn2);
        mysql_free_result(res);
	      getch();
	      return(-1);
	    }
	
	    sum.balance=0;
	    sum.rest_que=0;
	    sum.kind=99;
	    note("清本年99记录");
	    
	    set_f_sumtable_u_sql(&sum,sql);
	    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",sum.branch,sum.kind,sum.item_num);
	    strcat(sql,cond);
	    if(mysql_query(conn2,sql))
	    {
	      sprintf(sys_err_str,"清本年99记录总帐错!%s",mysql_error(conn2));
	      dis_str(22,22,sys_err_str,60,0);
	      mysql_rollback(conn2);
        mysql_free_result(res);
	      getch();
	      return(-1);
	    }
	  
	    note("清1-12月99记录");
	    set_f_sumtable_u_sql(&sum,sql);
	    int sqllen=strlen(sql);
	    char *sqlcond=sql+sqllen;
	    for(sum.kind=1;sum.kind<=12;sum.kind++)
	    {
	      sprintf(sqlcond,"branch=%d and kind=%d and item_num=%ld",sum.branch,sum.kind,sum.item_num);
	      //strcat(sql,cond);
		    if(mysql_query(conn2,sql))
		    {
		      sprintf(sys_err_str,"清1-12月99记录总帐错!%s",mysql_error(conn2));
		      dis_str(22,22,sys_err_str,60,0);
		      mysql_rollback(conn2);
          mysql_free_result(res);
		      getch();
		      return(-1);
		    }
	    }
	   }

    }
    mysql_free_result(res);


    //转换凭证文件昨日数据
    sprintf(sql,"select * from vouch where kind=90 and branch=%d",da.tr_br);;
    if(mysql_query(conn,sql))
    {
      sprintf(sys_err_str,"修改凭证文件错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      getch();
      return(-1);
    }
    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      sprintf(sys_err_str,"没有凭证!");
      dis_str(22,22,sys_err_str,60,0); 
      mysql_rollback(conn2);
      getch();
      return(-1);
		}
    
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_f_vouch(&vou,row);
      
      vou.kind=91; 
      sprintf(sql,vouch_u_bkt,
        vou.branch,vou.kind,vou.flag,vou.vou_type,vou.rest_que,vou.use_que,
        vou.cancel,vou.chg_book,vou.takeback,da.tr_br,vou.kind,vou.vou_type);
      
      if(mysql_query(conn2,sql))
      {
        sprintf(sys_err_str,"修改昨日凭证文件错!%s",mysql_error(conn2));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn2);
        mysql_free_result(res);
        getch();
        return(-1);
      }

	  note("取本凭证的本月记录");
	  vou.kind=mdy_today[0];
	
	  sprintf(sql,"select * from vouch where branch=%d and kind=%d and vou_type='%s'",
	    vou.branch,vou.kind,vou.vou_type);
	
	  if(mysql_query(conn2,sql))
	  {
	     sprintf(sys_err_str,"取本月记录错!%s",mysql_error(conn2));
	     dis_str(22,22,sys_err_str,60,0);
       mysql_rollback(conn2);
       mysql_free_result(res);
	     getch();
	    return(1);
	  }
    res2=mysql_store_result(conn2);
    row2=mysql_fetch_row(res2);
		if(row2==NULL){
      sprintf(sys_err_str,"取本月记录错!");
      dis_str(22,22,sys_err_str,60,0); 
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
		}
    set_f_vouch(&myvou,row2);
    mysql_free_result(res);
    
  //myvou.open_que+=vou.open_que;
  myvou.cancel+=vou.cancel;
  myvou.chg_book+=vou.chg_book;
  myvou.takeback+=vou.takeback;
  myvou.rest_que=vou.rest_que;
  
  sprintf(sql,vouch_u_bkt,
    myvou.branch,myvou.kind,myvou.flag,myvou.vou_type,myvou.rest_que,myvou.use_que,
    myvou.cancel,myvou.chg_book,myvou.takeback,myvou.branch,myvou.kind,myvou.vou_type);
  
  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"修改本月总帐错!%s",mysql_error(conn2));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn2);
    mysql_free_result(res);
    getch();
    return(-1);
  }

  vou.kind=99;
  
    sprintf(sql,"select * from vouch where branch=%d and kind=%d and vou_type='%s'",
      vou.branch,vou.kind,vou.vou_type);;
    if(mysql_query(conn2,sql))
    {
      sprintf(sys_err_str,"取99表外总帐错!%s",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
    res2=mysql_store_result(conn2);
    row2=mysql_fetch_row(res2);
		if(row2==NULL){
      sprintf(sys_err_str,"取99表外总帐错!");
      dis_str(22,22,sys_err_str,60,0); 
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
		}
    set_f_vouch(&myvou,row2);
		
//?  myvou.open_que+=vou.open_que;
  myvou.cancel+=vou.cancel;
  myvou.chg_book+=vou.chg_book;
  myvou.takeback+=vou.takeback;
  myvou.rest_que=vou.rest_que;
  
  sprintf(sql,vouch_u_bkt,
    myvou.branch,myvou.kind,myvou.flag,myvou.vou_type,myvou.rest_que,myvou.use_que,
    myvou.cancel,myvou.chg_book,myvou.takeback,myvou.branch,myvou.kind,myvou.vou_type);
  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"修改99表外总帐错!%s",mysql_error(conn2));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn2);
    mysql_free_result(res);
    getch();
    return(-1);
  }

  //vou.open_que=0;
  vou.cancel=0;
  vou.use_que=0;
  vou.chg_book=0;
  vou.takeback=0;
  vou.kind=90;
  
  sprintf(sql,vouch_u_bkt,
    vou.branch,vou.kind,vou.flag,vou.vou_type,vou.rest_que,vou.use_que,
    vou.cancel,vou.chg_book,vou.takeback,vou.branch,vou.kind,vou.vou_type);
  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"清90表外总帐错!%s",mysql_error(conn2));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn2);
    mysql_free_result(res);
    getch();
    return(-1);
  }
  
  if(mdy_today[1]==31&&mdy_today[0]==12)
  {
    myvou.kind=0;
	  sprintf(sql,vouch_u_bkt,
	    myvou.branch,myvou.kind,myvou.flag,myvou.vou_type,myvou.rest_que,myvou.use_que,
	    myvou.cancel,myvou.chg_book,myvou.takeback,myvou.branch,myvou.kind,myvou.vou_type);
	  if(mysql_query(conn2,sql))
	  {
	    sprintf(sys_err_str,"清年表外总帐错!%s",mysql_error(conn2));
	    dis_str(22,22,sys_err_str,60,0);
	    mysql_rollback(conn2);
      mysql_free_result(res);
	    getch();
	    return(-1);
	  }

    vou.rest_que=0;
    vou.kind=99;
	  sprintf(sql,vouch_u_bkt,
	    vou.branch,vou.kind,vou.flag,vou.vou_type,vou.rest_que,vou.use_que,
	    vou.cancel,vou.chg_book,vou.takeback,vou.branch,vou.kind,vou.vou_type);
	  if(mysql_query(conn2,sql))
	  {
	    sprintf(sys_err_str,"修改年99表外总帐错!%s",mysql_error(conn2));
	    dis_str(22,22,sys_err_str,60,0);
	    mysql_rollback(conn2);
      mysql_free_result(res);
	    getch();
	    return(-1);
	  }

    for(vou.kind=1;vou.kind<=12;vou.kind++)
    {
		  sprintf(sql,vouch_u_bkt,
		    vou.branch,vou.kind,vou.flag,vou.vou_type,vou.rest_que,vou.use_que,
		    vou.cancel,vou.chg_book,vou.takeback,vou.branch,vou.kind,vou.vou_type);
		  if(mysql_query(conn2,sql))
		  {
		    sprintf(sys_err_str,"修改月总帐错!%s",mysql_error(conn2));
		    dis_str(22,22,sys_err_str,60,0);
		    mysql_rollback(conn2);
        mysql_free_result(res);
		    getch();
		    return(-1);
		  }
    }
  }
}
  mysql_free_result(res);

    note("将本所打印标志,记总帐标志和流水清零,初始化标志置1");
    
	  sprintf(sql,"update br_file set sum_flag=0,pr_flag=0,ser_num=0,init_flag=1");
	  if(mysql_query(conn2,sql))
	  {
	    sprintf(sys_err_str,"修改本所文件错!%s",mysql_error(conn2));
	    dis_str(22,22,sys_err_str,60,0);
	    mysql_rollback(conn2);
	    getch();
	    return(-1);
	  }

    note("将本所流水清空!");
    
	  sprintf(sql,"delete from trade where branch=%d",da.tr_br);
	  if(mysql_query(conn2,sql))
	  {
	    sprintf(sys_err_str,"清空本所日志错!%s",mysql_error(conn2));
	    dis_str(22,22,sys_err_str,60,0);
	    mysql_rollback(conn2);
	    getch();
	    return(-1);
	  }

    mysql_commit(conn2);
     break;
  }
  dis_err("处理完毕！");
  return(0);
}

dis_err_date(long dd)
{
  short ymd[3];
  char tmp_str[20];

  rjulmdy(dd,ymd);
  sprintf(tmp_str,"%d年%d月%d日",ymd[2],ymd[0],ymd[1]);
  dis_err(tmp_str);
}
