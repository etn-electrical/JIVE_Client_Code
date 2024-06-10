#ifndef __FR_BVLC_H_INCLUDED
#define __FR_BVLC_H_INCLUDED

#define Register_Foreign_Device_ACK				0x00
#define BVLL_ACK								0x00	
#define Write_Broadcast_Distribution_Table_NAK	0x10	
#define Read_Broadcast_Distribution_Table_NAK	0x20	
#define Register_Foreign_Device_NAK				0x30	
#define Read_Foreign_Device_Table_NAK			0x40	
#define Delete_Foreign_Device_Table_Entry_NAK	0x50	
#define Distribute_Broadcast_To_Network_NAK		0x60	

enum FDRstates{
	fdrRegister=0,							//0	no BBMD connection yet	
	fdrWaitACK,								//1	waiting for FD ACK
	fdrRegistered,							//2	registered as FD
	fdrLocalSubnet							//3	using local subnet only	
};

typedef struct _smallbvlc {							
	octet	type;							//always 0x81 for BIP
	octet	function;
	octet	length[2];						//always 6 for smallbvlc
	octet	buf[2];
} smallbvlc;

//Function prototypes
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */

// local functions, called locally only
static void bbmd_sendResult(octet *bbmd,word resultcode);
static word fd_getfdrstate(void);
static void fd_reply(word reply);
static void FDRsecond(void);

// BVLC API functions (called by the BIP module)
void bpublic fd_second(void);
void bpublic fd_transmit(word,bvlc *);
bool bpublic bvlcWork(bvlc *,octet *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
