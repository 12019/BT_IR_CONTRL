#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f10x.h"

#define ADC1_DR_Address  ((u32)0x40012400+0x4c)
#define M 1
#define N 12

#define SaveDataStartAddr  ((uint32_t)0x0800FFFC)

#define		ISO7816MAXWAITmS	400
#define		IRDAMAXWAITmS	3000

#define TXD_high()		GPIO_SetBits(GPIOA, GPIO_Pin_6)
#define TXD_low()		GPIO_ResetBits(GPIOA, GPIO_Pin_6)

#define	XVER 0x20 		//协议版本
#define	SVER 0x10 		//版本号

//#define	IWDGSW			//看门狗开关

#define		IWDG_SLEEP	0x1234
#define		IWDG_RUN		0x4321
#define		NORMAL		  0xffff

#define		RST_PWRON 0x01
#define		RST_IWDG	0x02

#define		QUESUC		0 
#define		QUEFULL		1
#define		QUEEMP		2
#define		QUENOEGH	3
#define		MAXOFRXQ	0x200
#define		MAXBUFFER	0x200

#define		SYSISREADY 0x01
#define		SYSNOTREADY 0x00

#define		LEDSTA_BTLINKOFF 0x00
#define		LEDSTA_BTLINKON 0x01
#define		LEDSTA_BATLOW 0x02


#define		VBAT_MAX	0x930
#define		VBAT_MIN	0x800
#define		VBAT_USE	(VBAT_MAX-VBAT_MIN)

#define 	MAXTIMEWAITBTLINK 		0x05		//MIN
#define 	MAXTIMEBTLINKEDNODATA 0x05		//MIN
#define 	MAXTIMEBATLOW					0x02		//MIN
#define 	CHECKBATISLOW					0x01		//1MIN
#define   MAXBTUARTOUT					0X05		//50ms

#define NOPARITY            USART_Parity_No 
#define EVENPARITY          USART_Parity_Even
#define ODDPARITY           USART_Parity_Odd
																										
#define CBR_110             110
#define CBR_300             300
#define CBR_600             600
#define CBR_1200            1200
#define CBR_2400            2400
#define CBR_4800            4800
#define CBR_9600            9600
#define CBR_14400           14400
#define CBR_19200           19200
#define CBR_38400           38400

#define		TRUE	1
#define		FALSE	0

#define COM0	0				//红外
#define COM1	1				//串口
#define COM2	2				//ESAM
#define COM3	3				//蓝牙

#define ESAM 	COM2
#define RS485 COM1
#define iRDA 	COM0
#define BL	 	COM3

#define BLINKOK			0
#define BLINKOFF		1
#define BLINKMATCH	2
#define BLINKWAIT		3

#define	GPIO_BLRST_ON()			GPIO_ResetBits(GPIOA, GPIO_Pin_12)
#define	GPIO_BLRST_OFF()		GPIO_SetBits(GPIOA, GPIO_Pin_12)

#define	GPIO_BLSET_ON()			GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define	GPIO_BLSET_OFF()		GPIO_ResetBits(GPIOB, GPIO_Pin_7)

#define	RUN_ON()		GPIO_SetBits(GPIOA, GPIO_Pin_0)
#define	RUN_OFF()		GPIO_ResetBits(GPIOA, GPIO_Pin_0)
#define	RUN_TOG()		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0)))

#define	PWR_IR_ON()			GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define	PWR_IR_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_13)

#define	PWR_BL_ON()			GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define	PWR_BL_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_14)

#define	PWR_485_ON()		GPIO_ResetBits(GPIOA, GPIO_Pin_11)
#define	PWR_485_OFF()		GPIO_SetBits(GPIOA, GPIO_Pin_11)

#define	PWR_ESAM_ON()			GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define	PWR_ESAM_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_12)

#define   GPIO_ESAMRST_OFF()  GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define   GPIO_ESAMRST_ON()  GPIO_ResetBits(GPIOA, GPIO_Pin_7)

#define LED1_1				0 != GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)
#define	LED1_0				0 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)

#define LED2_1				0 != GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)
#define	LED2_0				0 == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)



#pragma  anon_unions

typedef struct	
{   
	u8 elem[MAXOFRXQ];   
	u16 head,tail;   
}Quque;


typedef struct
{
	unsigned char yearh;
  unsigned char yearl;
  unsigned char mon;
  unsigned char date;
  unsigned char hour;
  unsigned char min;
  unsigned char sec;
}Time_Def;

typedef struct
{
	unsigned char batperl;
  unsigned char batperh;
}LeftBat;

typedef struct
{
	unsigned char Xver;
  unsigned char Sver;
	Time_Def CompilTime;
}Version;

typedef struct 
{
	unsigned char IrDAisconfed;
	unsigned char RS485isconfed;
	unsigned char SleepTime;
	unsigned char SleepTime_BatLow;
	unsigned char OutTime_IRDA;
	unsigned char OutTime_RS485;
	unsigned char Sys_ready;
}Sys_Confed;

typedef struct 
{
	unsigned int Sleep_run_Time;
	unsigned int Sleep_run_Time_BatLow;
	unsigned int Out_run_Time_IRDA;
	unsigned int Out_run_Time_RS485;
	unsigned int Check_Bat_Time;
	unsigned char BTuart_Out_Run_Time;
	unsigned char BTuart_Time_Enable;
}Sys_Runed;


typedef struct 
{
	unsigned char Port;
	unsigned char BaudRate;
	unsigned char Parity;
	unsigned char SysSleepTime;
}AFN1d_F0_t;

typedef struct 
{
	unsigned char OutTime_Thisport;
	unsigned char TransLen;
	unsigned char TransData[0xff];
}AFN1d_F12_t;
typedef struct 
{
	unsigned char OutTime_Thisport;
	unsigned char TransLen;
	unsigned char TransData[0xff];
}AFN1d_F2_t;

typedef struct 
{
	unsigned char DBAddr[6];
}AFN1d_F3_t;

typedef struct 
{
	unsigned char OutTime_Thisport;
	unsigned char DBAddr[6];
	unsigned char TransLen;
	unsigned char TransData[0xff];
}AFN1d_F4_t;

typedef struct 
{
	unsigned char DBAddr[6];
}AFN1d_F5_t;

#pragma pack(push,1)
typedef struct 
{
	union
	{
		AFN1d_F0_t afn1_f0;
		AFN1d_F12_t afn1_f12;
		AFN1d_F3_t afn1_f3;
		AFN1d_F4_t afn1_f4;
		AFN1d_F5_t afn1_f5;
	};
}AFN1d_t;
typedef struct 
{
	unsigned char ReadLen;	
}AFN2d_t;
typedef struct 
{
	unsigned char Length;
	unsigned char Contrl;
	unsigned char AFN;
	unsigned char WriteFN;
	unsigned char ReadFN;	
	union
	{
		AFN1d_t AFN1D;
		AFN2d_t AFN2D;
	};	
}DataDown;
#pragma pack(pop)

typedef struct 
{
	unsigned char flag;
	LeftBat leftbat;
	Version ver; 
}AFN2u_F0_t;

typedef struct 
{
	unsigned char SendData[0xff];
}AFN2u_F1_t;

typedef struct 
{
	unsigned char SendData[0xff];
}AFN2u_F2_t;

typedef struct 
{
	unsigned char SendRand1[0x08];
	unsigned char SendEncD1[0x08];
}AFN2u_F3_t;

typedef struct 
{
	unsigned char AFN;
	unsigned char sta;
	unsigned char SendData[0xff];
}AFN2u_F4_t;

typedef struct 
{
	unsigned char SendStas;	
}AFN2u_F5_t;

#pragma pack(push,1)
typedef struct 
{
	unsigned char AFN;
	unsigned char Fn;
	unsigned char SendLen;
	union
	{
		AFN2u_F0_t afn2_f0;
		AFN2u_F1_t afn2_f1;
		AFN2u_F2_t afn2_f2;
		AFN2u_F3_t afn2_f3;
		AFN2u_F4_t afn2_f4;
		AFN2u_F5_t afn2_f5;
	};	
}AFN2u_t;
#pragma pack(pop)

typedef struct 
{
	unsigned char len;
	unsigned char contrl;
	AFN2u_t Data_uBuf;
}Frame_ut;

#ifdef  root
	#define EXT_ 
#else
	#define EXT_  extern
#endif

EXT_ Time_Def BuildTime;
	
EXT_ Quque IrDARxQUE;
EXT_ Quque RS485RxQUE;
	
EXT_ u8 *TxBuffer1;
EXT_ u8 *TxBuffer2;
EXT_ u8 RxBuffer2[MAXBUFFER];
EXT_ u8 *TxBuffer3;
EXT_ u8 RxBuffer3[MAXBUFFER];
	
EXT_ vu16 TxCounter1;
EXT_ vu16 RxCounter1;
EXT_ vu16 TxCounter2;
EXT_ vu16 RxCounter2; 
EXT_ vu16 TxCounter3;
EXT_ vu16 RxCounter3; 

EXT_ u16 MaxNbrofTx1;
EXT_ u16 MaxNbrofTx2;
EXT_ u16 MaxNbrofTx3;

EXT_ DataDown Dat_dbuf;
EXT_ Sys_Confed Sys_config;
EXT_ Sys_Runed Sys_run;
EXT_ Frame_ut ReturnFeame;

EXT_ u8 BAT_Low_FLAG;



EXT_ u32 RZ_Counter;

EXT_ u8 BYTE;
EXT_ u8 TX_FLAG;
EXT_ u8 RX_FLAG;

EXT_ u8 CountTX;
EXT_ u8 Send_bit;
EXT_ u8 Tx_bit;
EXT_ u8 Tx_Parity;

EXT_ u8 tmp_data;
EXT_ u8 CountRX;
EXT_ u8 Receive_bit;
EXT_ u8 Rx_Start;
EXT_ u8 Rx_Parity;
EXT_ u8 Rx_Parity_tmp;

EXT_ vu16 IrDARxCounter;
EXT_ u8 IrDARxBuffer1[MAXBUFFER];

EXT_ u16 IR_BaudRate_Time;

EXT_ u8  POW_RS485;//485电源标志 1 ON 0 OFF
EXT_ u8  POW_IR;//IR电源标志 1 ON 0 OFF
EXT_ u8  POW_ESAM;//ESAM电源标志 1 ON 0 OFF

EXT_ u8  Ver_flag;//协议标志

EXT_ u8	BT_STA;
EXT_ u16 BT_BlinkT;

/** @defgroup config_Exported_Functions
  * @{
  */
/*******队列操作函数区*************/
void InitQue(Quque *q);
void ResetQue(Quque *q);
u8 IsFull(Quque *q);
u8 InsertQue(Quque *q,u8 elem);
u8 IsEmpty(Quque *q);
u8 OutQueOneByte(Quque *q,u8 *pelem);
u8 OutQue(Quque *q,u8 *pelem,u16 len);
u8 AllOutQue(Quque *q,u8 *pelem);
u16 NumOfQue(Quque *q);

/*******串口功能函数区*************/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void RTC_Configuration(void);
void USART_Configuration(void);
void ISO7816_Enable(void);
void ISO7816_Disable(void);
void SetUartState(u8 COM, u32 BaudRate,u16 Parity);
void TIM_Configuration(void);
void PWM_Disable(void);
void PWM_Enable(void);
void IWDG_Configuration(void);
void Check_Rst(void);
void SysTick_Configuration(void);
u8 Bcd2Hex(u8 b);
u8 Hex2Bcd(u8 b);
void GetBuildTime(void);
/*******UART功能函数区*************/
void USART1send(u8 *buffer,u16 len);
void USART2send(u8 *buffer,u16 len);
void USART3send(u8 *buffer,u16 len);
/*******系统处理解析相关*************/
u8 CheckHE(void);
u8 CheckSum(u8 *buffer, u8 len);
u8 CheckDB(void);

void Set_Sys(void);
u8 Set_BL(u8 *pBuffer);
void Send_req(void);
void Send_HB(void);
u16 GetTime(void);
void PowerDown(void);
void PowerUp(void);
void PowerOff(void);
void Set_BL_Enter(void);
void Set_BL_Exit(void);
void Req_Irda(void);
void Wait_BTlink(void);
void Req_BL(void);

void BT_Analysis(void);
void Time_Comp(void);


void BL_Unpack(u8 *Buffer, u16 Length);
void ESAM_Info(void);
/*******模拟串口功能函数区*************/
void IR_GPIO_Init(void);
void SendOneByte(u8 Byte);
void SendBytes(u8 *str,u8 len);

void EXTI9_5_DISABLE(void);
void EXTI9_5_ENABLE(void);

void Set_IRDA_power_ON(void);
void Set_IRDA_power_OFF(void);
void Set_RS485_power_ON(void);
void Set_RS485_power_OFF(void);
void Set_ESAM_power_ON(void);
void Set_ESAM_power_OFF(void);

void delay_nms(uint16_t time1);
void delay_nus(uint16_t time2);

void Clear_RxBuffer1(void);
void Clear_RxBuffer2(void);
void Clear_RxBuffer3(void);
void Clear_RxBufferIR(void);

void Clear_IrRxBuffer1(void);
void Clear_IrRxBuffer2(void);

void ESAM_Reset(void);
u8 Ir_wait(void);

void WriteFLAG(void);
u8 FlashWrite(u32 Data);
u8 LoadFlash(void);
void BTSet(void);
void BTSetName(u8 *Name);
	
void myADC_init(void);
u16 ADC_filter(void);
#endif

