//#include "tim.h"
//#include "gpio.h"
//#include "stm32f1xx_hal.h"
//#include "myit.h"
//#include "main.h"
// int originmynum=20;
// int mynum=20;
// int mycount=0;
// int pwmVal=0;
//int myflag=1;
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{

//	
//}
//	


//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{

//	int mypwmduty_max=10000;
//	if(htim==&htim6)
//	{
//		if(myflag==1)
//		{
//			pwmVal=pwmVal+264;

//			if(pwmVal>=mypwmduty_max)
//			{
//				myflag=0;
//				pwmVal=mypwmduty_max;
//			}
//			
//				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, pwmVal);    //?????,?????
//			
//		}
//		
//		
//		if(myflag==0)
//		{
//			pwmVal=pwmVal-264;
//		
//			if(pwmVal<=0)
//			{
//				myflag=1;
//				pwmVal=0;
//			}
//			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, pwmVal);    //?????,?????
//			
//		}
//		
//		
//		//HAL_GPIO_TogglePin(led1_GPIO_Port,led1_Pin);
//		
//	}	
//		
//}