
//$include "/usr/informix/incl/esql/decimal.h";

#ifndef SH_TABLE_H
#define SH_TABLE_H

#define TAXRATE			0      /*����˰�� 	*/
#define STARTTAX		36464   /*����˰������	*/

/********************************************************************
  ����:input_data ��������ṹ
  ����:��������������������
  ˵��:ӦΪȫ�ֱ���
*********************************************************************/
struct in_data
{
  short  open_br;      /*��������*/
  short  open_op;      /*������Ա��*/
  short  tr_br;   /*ҵ������*/
  short  tr_op;   /*ҵ���Ա��*/
  short  operator;    /*��Ա��*/
  char   level[2];    /*��ԱȨ��*/
  short  ser_num;      /*��ˮ��*/
  short  detail_num;  /*��ϸ����*/
  short  code;        /*������*/
  char   pwd_old[7];  /*ԭ����*/
  char   pwd_new1[7];  /*������*/
  char   pwd_new2[7];  /*������*/
  char   correct[2];  /*״̬: A���� K���� */
  char   acc_type[2]; /*�ʻ�����:*/
  char   ltype[2];
  char   acc_flag[2]; /*�ʻ�״̬:*/
  char   abstract[2]; /*ժҪ: 0�޷����� 1�ֽ���2ת���� 3�ֽ�4ת�ʸ� */
  char   vou_type[3]; /*ƾ֤����: **/
  long   vou_num;     /*ƾ֤��(����ҵ���ƾ֤��) **/
  long   account;     /*�ʺ�*/
  long   new_acc;     /*�����¿����ʺ�*/
  long   que_acc;     /*��ѯ�ʺ�*/
  long   account1;     /*����ʺ�*/
  long   account2;     /*����ʺ�*/
  long   item;        /*��Ŀ��*/
  short  year;        /*���*/
  char   name[9];/*��Ա����*/
  char   customer[13];/*����*/
  char   open_name[21];/*��λ����*/
  char   address[31];/*��ַ*/
  long   transfer;   /*��ת�ʺ�*/
  long   other_item;  /*��ת��Ŀ��*/
  long   other_br;    /*��ת����(ͬ�Һ��з���Ϊ����)*/
  long   det_amount;  /*����������:���ķ����ķ�,��һ����Ϊ�� */
  long   cre_amount;  /*�跽������*/
  long   amount;      /*������*/
  long   balance;     /*��ǰ��� */
  long   interest;    /*������Ϣ**/
  long   keep_int;    /*��ֵ��Ϣ**/
  long   t_date;      /*ҵ������*/
  long   open_d;      /*��������*/
  long   k_date;      /*�������� */
  short  term;        /*����:����/������*/
  char   int_flag[2]; /*��Ϣ��־0����(ȱʡ)*/
  char   q_flag[2];   /*��ѯ��ʽ*/
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
  long   tax_int;    /*��Ϣ˰**/
  char   zj_kind[2];
  char   zj_num[22];
  char   card_acc[20];
  char   card_acc1[20];
  char   card_acc2[20];
};

/********************************************************************
4 ����:operator ����Ա�ļ� 
  ����:�������в���Ա
  ����:opeartor
*********************************************************************/

struct f_operator
{
  short   operator;     /*����Ա��(1-n),��Ϣת�ʹ�Ա��Ϊ(99)*/
  char    password[9]; /*����(11λ)*/
  char    op_level[2];  /*Ȩ�� 1����Ա7����Ա4����*/
  char    op_name[9];   /*����Ա��*/ 
  char    op_flag[2];   /*��־ A���� K����*/
  char    tty_no[7];    /*��¼�ն˺�:0ǩ�� tty21aǩ���ն�*/
  short    tty_pid;    /*��¼�ն˽��̺�:0ǩ�� */
  short    ser_num;
};

/********************************************************************
  ����:branch ���ļ� 
  ����:�����й�ĳ������Ϣ
  ����:branch
*********************************************************************/

struct f_branch
{
  short branch;         /*����*/
  char  br_name[11];    /*����*/
  char  br_flag[2];     /*��־(A����K����)*/
  long  work_d;         /*������:����ҵ���� */
  short pr_flag;        /*����־ 0(ÿ��һ���ۼ�һ���������) */
  short sum_flag;       /*�����ʱ�־ 0δ��*/
  short init_flag;      /*��ʼ����־ 0δ��ʼ��*/
  short   ser_num;      /*����������ˮ��,ÿ���蹤����ʱ��0*/
};


/********************************************************************
2 ����:rate �����ļ� 
  ����:�����йش��������
  ����:ltype,term,int_date
*********************************************************************/

struct f_rate
{
  char  ltype[2];  /*����:H_��  D_��*/
  short term;      /*����(���¼�����1��Ϊ12) ����Ϊ0*/
  long  int_date;  /*������Ϣʱ��*/
  long  rate;      /*������ֵ.
                     ��ԭ��Ϣ����1��
                     ԭ������*10000(Ϊ��ת��Ϊ������ʱ��Ϊ����)
                     ���ڴ洢,
                   */
};


/********************************************************************
  ����:item ��Ŀ���ļ�
  ����:�������п�Ŀ���й�����
  ����:item
*********************************************************************/

struct f_item
{
  char   type[2];       /*����: D_�� H_�� */
  char   acc_type[2];   /*���� H���� D���� */
  short  term;          /*���� */
  long   item_num;      /*��Ŀ��(999999Ϊ�ϼ����ӡ) */
  char   item_name[21]; /*��Ŀ���� */
  char   item_flag[2];  /*״̬:A���� K����*/
  long   sum_flag;      /*ͳ�Ʊ�־
                          999999 �ϼ�
                          N99999 С��(���ΪN��С����)
                          NXX999 N��X��С��(���ΪN�ĵ�X������)
                          NXXmmm������(���ΪN�ĵ�X��ĵ�mm������,��9)

                          999999=199999+299999+...+N99999
                          N9999=N01999+N02999+...+N98999
                          NXX999=NXX01+NXX02+...+NXX98
                          ͳ��ʱ������ֻ��һ��,���ֽ�,����Ϊ101999,
                          ����������,����������������Ŀ��Ӽ���.
                          �����ʼ��ʱ,�����ж�����Ŀ������,��������
                          С�ƺͺϼƶ����Ƚ������Ŀ��� */
  short  prt_ser;       /*��ӡ˳���(����ʱʹ��)*/
};


/********************************************************************
  ����:special ��������ļ� 
*********************************************************************/
struct f_special
{
  short  branch;          /*����*/
  short  operator;        /*����Ա��*/
  short  code;            /*������*/
  char   modi_tab[11];    /*�޸ı���*/
  long   modi_acc;        /*�޸��ʺ�*/
  char   old_content[21]; /*�޸�ǰ����*/
  char   new_content[21]; /*�޸ĺ�����*/
  long   s_date;          /*�޸�����*/
  char   note[31];        /*ע��*/
};

/********************************************************************
*   ˫����ṹ:   sf_mast                                           *
********************************************************************/
struct sf_mast {
	short	open_br;	/* ��������		*/
	long	account;	/* �ʺ�		*/
	char	customer[13];	/* ����		*/
	char	address[31];	/* ��ַ		*/
	long	open_d;		/* ������	*/
	long	close_d;	/* ������	*/
	short	term;		/* ����		*/
	long	amount;		/* ���		*/
	long	interest;	/* ��Ϣ		*/
	char	acc_type[2];	/* �ʻ�����
				     'F': ����*/
	char	acc_flag[2];	/* �ʻ�״̬
				     'A': ����
				     'l': ��ͷ��ʧ
				     'L': ��ʧ
				     'F': ����
				     'C': ������
				     'K': ����
				     'G': ��ʧ����
				     'B': ����	*/
	long	transfer;	/* ת���ʺ�	*/
	short	open_op;	/* ������Ա��	*/
	short	close_br;	/* ��������	*/
	short	close_op;	/* ������Ա��	*/
	long	item_num;	/* ��Ŀ��	*/
	char	abstract[2];	/* ժҪ 0�ֽ� 1ת�� 2ת��  */
	char	withdraw[2];	/* ������֧��־
				     ' ': ������
		     'Y': �Ѳ�����֧��*/
	long	rate;		/* ����		*/
        long    tax_int;	/* ��Ϣ˰*/
	};



/********************************************************************
*   ���ڱ�ṹ:   ss_mast                                           *
********************************************************************/
struct ss_mast {
      short         open_br;                  /* �������� */
      long          account;                 /* ���ڴ����ʺ� */
      char          acc_type[2];             /* �������� Z_���� W_δ��*/
      char          card_acc[20];            /* ���� */
      char          card_type[2];            /* ������ Z_���� W_δ��*/
      char          customer[13];             /* ��  �� */
      char          address[31];             /* ��  ַ */
      char          passwd[7];             /*  */
      char          zj_kind[2];             /*֤�� */
      char          zj_num[22];             /* */
      long          acc_open_d;              /* �ۿ����� */
      long          card_open_d;                  /* �������� */
      long          acc_close_d;                 /* �������� */
      long          card_close_d;                 /* �������� */
      long          balance;                 /* ʵ����� */
      short         det_num;                 /* ��ϸ���� */
      long          interest;                /* ��Ϣ���� */
      long          tax_int;	/* ��Ϣ˰*/
      char          acc_flag[2];             /* �ʻ�״̬ Z_���� X_���� G_��ʧ D_���� */
      long          transfer;                /* ת���ʺ� */
      long          item_num;                /* ��Ŀ�� */
      long          last_day;                /* ���ҵ���� */
      short         open_op;                  /* �������� */
      short         close_br;                  /* �������� */
      short         close_op;                  /* ��������Ա */
};

/*********************************************************************
*     ������ϸ��ṹ:  ss_detail                                     *
*********************************************************************/
struct ss_detail
{
  short  open_br;      /* ��  �� */
  long   account;      /* �����ʺ�*/
  char   card_acc[20]; /*���� */
  short  det_num;      /* ��ϸ˳����*/
  long   tr_date;      /* �������� */
  long   amount;       /* ������ */
  char   abstract[2];  /* ժҪ (0�ֽ� 1_ת��)*/
  char   flag[2];      /* ״̬ (O����D��WȡC��K���� Q_��˰ I_��Ϣ)*/
  short  tr_br;        /* ҵ������ */
  short  tr_op;        /* ҵ���Ա�� */
  long   balance;      /* */
};

/********************************************************************
  ��־��ṹ:   s_trade                                          *
  ����:trade ��־�ļ� 
  ����:branch,opeartor,sernum
*********************************************************************/
struct s_trade {
	short	branch;		 /* ҵ������     */
	short	operator;	 /* ��Ա�� */
	short	ser_num;		 /* ������� */
	short	tr_op;	 /* ��Ա�� */
	short	tr_code;                 /* ������     */
	long	amount;                  /* ������     */
	char	correct[2];              /* ������־   */
	char	type[2];                 /* ��  ��     */
	short	term;                    /* ��  ��     */
	short	open_br;                 /* ��������   */
	long	account;                 /* �����ʺ�     */
	char	card_acc[20];                 /* ����     */
	char	acc_type[2];             /* �ʻ�����   */
	char	customer[13];             /* ��  ��     */
	char	abstract[2];             /* ժ  Ҫ     */
	long	open_d;                  /* ������     */
	long	close_d;                 /* ������     */
	long	balance;                 /* ��  ��     */
	long	interest;                /* ��  Ϣ     */
	long	s_int;               /* ������Ϣ     */
	long	tax_int;               /* ��Ϣ˰     */
	long	transfer;                /* ת���ʺ�   */
	short	det_num;		 /* ��ϸ����   */
	long	item_num;		 /* ��Ŀ�� */

};

/********************************************************************
  ����:��Ҫ�հ�ƾ֤��ṹ:  f_vouch
  ����:�������б����Ŀ���й�����
  ����:branch,kind,item
*********************************************************************/
struct f_vouch
{
  short branch;	    /*����*/
  short kind;  	    /*��־*/
  char flag[2];     /*״̬ A���� K����*/
  char vou_type[3]; /*ƾ֤����*/
  long rest_que;    /*ʣ����*/
  long use_que;     /*������/ʹ����*/
  long cancel;	    /*������*/
  long chg_book;    /*������*/
  long takeback;    /*������*/
};

/********************************************************************
  ����:��Ҫ�հ�ƾ֤��ṹ:  f_voufile
  ����:�������б����Ŀ
  ����:vou_type
*********************************************************************/
struct f_voufile
{
  char  vou_type[3]; /*ƾ֤����*/
  char  vou_name[11];/*ƾ֤����*/
  char  flag[2];     /*״̬ A���� K����*/
  char  type[2];  /*��Ӧ����*/
  long  sum_flag;      /*ͳ�Ʊ�־*/
  short prt_ser;      /*��ӡ˳��*/
};
/********************************************************************
  ����:sumtotal �����ļ� 
  ����:���ظ������п�Ŀ���й�����
  ����:branch+kind+item
*********************************************************************/
struct f_sumtable
{
  short branch;	  /* ����      */
  short kind;     /* ��־ 0_���� 1-12���� 90_���� 91_���� 99_����*/
                  /* ��־ 101-131_1-31��*/
  long item_num;  /* ��Ŀ��    */
  char flag[2];   /* ״̬ A���� K����*/
  double det_amt; /* ����������*/
  double cre_amt; /* �跽������*/
  long open_que;  /* ������    */
  long cls_que;	  /* ������    */
  long dvou_que;  /* ������Ʊ��*/
  long cvou_que;  /* �跽��Ʊ��*/
  double balance; /* ������*/
  long rest_que;  /* ��滧��*/
};

/********************************************************************
  ����:item_amt ��Ŀ�������ļ� 
  ����:���ؿ�Ŀ�������,����ƽ����ɴ˸�������.
  ����:branch,item
*********************************************************************/
struct f_item_amt
{
  short  branch;    /* ����*/
  long   item;      /* ��Ŀ�� */
  short  m_det_num; /* �ֽ��ձ��� */
  short  m_cre_num; /* �ֽ𸶱��� */
  long   m_det_amt; /* �ֽ��ս�� */
  long   m_cre_amt; /* �ֽ𸶽�� */
  short  t_det_num; /* ת���ձ��� */
  short  t_cre_num; /* ת�ʸ����� */
  long   t_det_amt; /* ת���ս�� */
  long   t_cre_amt; /* ת�ʸ���� */
};
 
struct to_sum
{
  long   item_num;   /*��Ŀ��*/
  char   item_name[21];   /*��Ŀ����*/
  short  sum_flag;    /*ͳ�Ʊ�־*/
  short  prt_ser;    /*��ӡ˳���*/
  short  j_x_num;    /*�跽�ֽ����*/
  long   j_x_amt;    /*�跽�ֽ���*/
  short  j_z_num;    /*�跽ת�ʱ���*/
  long   j_z_amt;    /*�跽ת�ʽ��*/
  short  d_x_num;    /*�����ֽ����*/
  long   d_x_amt;    /*�����ֽ���*/
  short  d_z_num;    /*����ת�ʱ���*/
  long   d_z_amt;    /*����ת�ʽ��*/
  short  open_num;   /*������*/
  short  close_num;  /*������*/
  double   yes_bal;    /*�������*/
  double   today_bal;  /*�������*/
  short   yes_home;    /*�����໧*/
  short   today_home;  /*�����໧*/
};

struct to_vou
{
  char vou_type[3]; /*ƾ֤����*/
  char vou_name[11];/*ƾ֤����*/
  char  type[2];    /*��Ӧ����*/
  long  sum_flag;   /*ͳ�Ʊ�־*/
  short prt_ser;    /*��ӡ˳��*/
  long rest_que;    /*ʣ����*/
  long use_que;     /*������/ʹ����*/
  long cancel;      /*������*/
  long chg_book;    /*������*/
  long takeback;    /*������*/
};


/*********************************************************************
*  ֧Ʊ��ֻ��ṹ:  z_mast                                           *
*********************************************************************/
struct sz_mast
{
   short  open_br;          /* ��������*/
   long  account;         /* �����ʺ�*/
   char   open_name[21];   /* ������λ����*/
   char   open_bank_name[21];  /* �绰����*/
   long   open_amt;        /*�������*/
   long   amt;             /*���ҵ������*/
   long   balance;         /*���*/
   long   item_num;        /* ������Ŀ��/�ܺ�    */
   long   other_item;      /* �Է���Ŀ��*/
   char   address[31];     /* ������λ��ַ*/
   char   open_flag[2];    /* ������־ 0:�ֽ�1:ת��(���ʵ�)*/
   long   open_d;          /* ������ */
   long   close_d;         /* ������ */
   short  det_num;         /* ҵ����� */
   long   interest;        /* ��Ϣ���� */
   char   acc_type[2];     /* �ʻ����� Z ֧Ʊ*/
   char   acc_flag[2];     /* �ʻ�״̬ */
   long   last_day;        /* ���ҵ���� */
   short  operator;        /* ��Ա��*/
	short	open_op;	/* ��������Ա*/
  long      transfer;   /* ת���ʺ� */
	short	close_br;	/* ��������	*/
	short	close_op;	/* ������Ա��	*/
};

/*********************************************************************
*  ֧Ʊ����ϸ�ṹ:  z_detail                                           *
*********************************************************************/
struct sz_detail
{
   short  open_br;          /* ��������*/
   long   account;         /* �����ʺ�*/
   short  code;    
   char   open_name[21];       /* ������λ����*/
   char   open_bank_name[21];  /* ����������/�绰����*/
   char   open_address[31];       /* ������λ��ַ*/
   long   o_account;        /* �Է��ʺ�*/
   char   other_name[21];  /* �Է���λ����*/
   char   o_bank_name[21]; /* �Է�������/ת��ԭ��*/
   char   address[31];     /* �Է���ַ*/
   long   amount;          /*������*/
   long   balance;         /*���������*/
   long   interest;        /* ��Ϣ���� */
   short  det_num;         /* ҵ����� */
   long   tr_date;         /* ҵ���� */
   char	  abstract[2];	   /* ժҪ   '0': �ֽ𿪻�*/
				/*   '1': ת�ʿ���*/
				/*   'K': ����	*/
   long   item_num;        /* ������Ŀ��/z�ܺ�    */
   long   other_item;        /* �Է���Ŀ��*/
   short  tr_br;		/* ҵ������	*/
   short  operator;	/* ��Ա��	*/
   long	  sxfr;	/* ������*/
   char	  flag[2];	/* �������  1�跽2.����*/
   short	tr_op;	    /* ��Ա��	*/
};

/*******************************************************************
* ����˰�ļ��ṹ:s_tax                                       *
********************************************************************/
struct	s_tax 
{
	short branch ;	/* ���� */
	long	account;	/* ���ۺ� */
	long	card_acc;	/* ���� */
	char	customer[13];	/* ���� */
	long	open_d;		/* ������  */
	long	start_d;	/* ��Ϣ��  */
	long	close_d;	/* ������  */
	long	end_d;		/* ֹϢ��  */
	short	term;		/* ����    */
	long	balance;	/* ���*/
	long	interest;	/* ��Ϣ*/
	char	acc_type[2];	/* �ʻ�����*/
	char	acc_flag[2];	/* �ʻ�״̬*/
	long	transfer;	/* ת���ʺ�*/
	long	item_num;	/* ��Ŀ��*/
	long	rate;		/* ����*/
	short	close_op;	/* ������Ա*/
        long    tr_country;	/* ���׵�����*/
        long    taxamt;            /*����˰*/
	char	withdraw[2];	/* ������֧��־
				     ' ': ������
				     'Y': �Ѳ�����֧��*/
        short   ser_num;        /*���׺�*/
        long    back1;          /*����*/
        char    back2[16];          /*����*/
        long    back3;          /*����*/
        char    back4[16];          /*����*/
} ;

#endif
