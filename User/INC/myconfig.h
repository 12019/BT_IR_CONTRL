#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f10x.h"

#define	VERL 0x01 		//版本号低位
#define	VERH 0x00			//版本号高位

#define	IWDGSW			//看门狗开关

#define		IWDG_SLEEP	0x1234
#define		IWDG_RUN		0x4321

#define		RST_PWRON 0x01
#define		RST_IWDG	0x02



#define		QUESUC		0 
#define		QUEFULL		1
#define		QUEEMP		2
#define		QUENOEGH	3
#define		MAXOFRXQ	256

#define		VBAT_MAX	0x900
#define		VBAT_MIN	0x600
#define		VBAT_USE	(VBAT_MAX-VBAT_MIN)

#define		COMNFIR	0
#define		COMFIR	1
#define		COMTXON	2

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

#define		SLEEPTIME	10
#define		HBTIME	5
#define		IRDAMODE	0
#define		UARTMODE	1
#define		BLMODE		2

#define ADC_TIMES	0x20
#define	REQ_TIMES	0x05
#define	IRREQ_TIMES	0x10

#define COM1	1				//串口
#define COM2	2				//红外
#define COM3	3				//蓝牙

#define UART COM1
#define iRDA COM2
#define BL	 COM3

#define BLINKOK			0
#define BLINKOFF		1
#define BLINKMATCH	2
#define BLINKWAIT		3


#define	GPIO_ESAMRST_ON()		GPIO_ResetBits(GPIOA, GPIO_Pin_10)
#define	GPIO_ESAMRST_OFF()	GPIO_SetBits(GPIOA, GPIO_Pin_10)
//#define	GPIO_BLRST_ON()			GPIO_ResetBits(GPIOB, GPIO_Pin_12)
//#define	GPIO_BLRST_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_12)

#define	GPIO_BLRST_ON()			GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define	GPIO_BLRST_OFF()		GPIO_SetBits(GPIOA, GPIO_Pin_5)

#define	GPIO_BLSET_ON()			GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define	GPIO_BLSET_OFF()		GPIO_ResetBits(GPIOB, GPIO_Pin_12)

#define	RUN_ON()		GPIO_SetBits(GPIOA, GPIO_Pin_0)
#define	RUN_OFF()		GPIO_ResetBits(GPIOA, GPIO_Pin_0)
#define	RUN_TOG()		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0)))

#define	PWR_OT_ON()			GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define	PWR_OT_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_5)

#define	PWR_IR_ON()			GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define	PWR_IR_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_7)

#define	PWR_BL_ON()			GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define	PWR_BL_OFF()		GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define LED1_1				0 != GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)
#define	LED1_0				0 == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)

#define LED2_1				0 != GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)
#define	LED2_0				0 == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)



typedef struct	
	{   
	u8 elem[MAXOFRXQ];   
	u8 head,tail;   
	}Quque;
	
#ifdef  root
	#define EXT_ 
#else
	#define EXT_  extern
#endif

EXT_ u8	RxTmp[0x20];
EXT_ u8 RxData1;
EXT_ u8 RxData2;
EXT_ u8	RxTmp2[0x20];
EXT_ u8 Rx2len;
EXT_ Quque RxQUE3;
EXT_ u16 TxCounter3;
EXT_ u16 MaxNbrofTx3;
EXT_ u8 *TxBuffer3;

EXT_ u8 Com1first;
EXT_ u8 Com2first;
EXT_ u8 Com3first;

EXT_ u8 PowerOn;

EXT_ u8 timed;
EXT_ u8 sysread;
EXT_ u8 time_send;

EXT_ u16 time_sleep;

EXT_ u16 ADCConvertedValue[ADC_TIMES];	   
EXT_ u16 ADCConverted[ADC_TIMES];		   

EXT_ u8 ADCcount;						   
EXT_ u8 ADCTure;						   

EXT_ u8 Bat_Low;
EXT_ u8 HB_Time;
EXT_ u8 W_Mode;
EXT_ u8 W_BaudRate;
EXT_ u8 W_Parity;
EXT_ u8 W_Time;
EXT_ u16 W_TimeRe;
EXT_ u16 W_Bat;



EXT_ __IO uint32_t TimingDelay ;

/** @defgroup config_Exported_Functions
  * @{
  */
/*******队列操作函数区*************/
void InitQue(Quque *q); //???
u8 IsFull(Quque *q);
u8 InsertQue(Quque *q,u8 elem);
u8 IsEmpty(Quque *q);
u8 OutQue(Quque *q,u8 *pelem,u16 len);
u8 AllOutQue(Quque *q,u8 *pelem);
u16 NumOfQue(Quque *q);

/*******串口功能函数区*************/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void ADC_Configuration(void);
u16 ADC_Getvalue(void);
void RTC_Configuration(void);
void USART_Configuration(void);
void SetUartState(u8 COM, u32 BaudRate,u16 Parity);
void TIM_Configuration(void);
void PWM_Disable(void);
void PWM_Enable(void);
void IWDG_Configuration(void);
void Check_Rst(void);
void SysTick_Configuration(void);
void Delay(__IO uint32_t nTime);
u8 Bcd2Hex(u8 b);
u8 Hex2Bcd(u8 b);
/*******UART功能函数区*************/
void UARTsend(u8 ch);
void USART1send(Quque *q);
void iRDAsend(u8 ch);
void USART2send(Quque *q);
void USART3send(u8 ch);
void BLsend(u8 *buffer,u16 len);
/*******系统处理解析相关*************/

u8 CheckHE(void);
u8 CheckSum(void);
void Set_Sys(void);
u8 Set_BL(u8 *pBuffer);
void Send_req(u8 com);
void Send_HB(void);
u16 GetTime(void);
void PowerDown(void);
void PowerUp(void);
void Set_BL_Enter(void);
void Set_BL_Exit(void);
void Req_Irda(void);
void Req_BL(void);
u8 Check_BLlink(void);
#endif

