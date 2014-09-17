#ifndef ESAM_H
#define ESAM_H
#include "myconfig.h"

#define		ISO7816D	35

#define	  OPERATOR0	't'
#define	  OPERATOR1	's'
#define	  OPERATOR2	'o'
#define	  OPERATOR3	'b'

#define 	CASE1			1
#define 	CASE2			2
#define 	CASE3			3
#define 	CASE4			4
#define 	CASE5			5

#define   PRIVATESUC 		0
#define   PUBLICSUC 		1
#define 	VERIFICAFAIL	2
#define 	NORETURN			3

u8 DoVerifica(u8 *DbAddr); 	//进行电表的认证
void ReadESAMID(void);
u8 ReadESAMSta(void);
u32 ReadCounter(void);
void Get_ESAM_Data(u8 *DbAddr, u8 *Redata);
void IrDASend(u8 *buffer,u16 len);

#endif

