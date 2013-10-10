/**
  ******************************************************************************
  * @file    GPIO/IOToggle/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#define root
#include "myconfig.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define PWR_SLEEPEntry_WFI 0
#define	PWR_SLEEPEntry_WFE 1
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

/*******************************************************************************
* Function Name  : PWR_EnterSLEEPMode
* Description    : Enters SLEEP mode.
* Input          : - SysCtrl_Set: Select the Sleep mode entry mechanism,.
*                    This parameter can be one of the following values:
*                       - 0: MCU enters Sleep mode as soon as WFI or WFE instruction is executed.
*                       - 1: MCU enters Sleep mode as soon as it exits the lowest priority ISR.
*
*                  - PWR_STOPEntry: specifies if SLEEP mode in entered with WFI or WFE instruction.
*                     This parameter can be one of the following values:
*                       - PWR_SLEEPEntry_WFI: enter STOP mode with WFI instruction
*                       - PWR_SLEEPEntry_WFE: enter STOP mode with WFE instruction
* Output         : None
* Return         : None
*******************************************************************************/
void PWR_EnterSLEEPMode(/*u32 SysCtrl_Set,*/ u8 PWR_SLEEPEntry)
	{
//     if (SysCtrl_Set)
//         *(vu32 *) SCB_SysCtrl |= SysCtrl_SLEEPONEXIT_Set;    // Set SLEEPONEXIT
//     else
//         *(vu32 *) SCB_SysCtrl &= ~SysCtrl_SLEEPONEXIT_Set;// Reset SLEEPONEXIT

//     *(vu32 *) SCB_SysCtrl &= ~SysCtrl_SLEEPDEEP_Set;    // Clear SLEEPDEEP bit
	if(PWR_SLEEPEntry == PWR_SLEEPEntry_WFI)            // Select SLEEP mode entry
			__WFI();                                        // Request Wait For Interrupt
	else
			__WFE();                                        // Request Wait For Event
	}	
	
	
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
	/* System Clocks Configuration */
	RCC_Configuration();
#ifdef  IWDGSW
	/*IWDG configuration*/
// 	IWDG_Configuration();
	Check_Rst();
#endif
	Com3first = COMFIR;
	TimingDelay = 0;
	
	InitQue(&RxQUE3);
	/* Configure the GPIO ports */
	GPIO_Configuration();
	/*USART and UART configuration*/
	USART_Configuration();	
	/*TIM configuration*/
	TIM_Configuration();
	/* NVIC configuration */
	NVIC_Configuration(); 	
	time_sleep = 0;
	/* RTC configuration */
	RTC_Configuration();	
  /* Configure the SysTick to generate an interrupt each 1 millisecond */
  SysTick_Configuration();
	/*ADC configuration*/
	ADC_Configuration();
	PowerUp();
	sysread = 0;
	HB_Time = HBTIME;
	W_Mode = IRDAMODE;
	W_Time = SLEEPTIME;
	if(PowerOn == RST_PWRON)
	{
		Req_Irda();	
	}
	PWR_IR_OFF();
	Req_BL();
	if(sysread != 1)
	{
		NVIC_SystemReset();
	}
	InitQue(&RxQUE3);
	time_send = 0;
  while (1)
  {
		Delay(100);
		if(HB_Time <= time_send)
		{
			PWR_IR_OFF();
			time_send = 0;
			Send_HB();
		}
		if(time_sleep>=(W_Time*60))
		{
			PowerDown();
		}
		if(LED2_0)
		{
			NVIC_SystemReset();
		}
//		PWR_EnterSLEEPMode(PWR_SLEEPEntry_WFI);		
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
