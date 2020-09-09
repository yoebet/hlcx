#ifndef __st102ic_h__
#define __st102ic_h__

/*  some const */
#define	T0_PROTOCOL		0
#define	T1_PROTOCOL		1

#define MAXLEN 			100      /*the buffer len*/
#define MAX_RESPOND_DELAY  	1000  /*1000ms*/
#define ACK      	       	0x60 /* Acknowledge      */
#define NACK        	    	0xE0 /* No acknowledge   */
#define STX            		0x02 /* Begin character    */
#define ETX       	      	0x03 /* End   character    */
#define COM_ERROR   	    	0xa2 /* Communication error */
#define ESC_KEY			0x1b
/**************************************************************************
 CPU IC1 Card OPERATION Command
***************************************************************************/
#define CPU_ICC1_INCOMING	0xDA
#define CPU_ICC1_OUTGOING	0xDB
#define CPU_ICC1_POWER_UP	0x6E
#define CPU_ICC1_POWER_DOWN  	0x4D
#define CPU_ICC1_EXCHANGE  	0xDF
/**************************************************************************
 CPU IC2 Card OPERATION Command
***************************************************************************/
#define CPU_ICC2_INCOMING	0xE4
#define CPU_ICC2_OUTGOING	0xE5
#define CPU_ICC2_POWER_UP	0xE1
#define CPU_ICC2_POWER_DOWN  	0xE2
#define CPU_ICC2_EXCHANGE  	0xE6
/**************************************************************************
 CPU IC3 Card OPERATION Command
***************************************************************************/
#define CPU_ICC3_INCOMING	0xE4
#define CPU_ICC3_OUTGOING	0xE5
#define CPU_ICC3_POWER_UP	0xE1
#define CPU_ICC3_POWER_DOWN  	0xE2
#define CPU_ICC3_EXCHANGE  	0xE6
/**************************************************************************
 SAM IC1 Card OPERATION Command
***************************************************************************/
#define SAM_ICC1_INCOMING	0xDC
#define SAM_ICC1_OUTGOING	0xDD
#define SAM_ICC1_POWER_UP	0x6F
#define SAM_ICC1_POWER_DOWN	0x4E
#define SAM_ICC1_EXCHANGE  	0xE0
/**************************************************************************
 SAM IC2 Card OPERATION Command
***************************************************************************/
#define SAM_ICC2_INCOMING	0xE4
#define SAM_ICC2_OUTGOING	0xE5
#define SAM_ICC2_POWER_UP	0xE1
#define SAM_ICC2_POWER_DOWN	0xE2
#define SAM_ICC2_EXCHANGE  	0xE6
/**************************************************************************
 SAMCPU IC2 IC3 IC4 Card PUBLIC Command
***************************************************************************/
#define SAMCPU_ICC_INCOMING	0xE4
#define SAMCPU_ICC_OUTGOING	0xE5
#define SAMCPU_ICC_POWER_UP	0xE1
#define SAMCPU_ICC_POWER_DOWN	0xE2
#define SAMCPU_ICC_EXCHANGE  	0xE6

/* Command byte */
/*Memory Card*/
#define MEM_ICC_POWER_UP	0x10 /* Power up */
#define MEM_ICC_POWER_DOWN	0x11 /* Power down */
#define MEM_ICC_SELECT_CARD	0x20 /* Select IC card type */
#define MEM_ICC_CHECK_SECURITY 	0x21 /* Check user code */
#define MEM_ICC_ERASE_CARD     	0x22 /* Erase application zone */
#define MEM_ICC_READ_CARD	0x23 /* Reading datas from card */
#define MEM_ICC_WRITE_CARD	0x24 /* Writing '0' in card */
#define MEM_ICC_ERASE_WORD	0x25 /* Writing '1' word by word */
#define MEM_ICC_CHECK_ERASEPIN	0x50 /* Writing '1' word by word */
#define MEM_ICC_READ_ERRNUM	0x51 /* Writing '1' word by word */
#define MEM_ICC_FUSE_BLOWN	0x26 /* Fuse2 blown */
#define MEM_ICC_PROT_WRT_4428	0x27 /* Used for SLE_4428 */
#define MEM_ICC_PROT_READ_4428	0x28 /* Used for SLE_4428 */
#define LED_DIS			0x2a /* LED display for STAR-717 or SATR-727 */
#define WAIT_CARD_IN    	0x2b /* Wait card in and power up, used for STAR-717 */

#define READ_KEY_CODE	0x2c /* Read Code From STAR-272 */
#define READ_KEY        	0x2d /* Read key from STAR-727 */
#define PUSH_CARD		0x30 /* FOR 727 PUSH CARD */
#define BUZZER	        	0x31 /* buzzer for STAR-727 */
#define CLS			0x32 /* clear start-727 screen */
#define INIT_CODE		0x33 /* clear start-727 screen */


/**************************************************************************
	Error code of HANDLER interface
***************************************************************************/
#define ICCER_OK     0  /* 01h Ok */
#define ICCER_UN     1  /* 01h (unavailable) the session is not available */
#define ICCER_PE     2  /* 02h (parameter error)one or more invail parameters */
#define ICCER_AC     3  /* 03h (already connected) reader already connected*/
#define ICCER_WE     4  /* 04h (write error) transmission impossible*/
#define ICCER_NC     5  /* 05h (not connected) the reader is not connected */
#define ICCER_MP     6  /* 06h (message pending) a message has been received and must be read */
#define ICCER_ME     7  /* 07h (message error) the message received is not in a vaild format*/
#define ICCER_NM     8  /* 08h (no message) no message received */
#define ICCER_ND     9  /* 09h (not disconnected) the reader is not disconnected */
#define ICCER_DRV    10 /* 0Ah (driver error) unmanaged error on COM driver */
#define ICCER_MEM    11 /* 0Bh (memory error)unmangaged error on a memory operation */
#define ICCER_ESC    12 /* 0Ch operation code nEscape not supported */

#define ICCER_IH  101   /* 65h (invalid handle) there is no session corresponding to this handle */
#define ICCER_UNR 102   /* 66h (unavailable resource) the item resource does not exist */
#define ICCER_OE  103   /* 67h (open error) opning of session impossible(link problem) */
#define ICCER_NMS 104   /* 68h (no more session avalaible) no more memory resources for a session */
#define ICCER_RB  105   /* 69h (ressource busy) the item is already reserved */
#define ICCER_SB  106   /* 6Ah (session busy) an operation is in progress in this session */
#define ICCER_IOP 107   /* 6Bh (illegal operation) the operation requested is not possible on tjis item */
#define ICCER_NMR 108   /* 6Ch (no message received) the operation has not yet terminated */
#define ICCER_AD  109   /* 6Dh (access denied) access impossible: the is no longer powered up */
#define ICCER_SI  110   /* 6Eh (session idle) no operation in progress */
#define ICCER_SL  111   /* 6Fh (session locked) session disableled by priority session */
#define ICCER_TX  112   /* 70h (Transmit error) transmission error (returned by ICCHRead) */
#define ICCER_SCR 113	/* 71h (smart card removed) Removal of card detected by HANDLER */
#define ICCER_AMEM 114	/* 72h (memory error) erreur d'allocation m‚moire */
#define ICCER_PIP 115	/* 73h (Pipe error) erreur de communication inter-process */

#define ICCEV_CXERR  1        /* connection impossible */
#define ICCEV_CXEOF  2        /* connection made */
#define ICCEV_TXHOLD 4        /* transmission disabled */
#define ICCEV_TXERR  8        /* transmission error */
#define ICCEV_RXERR  16       /* recption error */
#define ICCEV_PRERR  32       /* protocol error */
#define ICCEV_RXEOF  64       /* message received (possibly empty) */
#define ICCEV_RXTO   128      /* reception time_out */
#define ICCEV_UN     256      /* the session does not exist (no longer exists ?) */
#define ICCEV_SC     0x4000   /* change of state of ICCEV_CP since the last call */
#define ICCEV_CP     0x8000   /* card present if 1, missing if 0 */


/**************************************************************************
*		       Codes return by the function
**************************************************************************/

#define ICCE_OK			0	/* operation performed */
#define ICCE_TIMEOUT		1
#define ICCE_ESC_KEY_PRESS	2
#define ICCE_LRC_ERR		3
#define ICCE_DATAPACK_ERR	4
#define ICCE_NACK		5
#define ICCE_RESET  210	/* RESET order not performed */
#define ICCE_IN     211	/* incorrect input parameter */
#define ICCE_OUTZ   212	/* Overflow of zone (Address out of range) */
#define ICCE_RETRY  213	/* Retry impossible in write */
#define ICCE_NTROUV 214	/* Search failed */
#define ICCE_SAT    215	/* card full */
#define ICCE_BLOQ   216	/* card disabled */
#define ICCE_SEL    217 /* selection of zone failed */
#define ICCE_DEB    218 /* not enough space */
#define ICCE_NULL   219 /* Session not open */
#define ICCE_MSK    220	/* Card  not supported by the LIBRARY */
#define ICCE_SPE    221	/* Order not supported by this order */
#define ICCE_ES     222	/* Card or line status incorrect (MED abnormal,not 0,0xE7) */
#define ICCE_LINK   223	/* Error in connection with reader */
#define ICCE_1CF    224 /* 1 false code submitted to card */
#define ICCE_2CF    225 /* 2 false code submitted to card */
#define ICCE_DEBLOQ 226 /* Submission of false cardholder code during a re-enable */
#define ICCE_OKAR   227 /* Operation OK but with a retry in write */
#define ICCE_TZI    228 /* Secrect file too small for a change of PIN code(TB card only) */
#define ICCE_MEM    229 /* Memory allocation error */
#define ICCE_EX     230 /* Exceptional error */
#define ICCE_ZONE   231 /* Order not support in the entity selected */
#define ICCE_CTX    232 /* No more context available  */
#define ESC_KEY_PRESS  238

#define MAKELONG(a, b)	((LONG)(((WORD)(a)) | (((DWORD)((WORD)(b))) << 16)))
#define LOWORD(l)       ((WORD)(l))
#define HIWORD(l)       ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
#define LOBYTE(w)       ((BYTE)(w))
#define HIBYTE(w)       ((BYTE)(((WORD)(w) >> 8) & 0xFF))

typedef struct {
		 unsigned char me1;              /* 1st card status word */
		 unsigned char me2;              /* 2nd card status word */
		 unsigned int  MED;              /* reader status */
		 } STATUS, LPSTATUS;
int gsIcProtocol[10];
#endif
