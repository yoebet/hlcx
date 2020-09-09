
//$include "/usr/informix/incl/esql/decimal.h";

#ifndef SH_TABLE_H
#define SH_TABLE_H

#define TAXRATE			0      /*所得税率 	*/
#define STARTTAX		36464   /*所得税起征日	*/

/********************************************************************
  名称:input_data 数据输入结构
  功能:定义所有数据输入内容
  说明:应为全局变量
*********************************************************************/
struct in_data
{
  short  open_br;      /*开户所号*/
  short  open_op;      /*开户柜员号*/
  short  tr_br;   /*业务所号*/
  short  tr_op;   /*业务柜员号*/
  short  operator;    /*柜员号*/
  char   level[2];    /*柜员权限*/
  short  ser_num;      /*流水号*/
  short  detail_num;  /*明细笔数*/
  short  code;        /*交易码*/
  char   pwd_old[7];  /*原密码*/
  char   pwd_new1[7];  /*新密码*/
  char   pwd_new2[7];  /*新密码*/
  char   correct[2];  /*状态: A正常 K冲正 */
  char   acc_type[2]; /*帐户类型:*/
  char   ltype[2];
  char   acc_flag[2]; /*帐户状态:*/
  char   abstract[2]; /*摘要: 0无发生额 1现金收2转帐收 3现金付4转帐付 */
  char   vou_type[3]; /*凭证类型: **/
  long   vou_num;     /*凭证号(办理业务的凭证号) **/
  long   account;     /*帐号*/
  long   new_acc;     /*部提新开新帐号*/
  long   que_acc;     /*查询帐号*/
  long   account1;     /*存款帐号*/
  long   account2;     /*存款帐号*/
  long   item;        /*科目号*/
  short  year;        /*年份*/
  char   name[9];/*柜员户名*/
  char   customer[13];/*户名*/
  char   open_name[21];/*单位名称*/
  char   address[31];/*地址*/
  long   transfer;   /*对转帐号*/
  long   other_item;  /*对转科目号*/
  long   other_br;    /*对转所号(同兑后有否则为本所)*/
  long   det_amount;  /*贷方发生额:有哪方记哪方,另一方记为零 */
  long   cre_amount;  /*借方发生额*/
  long   amount;      /*发生额*/
  long   balance;     /*当前余额 */
  long   interest;    /*销户利息**/
  long   keep_int;    /*保值利息**/
  long   t_date;      /*业务日期*/
  long   open_d;      /*开户日期*/
  long   k_date;      /*冲帐日期 */
  short  term;        /*存期:定期/零整户*/
  char   int_flag[2]; /*计息标志0按季(缺省)*/
  char   q_flag[2];   /*查询方式*/
  long   sum_flag; 
  short  vou_flag; 
  short  sum_que; 
  short  item_kind; 
  short  home_que; 
  short  tr_code; 
  short  ser_num1; 
  short  ser_num2; 
  char   br_name[11];
  short  oper; 
  char   item_name[21];
  char   vou_name[11];
  short  prt_ser; 
  char   vou_code[3];
  char   rate_type[2];
  long   rate; 
  short  month;     
  short  bal_flag;     
  double det_amt;    
  double cre_amt;    
  short  det_num;    
  short  cre_num;    
  short  open_num;    
  short  close_num;    
  short  rest_num;    
  long   tax_int;    /*利息税**/
  char   zj_kind[2];
  char   zj_num[22];
  char   card_acc[20];
  char   card_acc1[20];
  char   card_acc2[20];
};

/********************************************************************
4 名称:operator 操作员文件 
  功能:记载所有操作员
  索引:opeartor
*********************************************************************/

struct f_operator
{
  short   operator;     /*操作员号(1-n),计息转帐柜员号为(99)*/
  char    password[9]; /*密码(11位)*/
  char    op_level[2];  /*权限 1记帐员7主机员4主任*/
  char    op_name[9];   /*操作员名*/ 
  char    op_flag[2];   /*标志 A正常 K作废*/
  char    tty_no[7];    /*登录终端号:0签退 tty21a签到终端*/
  short    tty_pid;    /*登录终端进程号:0签退 */
  short    ser_num;
};

/********************************************************************
  名称:branch 所文件 
  功能:记载有关某所的信息
  索引:branch
*********************************************************************/

struct f_branch
{
  short branch;         /*所号*/
  char  br_name[11];    /*所名*/
  char  br_flag[2];     /*标志(A正常K作废)*/
  long  work_d;         /*工作日:当天业务日 */
  short pr_flag;        /*打表标志 0(每打一表累计一最后检查总数) */
  short sum_flag;       /*记总帐标志 0未记*/
  short init_flag;      /*初始化标志 0未初始化*/
  short   ser_num;      /*本日最终流水号,每日设工作日时清0*/
};


/********************************************************************
2 名称:rate 利率文件 
  功能:记载有关存款年利率
  索引:ltype,term,int_date
*********************************************************************/

struct f_rate
{
  char  ltype[2];  /*类型:H_活  D_定*/
  short term;      /*存期(按月计算如1年为12) 活期为0*/
  long  int_date;  /*调整利息时间*/
  long  rate;      /*年利率值.
                     将原年息扩大1万倍
                     原年利率*10000(为了转换为月日率时仍为整数)
                     便于存储,
                   */
};


/********************************************************************
  名称:item 科目号文件
  功能:记载所有科目的有关数据
  索引:item
*********************************************************************/

struct f_item
{
  char   type[2];       /*储种: D_定 H_活 */
  char   acc_type[2];   /*性质 H活折 D定期 */
  short  term;          /*存期 */
  long   item_num;      /*科目号(999999为合计项不打印) */
  char   item_name[21]; /*科目名称 */
  char   item_flag[2];  /*状态:A正常 K作废*/
  long   sum_flag;      /*统计标志
                          999999 合计
                          N99999 小计(类别为N的小计项)
                          NXX999 N类X项小计(类别为N的第X项内容)
                          NXXmmm基本项(类别为N的第X类的第mm项内容,无9)

                          999999=199999+299999+...+N99999
                          N9999=N01999+N02999+...+N98999
                          NXX999=NXX01+NXX02+...+NXX98
                          统计时若本项只有一项,如现金,则设为101999,
                          而它无子项,对这种情况将本身科目类加即可.
                          另外初始化时,将所有读出科目都清零,这样所有
                          小计和合计都可先将本身科目类加 */
  short  prt_ser;       /*打印顺序号(出表时使用)*/
};


/********************************************************************
  名称:special 特殊操作文件 
*********************************************************************/
struct f_special
{
  short  branch;          /*所号*/
  short  operator;        /*操作员号*/
  short  code;            /*交易码*/
  char   modi_tab[11];    /*修改表名*/
  long   modi_acc;        /*修改帐号*/
  char   old_content[21]; /*修改前内容*/
  char   new_content[21]; /*修改后内容*/
  long   s_date;          /*修改日期*/
  char   note[31];        /*注释*/
};

/********************************************************************
*   双整表结构:   sf_mast                                           *
********************************************************************/
struct sf_mast {
	short	open_br;	/* 开户所号		*/
	long	account;	/* 帐号		*/
	char	customer[13];	/* 户名		*/
	char	address[31];	/* 地址		*/
	long	open_d;		/* 开户日	*/
	long	close_d;	/* 销户日	*/
	short	term;		/* 存期		*/
	long	amount;		/* 金额		*/
	long	interest;	/* 利息		*/
	char	acc_type[2];	/* 帐户性质
				     'F': 定期*/
	char	acc_flag[2];	/* 帐户状态
				     'A': 正常
				     'l': 口头挂失
				     'L': 挂失
				     'F': 冻结
				     'C': 已销户
				     'K': 作废
				     'G': 挂失补开
				     'B': 部提	*/
	long	transfer;	/* 转户帐号	*/
	short	open_op;	/* 开户柜员号	*/
	short	close_br;	/* 销户所号	*/
	short	close_op;	/* 销户柜员号	*/
	long	item_num;	/* 科目号	*/
	char	abstract[2];	/* 摘要 0现金 1转收 2转付  */
	char	withdraw[2];	/* 部分提支标志
				     ' ': 正常户
		     'Y': 已部分提支户*/
	long	rate;		/* 利率		*/
        long    tax_int;	/* 利息税*/
	};



/********************************************************************
*   活期表结构:   ss_mast                                           *
********************************************************************/
struct ss_mast {
      short         open_br;                  /* 开户所号 */
      long          account;                 /* 活期存折帐号 */
      char          acc_type[2];             /* 活折性质 Z_正常 W_未开*/
      char          card_acc[20];            /* 卡号 */
      char          card_type[2];            /* 卡性质 Z_正常 W_未开*/
      char          customer[13];             /* 户  名 */
      char          address[31];             /* 地  址 */
      char          passwd[7];             /*  */
      char          zj_kind[2];             /*证件 */
      char          zj_num[22];             /* */
      long          acc_open_d;              /* 折开户日 */
      long          card_open_d;                  /* 卡开户日 */
      long          acc_close_d;                 /* 折销户日 */
      long          card_close_d;                 /* 卡销户日 */
      long          balance;                 /* 实际余额 */
      short         det_num;                 /* 明细笔数 */
      long          interest;                /* 利息积数 */
      long          tax_int;	/* 利息税*/
      char          acc_flag[2];             /* 帐户状态 Z_正常 X_销户 G_挂失 D_冻结 */
      long          transfer;                /* 转帐帐号 */
      long          item_num;                /* 科目号 */
      long          last_day;                /* 最后业务日 */
      short         open_op;                  /* 开户所号 */
      short         close_br;                  /* 销户所号 */
      short         close_op;                  /* 销户操作员 */
};

/*********************************************************************
*     活期明细表结构:  ss_detail                                     *
*********************************************************************/
struct ss_detail
{
  short  open_br;      /* 所  号 */
  long   account;      /* 存折帐号*/
  char   card_acc[20]; /*卡号 */
  short  det_num;      /* 明细顺序数*/
  long   tr_date;      /* 交易日期 */
  long   amount;       /* 发生额 */
  char   abstract[2];  /* 摘要 (0现金 1_转帐)*/
  char   flag[2];      /* 状态 (O开户D存W取C销K冲正 Q_扣税 I_利息)*/
  short  tr_br;        /* 业务所号 */
  short  tr_op;        /* 业务柜员号 */
  long   balance;      /* */
};

/********************************************************************
  日志表结构:   s_trade                                          *
  名称:trade 日志文件 
  索引:branch,opeartor,sernum
*********************************************************************/
struct s_trade {
	short	branch;		 /* 业务所号     */
	short	operator;	 /* 柜员号 */
	short	ser_num;		 /* 交易序号 */
	short	tr_op;	 /* 柜员号 */
	short	tr_code;                 /* 操作码     */
	long	amount;                  /* 发生额     */
	char	correct[2];              /* 冲正标志   */
	char	type[2];                 /* 储  种     */
	short	term;                    /* 存  期     */
	short	open_br;                 /* 开户所号   */
	long	account;                 /* 存折帐号     */
	char	card_acc[20];                 /* 卡号     */
	char	acc_type[2];             /* 帐户性质   */
	char	customer[13];             /* 户  名     */
	char	abstract[2];             /* 摘  要     */
	long	open_d;                  /* 开户日     */
	long	close_d;                 /* 销户日     */
	long	balance;                 /* 余  额     */
	long	interest;                /* 利  息     */
	long	s_int;               /* 活期利息     */
	long	tax_int;               /* 利息税     */
	long	transfer;                /* 转户帐号   */
	short	det_num;		 /* 明细笔数   */
	long	item_num;		 /* 科目号 */

};

/********************************************************************
  名称:重要空白凭证表结构:  f_vouch
  功能:记载所有表外科目的有关数据
  索引:branch,kind,item
*********************************************************************/
struct f_vouch
{
  short branch;	    /*所号*/
  short kind;  	    /*标志*/
  char flag[2];     /*状态 A正常 K作废*/
  char vou_type[3]; /*凭证类型*/
  long rest_que;    /*剩余数*/
  long use_que;     /*开户数/使用数*/
  long cancel;	    /*作废数*/
  long chg_book;    /*换折数*/
  long takeback;    /*交回数*/
};

/********************************************************************
  名称:重要空白凭证表结构:  f_voufile
  功能:记载所有表外科目
  索引:vou_type
*********************************************************************/
struct f_voufile
{
  char  vou_type[3]; /*凭证类型*/
  char  vou_name[11];/*凭证名称*/
  char  flag[2];     /*状态 A正常 K作废*/
  char  type[2];  /*对应储种*/
  long  sum_flag;      /*统计标志*/
  short prt_ser;      /*打印顺序*/
};
/********************************************************************
  名称:sumtotal 总帐文件 
  功能:记载各所所有科目的有关数据
  索引:branch+kind+item
*********************************************************************/
struct f_sumtable
{
  short branch;	  /* 所号      */
  short kind;     /* 标志 0_上年 1-12各月 90_本日 91_上日 99_本年*/
                  /* 标志 101-131_1-31日*/
  long item_num;  /* 科目号    */
  char flag[2];   /* 状态 A正常 K作废*/
  double det_amt; /* 贷方发生额*/
  double cre_amt; /* 借方发生额*/
  long open_que;  /* 开户数    */
  long cls_que;	  /* 销户数    */
  long dvou_que;  /* 贷方传票数*/
  long cvou_que;  /* 借方传票数*/
  double balance; /* 结存余额*/
  long rest_que;  /* 结存户数*/
};

/********************************************************************
  名称:item_amt 科目发生额文件 
  功能:记载科目发生情况,若帐平表对由此更新总帐.
  索引:branch,item
*********************************************************************/
struct f_item_amt
{
  short  branch;    /* 所号*/
  long   item;      /* 科目号 */
  short  m_det_num; /* 现金收笔数 */
  short  m_cre_num; /* 现金付笔数 */
  long   m_det_amt; /* 现金收金额 */
  long   m_cre_amt; /* 现金付金额 */
  short  t_det_num; /* 转帐收笔数 */
  short  t_cre_num; /* 转帐付笔数 */
  long   t_det_amt; /* 转帐收金额 */
  long   t_cre_amt; /* 转帐付金额 */
};
 
struct to_sum
{
  long   item_num;   /*科目号*/
  char   item_name[21];   /*科目名称*/
  short  sum_flag;    /*统计标志*/
  short  prt_ser;    /*打印顺序号*/
  short  j_x_num;    /*借方现金笔数*/
  long   j_x_amt;    /*借方现金金额*/
  short  j_z_num;    /*借方转帐笔数*/
  long   j_z_amt;    /*借方转帐金额*/
  short  d_x_num;    /*贷方现金笔数*/
  long   d_x_amt;    /*贷方现金金额*/
  short  d_z_num;    /*贷方转帐笔数*/
  long   d_z_amt;    /*贷方转帐金额*/
  short  open_num;   /*开户数*/
  short  close_num;  /*销户数*/
  double   yes_bal;    /*昨日余额*/
  double   today_bal;  /*今日余额*/
  short   yes_home;    /*昨日余户*/
  short   today_home;  /*今日余户*/
};

struct to_vou
{
  char vou_type[3]; /*凭证类型*/
  char vou_name[11];/*凭证名称*/
  char  type[2];    /*对应储种*/
  long  sum_flag;   /*统计标志*/
  short prt_ser;    /*打印顺序*/
  long rest_que;    /*剩余数*/
  long use_que;     /*开户数/使用数*/
  long cancel;      /*作废数*/
  long chg_book;    /*换折数*/
  long takeback;    /*交回数*/
};


/*********************************************************************
*  支票表分户结构:  z_mast                                           *
*********************************************************************/
struct sz_mast
{
   short  open_br;          /* 开户所号*/
   long  account;         /* 开户帐号*/
   char   open_name[21];   /* 开户单位名称*/
   char   open_bank_name[21];  /* 电话号码*/
   long   open_amt;        /*开户金额*/
   long   amt;             /*最后业务发生额*/
   long   balance;         /*余额*/
   long   item_num;        /* 开户科目号/总号    */
   long   other_item;      /* 对方科目号*/
   char   address[31];     /* 开户单位地址*/
   char   open_flag[2];    /* 开户标志 0:现金1:转帐(进帐单)*/
   long   open_d;          /* 开户日 */
   long   close_d;         /* 销户日 */
   short  det_num;         /* 业务笔数 */
   long   interest;        /* 利息积数 */
   char   acc_type[2];     /* 帐户性质 Z 支票*/
   char   acc_flag[2];     /* 帐户状态 */
   long   last_day;        /* 最后业务日 */
   short  operator;        /* 柜员号*/
	short	open_op;	/* 开户操作员*/
  long      transfer;   /* 转帐帐号 */
	short	close_br;	/* 销户所号	*/
	short	close_op;	/* 销户柜员号	*/
};

/*********************************************************************
*  支票表明细结构:  z_detail                                           *
*********************************************************************/
struct sz_detail
{
   short  open_br;          /* 开户所号*/
   long   account;         /* 开户帐号*/
   short  code;    
   char   open_name[21];       /* 开户单位名称*/
   char   open_bank_name[21];  /* 开户行名称/电话号码*/
   char   open_address[31];       /* 开户单位地址*/
   long   o_account;        /* 对方帐号*/
   char   other_name[21];  /* 对方单位名称*/
   char   o_bank_name[21]; /* 对方行名称/转帐原因*/
   char   address[31];     /* 对方地址*/
   long   amount;          /*发生额*/
   long   balance;         /*本户现余额*/
   long   interest;        /* 利息积数 */
   short  det_num;         /* 业务笔数 */
   long   tr_date;         /* 业务日 */
   char	  abstract[2];	   /* 摘要   '0': 现金开户*/
				/*   '1': 转帐开户*/
				/*   'K': 冲正	*/
   long   item_num;        /* 开户科目号/z总号    */
   long   other_item;        /* 对方科目号*/
   short  tr_br;		/* 业务所号	*/
   short  operator;	/* 柜员号	*/
   long	  sxfr;	/* 手续费*/
   char	  flag[2];	/* 发生额方向  1借方2.贷方*/
   short	tr_op;	    /* 柜员号	*/
};

/*******************************************************************
* 所得税文件结构:s_tax                                       *
********************************************************************/
struct	s_tax 
{
	short branch ;	/* 所号 */
	long	account;	/* 存折号 */
	long	card_acc;	/* 卡号 */
	char	customer[13];	/* 姓名 */
	long	open_d;		/* 开户日  */
	long	start_d;	/* 起息日  */
	long	close_d;	/* 销户日  */
	long	end_d;		/* 止息日  */
	short	term;		/* 存期    */
	long	balance;	/* 余额*/
	long	interest;	/* 利息*/
	char	acc_type[2];	/* 帐户性质*/
	char	acc_flag[2];	/* 帐户状态*/
	long	transfer;	/* 转帐帐号*/
	long	item_num;	/* 科目号*/
	long	rate;		/* 利率*/
	short	close_op;	/* 销户柜员*/
        long    tr_country;	/* 交易地区号*/
        long    taxamt;            /*所得税*/
	char	withdraw[2];	/* 部分提支标志
				     ' ': 正常户
				     'Y': 已部分提支户*/
        short   ser_num;        /*交易号*/
        long    back1;          /*备用*/
        char    back2[16];          /*备用*/
        long    back3;          /*备用*/
        char    back4[16];          /*备用*/
} ;

#endif
