#define SUCCESS			0	/* �ɹ�			*/

/*   ���ݿ������      */
#define ERR_DB_ERROR		1	/* ���ݿ��		*/
#define ERR_DB_F_MAST		2	/* ˫�����ļ���		*/
#define ERR_DB_L_MAST		3	/* ������ļ���		*/
#define ERR_DB_P_MAST		4	/* �����ȡ���ļ���	*/
#define ERR_DB_P_DETAIL		5	/* �����ȡ��ϸ�ļ���	*/
#define ERR_DB_B_MAST		6	/* �����������ļ���	*/
#define ERR_DB_Q_MAST		7	/* �汾ȡϢ���ļ���	*/
#define ERR_DB_Q_DETAIL		8	/* �汾ȡϢ�ļ���	*/
#define ERR_DB_S_MAST		9	/* �������ļ���		*/
#define ERR_DB_S_DETAIL		10	/* ������ϸ�ļ���	*/
#define ERR_DB_RATEFILE		11	/* �����ļ���		*/
#define ERR_DB_ACC_FILE		12	/* �ʺ��ļ���		*/
#define ERR_DB_BR_FILE		13	/* �����ļ���		*/
#define ERR_DB_CEN_FILE		14	/* ���´��ļ���		*/
#define ERR_DB_LOCAL		15	/* �������ļ���		*/
#define ERR_DB_OPERFILE		16	/* ����Ա�ļ���		*/
#define ERR_DB_VOUCH		17	/* ƾ֤�ļ���		*/
#define ERR_DB_CHANGE_ACCT	18	/* �ʺ�ת���ļ���	*/
#define ERR_DB_SUMTABLE		19	/* �����ļ���		*/
#define ERR_DB_TRADE		20	/* �����ļ���		*/
#define ERR_DB_MOD_FILE		21	/* �����޸��ļ���	*/
#define ERR_DB_INTERNAL		22	/* �ڲ���Ŀ���ļ���	*/
#define ERR_DB_ITEM_FILE	23	/* ��Ŀ���ļ���		*/
#define ERR_DB_D_MAST  		24	/* ����ļ���           */
#define ERR_DB_OPER_CTL		25	/* ���������ļ���       */

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

#define ERR_DB_TRADE_MOD	40	/* �����޸��ļ���	*/

#define ERR_OPTCODE		100	/* ���״����		*/
#define ERR_BRANCH		101	/* ���Ŵ�		*/
#define ERR_ACCOUNT		102	/* �ʺŴ�		*/
#define ERR_PASSWORD		103	/* �����		*/
#define ERR_AMOUNT		104	/* ����		*/
#define ERR_BALANCE		105	/* ����		*/
#define ERR_TERM		106	/* ���ڴ�		*/
#define ERR_START_D		107	/* ��Ϣ�մ�		*/
#define ERR_END_D		108	/* ֹϢ�մ�		*/
#define ERR_PRT_NUM		109	/* �ܺŴ�		*/
#define ERR_ABSTRACT		110	/* ժҪ��		*/
#define ERR_OPER_NO		111	/* ��Ա�Ŵ�		*/
#define ERR_NO_CHECK		112	/* ӡ��û���		*/
#define ERR_ACC_FLAG		113	/* �ʻ����ʴ�		*/
#define ERR_INTEREST		114	/* ��Ϣ			*/
#define ERR_UNKOWN  		115	/* ��   */
#define ERR_NOT_WITHDREW	116	/* ������֧ȡ   */
#define ERR_ACTIVE		117	/* �ǻ��   */
#define ERR_CRO_FLAG            118     /* ӡͨ��               */
#define ERR_NEED_CLOSE		119	/* �ѵ���, ��������	*/

/***add in 1997/04/   ***/
#define ERR_TR_TYPE		120	/* ���ۻ������ʹ�*/
#define ERR_NO_CLOSE		121	/* ��������*/
#define ERR_BOOK_FULL_1		122	/* ʹ�üӷ�����*/
#define ERR_BOOK_FULL_2		123	/* ��ӷ�����*/
#define ERR_LINE		124	/* ��Ŵ���*/
#define ERR_ID_TYPE		125	/* ֤�����ʹ�*/
#define ERR_ID_NO		126	/* ֤���Ŵ�*/
#define ERR_DB_M_MAST		127  /**С���Ѻ�����ļ���*/
#define ERR_DB_M_DETAIL		128  /**С���Ѻ������ϸ�ļ���*/
#define ERR_DB_M_BILL		129  /**С���Ѻ�浥�ļ���*/
#define ERR_DB_ALREADY_LOAN     130  /*�˻��ѵ�Ѻ*/
#define ERR_DB_UNLOAN	        131  /*�˻���δ��Ѻ*/
#define ERR_DB_NAME  		132  /*��������*/
/***add in 1997/04/  end ***/


#define ERR_NO_ACCOUNT		200	/* �޴��ʻ�		*/
#define ERR_NO_NEW_ACCOUNT	201	/* �޴����ʻ�		*/
#define ERR_NO_TRADE		202	/* �޴˱���ˮ		*/
#define ERR_NO_DETAIL		203	/* �޴˱���ϸ		*/
#define ERR_CORRECTED		204	/* �ý����ѳ���		*/
#define ERR_UNABLE_CORRECT	205	/* �ý��ײ��ܳ���	*/
#define ERR_LOSTED		206	/* �û��ѹ�ʧ�򶳽�	*/
#define ERR_CLOSED		207	/* �û�������������	*/
#define ERR_UNABLE_CROSS	208	/* �û���ͨ��		*/
#define ERR_NON_ACCESS		209	/* �浥����������,֧ȡ	*/
#define ERR_NO_ARRIVAL		210	/* ���δ����		*/
#define ERR_L                   211     /* �û�����ʧ           */
#define ERR_F                   212     /* �û�������           */

#define ERR_NOT_ENOUGH_AMOUNT	300	/* ������		*/
#define ERR_NO_TERM		301	/* �޴˴���		*/
#define ERR_WITHDRAWED		302	/* �û�����֧��		*/
#define ERR_UNWRITE		303	/* ��δ��������		*/
#define ERR_ALREADY		304	/* �ѵ���		*/

#define ERR_BRANCH_NOT_EXIST	400	/* ����������		*/
#define ERR_BRANCH_LOGINED	401	/* �����ѵ�¼		*/
#define ERR_BRANCH_LOGOUTED	402	/* �������˳�		*/
#define ERR_OPERATOR_NOT_EXIST	403	/* �˲���Ա������	*/
#define ERR_OPERATOR_LEVEL_LOW	404	/* �˲���Ա����̫��	*/
#define ERR_OPERATOR_EXISTED	405	/* �˲���Ա�Ѵ���	*/
#define ERR_NO_TYPE		406	/* �޴˴���		*/
#define ERR_NO_STATUS		407	/* �޴��ʻ�״̬		*/
#define ERR_NO_FIELD		408	/* �����޸��޴���	*/
#define ERR_NO_CHOICE           409     /* �޴�ѡ��             */
#define ERR_TRADE_EMPTY		410	/* �ù�Ա����ˮΪ��	*/
#define ERR_SERVER_SYSTEM	411	/* ���Ļ�ϵͳ����	*/
#define ERR_BRANCH_NOT_LOGIN	412	/* ����δ��¼		*/
#define ERR_INIT_NOT_LOGIN 	413	/*����������ʼ�����ɵ�¼*/
#define ERR_NOCOUNTRY	 	414	/*���������źͶԷ������Ų���*/
#define ERR_ACC_TYPE	 	415	/* �ʻ����ʴ�*/
#define ERR_PDZ	 		416	/* �벹����*/
#define ERR_DB_GAS_MASTZ	417	/* �벹����*/
#define ERR_GAS_EXIST		418	/* �벹����*/
#define ERR_HZ			420	/* �벹����*/
#define ERR_DKHSHZ		421	/* �뵽����������*/
#define ERR_HZ1			422	/* �벹����*/
