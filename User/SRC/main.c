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
	ISO7816_Enable();
	/*TIM configuration*/
	Sys_run.Out_run_Time_IRDA = 0;
	Sys_run.Out_run_Time_RS485 = 0;
	Sys_run.Sleep_run_Time = 0;
	TIM_Configuration();
	/* NVIC configuration */
	NVIC_Configuration(); 		
  /* Configure the SysTick to generate an interrupt each 10 Microseconds */
  SysTick_Configuration();
	/*ADC configuration*/
	myADC_init();	
	
	BTSet();

/*****************************************************/	
	
 	PowerOff();
	
	InitQue(&RS485RxQUE);//485接收队列
	InitQue(&IrDARxQUE);//红外接收队列

	Clear_RxBuffer2();
	Clear_RxBuffer3();
//	ESAM_Info();
//	W_Bat = ADC_filter();
	
	
	
	PowerUp();//BL等待配对	
	
	delay_nms(100);
	BT_STA = 0;
	IR_BaudRate_Time = 83;
//	Set_IRDA_power_ON();
	Wait_BTlink();
	Sys_config.SleepTime = MAXTIMEBTLINKEDNODATA;
	
  while (1)
  {
//		IWDG_ReloadCounter();		
		
		BT_Analysis();
//		Time_Comp();
		if(LED2_0)//断开服务
		{
//			NVIC_SystemReset();
		}
	}
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
