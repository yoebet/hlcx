#define SUCCESS			0	/* 成功			*/

/*   数据库操作错      */
#define ERR_DB_ERROR		1	/* 数据库错		*/
#define ERR_DB_F_MAST		2	/* 双整主文件错		*/
#define ERR_DB_L_MAST		3	/* 大额主文件错		*/
#define ERR_DB_P_MAST		4	/* 零存整取主文件错	*/
#define ERR_DB_P_DETAIL		5	/* 零存整取明细文件错	*/
#define ERR_DB_B_MAST		6	/* 定活两便主文件错	*/
#define ERR_DB_Q_MAST		7	/* 存本取息主文件错	*/
#define ERR_DB_Q_DETAIL		8	/* 存本取息文件错	*/
#define ERR_DB_S_MAST		9	/* 活期主文件错		*/
#define ERR_DB_S_DETAIL		10	/* 活期明细文件错	*/
#define ERR_DB_RATEFILE		11	/* 利率文件错		*/
#define ERR_DB_ACC_FILE		12	/* 帐号文件错		*/
#define ERR_DB_BR_FILE		13	/* 所号文件错		*/
#define ERR_DB_CEN_FILE		14	/* 办事处文件错		*/
#define ERR_DB_LOCAL		15	/* 地区号文件错		*/
#define ERR_DB_OPERFILE		16	/* 操作员文件错		*/
#define ERR_DB_VOUCH		17	/* 凭证文件错		*/
#define ERR_DB_CHANGE_ACCT	18	/* 帐号转换文件错	*/
#define ERR_DB_SUMTABLE		19	/* 总帐文件错		*/
#define ERR_DB_TRADE		20	/* 交易文件错		*/
#define ERR_DB_MOD_FILE		21	/* 操作修改文件错	*/
#define ERR_DB_INTERNAL		22	/* 内部科目号文件错	*/
#define ERR_DB_ITEM_FILE	23	/* 科目号文件错		*/
#define ERR_DB_D_MAST  		24	/* 大额文件错           */
#define ERR_DB_OPER_CTL		25	/* 操作控制文件错       */

#define ERR_LOST_OPEN           26	/*     */
#define ERR_D7B		27	/*     */
#define ERR_D8B		28	/*     */
#define ERR_D9B		29	/*     */
#define ERR_D0B		30	/*     */
#define ERR_D11B		31	/*     */
#define ERR_D12B		32	/*     */
#define ERR_D13B		33	/*     */
#define ERR_D14B		34	/*     */
#define ERR_D15B		35	/*     */
#define ERR_D16B		36	/*     */
#define ERR_D17B		37	/*     */
#define ERR_D18B		38	/*     */
#define ERR_D19B  	39	/*     */

#define ERR_DB_TRADE_MOD	40	/* 交易修改文件错	*/

#define ERR_OPTCODE		100	/* 交易代码错		*/
#define ERR_BRANCH		101	/* 所号错		*/
#define ERR_ACCOUNT		102	/* 帐号错		*/
#define ERR_PASSWORD		103	/* 密码错		*/
#define ERR_AMOUNT		104	/* 金额错		*/
#define ERR_BALANCE		105	/* 余额错		*/
#define ERR_TERM		106	/* 存期错		*/
#define ERR_START_D		107	/* 起息日错		*/
#define ERR_END_D		108	/* 止息日错		*/
#define ERR_PRT_NUM		109	/* 总号错		*/
#define ERR_ABSTRACT		110	/* 摘要错		*/
#define ERR_OPER_NO		111	/* 柜员号错		*/
#define ERR_NO_CHECK		112	/* 印鉴没检查		*/
#define ERR_ACC_FLAG		113	/* 帐户性质错		*/
#define ERR_INTEREST		114	/* 利息			*/
#define ERR_UNKOWN  		115	/* 错   */
#define ERR_NOT_WITHDREW	116	/* 不允许支取   */
#define ERR_ACTIVE		117	/* 非活动错   */
#define ERR_CRO_FLAG            118     /* 印通错               */
#define ERR_NEED_CLOSE		119	/* 已到期, 不允许划付	*/

/***add in 1997/04/   ***/
#define ERR_TR_TYPE		120	/* 定折换折类型错*/
#define ERR_NO_CLOSE		121	/* 不能销户*/
#define ERR_BOOK_FULL_1		122	/* 使用加发定折*/
#define ERR_BOOK_FULL_2		123	/* 请加发定折*/
#define ERR_LINE		124	/* 序号错定折*/
#define ERR_ID_TYPE		125	/* 证件类型错*/
#define ERR_ID_NO		126	/* 证件号错*/
#define ERR_DB_M_MAST		127  /**小额抵押贷款文件错*/
#define ERR_DB_M_DETAIL		128  /**小额抵押还款明细文件错*/
#define ERR_DB_M_BILL		129  /**小额抵押存单文件错*/
#define ERR_DB_ALREADY_LOAN     130  /*此户已抵押*/
#define ERR_DB_UNLOAN	        131  /*此户尚未抵押*/
#define ERR_DB_NAME  		132  /*户名不符*/
/***add in 1997/04/  end ***/


#define ERR_NO_ACCOUNT		200	/* 无此帐户		*/
#define ERR_NO_NEW_ACCOUNT	201	/* 无此新帐户		*/
#define ERR_NO_TRADE		202	/* 无此笔流水		*/
#define ERR_NO_DETAIL		203	/* 无此笔明细		*/
#define ERR_CORRECTED		204	/* 该交易已冲正		*/
#define ERR_UNABLE_CORRECT	205	/* 该交易不能冲正	*/
#define ERR_LOSTED		206	/* 该户已挂失或冻结	*/
#define ERR_CLOSED		207	/* 该户已销户或作废	*/
#define ERR_UNABLE_CROSS	208	/* 该户不通兑		*/
#define ERR_NON_ACCESS		209	/* 存单户不能续存,支取	*/
#define ERR_NO_ARRIVAL		210	/* 提款未到期		*/
#define ERR_L                   211     /* 该户己挂失           */
#define ERR_F                   212     /* 该户己冻结           */

#define ERR_NOT_ENOUGH_AMOUNT	300	/* 存款金额不足		*/
#define ERR_NO_TERM		301	/* 无此存期		*/
#define ERR_WITHDRAWED		302	/* 该户已提支过		*/
#define ERR_UNWRITE		303	/* 无未登折数据		*/
#define ERR_ALREADY		304	/* 已到期		*/

#define ERR_BRANCH_NOT_EXIST	400	/* 该所不存在		*/
#define ERR_BRANCH_LOGINED	401	/* 该所已登录		*/
#define ERR_BRANCH_LOGOUTED	402	/* 该所已退出		*/
#define ERR_OPERATOR_NOT_EXIST	403	/* 此操作员不存在	*/
#define ERR_OPERATOR_LEVEL_LOW	404	/* 此操作员级别太低	*/
#define ERR_OPERATOR_EXISTED	405	/* 此操作员已存在	*/
#define ERR_NO_TYPE		406	/* 无此储种		*/
#define ERR_NO_STATUS		407	/* 无此帐户状态		*/
#define ERR_NO_FIELD		408	/* 操作修改无此域	*/
#define ERR_NO_CHOICE           409     /* 无此选择             */
#define ERR_TRADE_EMPTY		410	/* 该柜员的流水为空	*/
#define ERR_SERVER_SYSTEM	411	/* 中心机系统出错	*/
#define ERR_BRANCH_NOT_LOGIN	412	/* 该所未登录		*/
#define ERR_INIT_NOT_LOGIN 	413	/*今日已做初始化不可登录*/
#define ERR_NOCOUNTRY	 	414	/*开户地区号和对方地区号不符*/
#define ERR_ACC_TYPE	 	415	/* 帐户性质错*/
#define ERR_PDZ	 		416	/* 请补登折*/
#define ERR_DB_GAS_MASTZ	417	/* 请补登折*/
#define ERR_GAS_EXIST		418	/* 请补登折*/
#define ERR_HZ			420	/* 请补登折*/
#define ERR_DKHSHZ		421	/* 请到开户所换折*/
#define ERR_HZ1			422	/* 请补登折*/
