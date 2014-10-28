/* Includes ------------------------------------------------------------------*/
#define root
#include "myconfig.h"
#include "ESAM.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
extern __IO uint16_t ADC_ConvertedValue;


int main(void)
{			
	
	/* System Clocks Configuration */
	RCC_Configuration();
	
#ifdef  IWDGSW
	Check_Rst();
#endif
	
	/* Configure the GPIO ports */
	GPIO_Configuration();
	/*USART configuration*/
	USART_Configuration();
	/*7816 configuration*/
//	ISO7816_Enable();
	/*TIM configuration*/
	Sys_run.Out_run_Time_IRDA = 0;
	Sys_run.Out_run_Time_RS485 = 0;
	Sys_run.Sleep_run_Time = 0;
	Sys_run.Sleep_run_Time_BatLow = 0;
	Sys_run.Check_Bat_Time = 0;
	Sys_run.BTuart_Time_Enable = 0;
	Sys_run.BTuart_Out_Run_Time = 0;
	TIM_Configuration();
	TIM2_Configuration_56K();
	/* NVIC configuration */
	NVIC_Configuration(); 		
  /* Configure the SysTick to generate an interrupt each 10 Microseconds */
  SysTick_Configuration();
	/*ADC configuration*/
//	myADC_init();	
	
	BTSet();

/*****************************************************/	
	
 	PowerOff();
	
	InitQue(&RS485RxQUE);//485接收队列
	InitQue(&IrDARxQUE);//红外接收队列

	Clear_RxBuffer2();
	Clear_RxBuffer3();
	Sys_config.Sys_ready = SYSNOTREADY;
	
	
	
	PowerUp();//BL等待配对	
	
	delay_nms(100);
	BT_STA = LEDSTA_BTLINKOFF;
	IR_BaudRate_Time = 83;	
	Wait_BTlink();
	Sys_config.SleepTime = MAXTIMEBTLINKEDNODATA;
	
  while (1)
  {
#ifdef  IWDGSW
		IWDG_ReloadCounter();		
#endif
		
		BT_Analysis();
		Time_Comp();
		if(LED2_0)//断开服务
		{
			NVIC_SystemReset();
		}
	}
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
