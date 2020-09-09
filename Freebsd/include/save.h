#define I_OPEN       'O'          /* 开户标志 */
#define I_CLOSE      'C'          /* 销户标志 */
#define I_TRANSFER   'T'          /* 转收己登折 */
#define I_DEPOSITARY 'D'          /* 现金存款 */
#define I_WITHDREW   'W'          /* 现金取款 */
#define I_CORRECT    'K'          /* 冲正,作废 */
#define I_T_UNWRITE  't'          /* 转收未登折 */
#define I_ACTIVE     'A'          /* 正常状态 */
#define I_LOST       'L'          /* 挂失状态 */
#define I_O_LOST     'l'          /* 口头挂失 */
#define I_CHANGE     'R'          /* 换折 */

#define S_TYPE       'S'          /* 活期 */
#define F_TYPE       'F'          /* 双整 */
#define P_TYPE       'P'          /* 零整 */
#define B_TYPE       'B'          /* 定活两便 */
#define Q_TYPE       'Q'          /* 存本取息 */
#define H_TYPE       'H'          /* 华侨 */
#define D_TYPE       'D'          /* 大额定期 */
#define T_TYPE       'T'          /* 通知存款 */

#define S_RATE       36000        /* 活期计息日基利率 */
#define CONT         300000000    /* 常数 */

#define AUTO_RUN     '9'          /* 新老帐号对照标志 */

#define MAX_PEND     10           /* 最大转帐笔数 */
#define START_S      001          /* 活期起存额 */
#define MY_COUNT     10           /*未满页笔数*/

#include "../include/error.h"
