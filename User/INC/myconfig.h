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

#define	VERL 0x02 		//版本号低位
#define	VERH 0x00			//版本号高位

#define	IWDGSW			//看门狗开关

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
#define		MAXBUFFER	0x100

#define		MAXIRBUFLEN	50

#define		VBAT_MAX	0x930
#define		VBAT_MIN	0x800
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

#define		IRDAMODE	0
#define		UARTMODE	1
#define		BLMODE		2
#define		ESAMMODE	3

#define	REQ_TIMES	0x05
#define	IRREQ_TIMES	0x10

#define Byte_Time		50
#define Byte_Time_BL 200
 
#define COM0	0				//红外
#define COM1	1				//串口
#define COM2	2				//ESAM
#define COM3	3				//蓝牙

#define ESAM COM2
#define UART COM1
#define iRDA COM0
#define BL	 COM3

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

//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

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

//EXT_ u8	RX;
EXT_ u8	RxTmp[0x20];
EXT_ u8 RxData1;
EXT_ u8 RxData2;
EXT_ u8	RxTmp2[0x20];
EXT_ u8 Rx2len;
	
EXT_ Quque RxQUE1;
//EXT_ Quque RxQUE2;
EXT_ Quque RxQUE3;
	
EXT_ Quque IR_Buf1;
EXT_ Quque IR_Buf2;	
	
EXT_ u8 *TxBuffer1;
EXT_ u8 RxBuffer1[MAXBUFFER];
EXT_ u8 *TxBuffer2;
EXT_ u8 RxBuffer2[MAXBUFFER];
EXT_ u8 *TxBuffer3;
EXT_ u8 RxBuffer3[MAXBUFFER];
//EXT_ u8 RxBufferIR[MAXBUFFER];
	
EXT_ vu16 TxCounter1;
EXT_ vu16 RxCounter1;
EXT_ vu16 TxCounter2;
EXT_ vu16 RxCounter2; 
EXT_ vu16 TxCounter3;
EXT_ vu16 RxCounter3; 
//EXT_ vu16 RxCounterIR; 

EXT_ u16 MaxNbrofTx1;
EXT_ u16 MaxNbrofTx2;
EXT_ u16 MaxNbrofTx3;

EXT_ u8 BL_STA;//1未连接 2已连接 3电量低
EXT_ u8 LED_STA;//LED状态保存

EXT_ u8 timed;
EXT_ u8 sysread;

EXT_ u32 time_sleep;
EXT_ u16 BL_TIME;
EXT_ u16 POW_TIME; 		
EXT_ u16 M_Time;

EXT_ u8 GET_ESAM;

EXT_ u8 Bat_Low;
EXT_ u8 Bat_FLAG;
EXT_ u8 W_Mode;
EXT_ u8 W_BaudRate;
EXT_ u8 W_Parity;
EXT_ u8 W_Time;
EXT_ u8 W_Code[4];
EXT_ u16 W_Bat;
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

EXT_ u8 Buf1_FULL,Buf2_FULL;

EXT_ vu16 IrRxCounter1;
EXT_ vu16 IrRxCounter2;
EXT_ u8 IrBuf1[MAXIRBUFLEN];
EXT_ u8 IrBuf2[MAXIRBUFLEN];

EXT_ u16 IR_BaudRate_Time;

EXT_ u8  AT_FLAG;//AT模式标志
EXT_ u8  OK;//BL set flag
EXT_ u8	 TrasferMode;//0透传   3红外抄表   4ESAM
EXT_ u8  BL_REQ_FLAG;//1:第一帧读参数应答标志 2:第二帧及以后数据
EXT_ u8  IR_TC;
EXT_ u8  Rs485_TC;
EXT_ u8  IR_DB_RZ;//红外认证标志 0不认证 1认证 
EXT_ u8  ZF_LEN;//转发长度为0标志
EXT_ u8  Ver_flag;//红外透传、协议标志
EXT_ u8  IR_to_BL;//IR回传标志
EXT_ u8  POW_IR;//IR电源标志

EXT_ __IO uint32_t TimingDelay ;

EXT_ vu16 Usart3_Wtime;	//串口3蓝牙接收数据间隔时长
EXT_ vu8 	Usart3_EN;		//串口3蓝牙接收数据有效

EXT_ vu16 IR_Wtime;

EXT_ vu8 Buf_Flag;
EXT_ vu8 IrTimeBegin;

/** @defgroup config_Exported_Functions
  * @{
  */
/*******队列操作函数区*************/
void InitQue(Quque *q);
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
/*******UART功能函数区*************/
void USART1send(u8 *buffer,u16 len);
void USART2send(u8 *buffer,u16 len);
void USART3send(u8 *buffer,u16 len);
//void BLSETsend(u8 *buffer);
/*******系统处理解析相关*************/
u8 CheckHE(void);
u8 CheckSum(void);
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
void Req_BL(void);

void BL_Unpack(u8 *Buffer, u16 Length);
void ESAM_Info(void);

/*******模拟串口功能函数区*************/
void IR_GPIO_Init(void);

void SendOneByte(u8 Byte);
//void SendStr(u8 *str);
void SendBytes(u8 *str,u8 len);

void EXTI9_5_DISABLE(void);
void EXTI9_5_ENABLE(void);

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

void myADC_init(void);
u16 ADC_filter(void);
#endif

