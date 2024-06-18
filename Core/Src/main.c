/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "ssd1306.h"
#include "fonts.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


//设置TME3的CH2通道的周期时间
//参数newfrep为毫秒单位，整数，例如1代表1ms
uint8_t test[5]={1,2,3,4,5};
void Time3PWM(int newfrep)   
{

}

 int originmynum=20;
 int mynum=20;
 int mycount=0;
 int pwmVal=0;
int myflag=1;
uint8_t myrebuf3[800]; //串口3接收缓冲区
uint8_t myrebuf1[800]; //串口1接收缓冲区
int mywritept3=0; //串口3写指针
int myreadpt3=0;  //串口3读指针
int myreadpt32=0;  //串口3读指针

int mywritept1=0; //串口1写指针
int myreadpt1=0;  //串口1读指针
uint16_t my_adbuff1[10][3];  //50行，3列。每个adc采集周期，填充一行数据，按先后顺序填充0-1-2
int BreaVelo=264;
float temp=0;
float tmpdata=0;
const int N=1e7;
void wait()
{
	for (int i=0;i<7*N;i++)
	{;
	}
}

void BeepPlay(char cmd)   
{
	  int safevolume=0;
		int alertvolume=32767;
		if(cmd=='A')
		{
			 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,alertvolume); //控制占空比，调整音量
    //在不使用缓冲的情况下，必须把计数值清零
			__HAL_TIM_SET_COUNTER(&htim4,0);
		}
		else if (cmd=='S')
		{
				__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,safevolume); //控制占空比，调整音量
			//在不使用缓冲的情况下，必须把计数值清零
			__HAL_TIM_SET_COUNTER(&htim4,0);
		}
  
}
void read()
{
		while(myreadpt1<mywritept1)
		{
			HAL_UART_Transmit(&huart3,&myrebuf1[myreadpt1++],1,1000);
			
		}
		if(myreadpt1>mywritept1)
			myreadpt1=0;

}

void send_to_tcpserver()
{
	
	//const char* Chat=chat;
	//strlen(Chat);
	char Cmd[100];
	//char end[20]="\r\n";
	sprintf(Cmd,"AT+CIPSEND\r\n");//必须是AT+CIPSEND 不能是AT+CIPSEND=《lenth》 和手册上写的不一样
	HAL_UART_Transmit(&huart3,(uint8_t*)Cmd,sizeof Cmd/sizeof Cmd[0],50000);
	printf("\nthis is cmd :%s \n",Cmd);
	wait();
	read();
	//HAL_UART_Transmit(&huart3,(uint8_t*)Chat,sizeof Chat/sizeof Chat[0],50000);
	//int test[]={66};
	//HAL_UART_Transmit(&huart3,(uint8_t*)test,sizeof test/sizeof test[0],2000);
	//printf("\nthis is chat :%s \n",Chat);
	//wait();
	//wait();
	//read();
	//char exit[]="+++\r\n";
	//HAL_UART_Transmit(&huart3,(uint8_t*)exit,sizeof exit/sizeof exit[0],50000);
}

void send_message()
{
	char sc[]="Heloo";
	char TempMsg[50]="";
	sprintf(TempMsg,"RTem:%.2f",tmpdata);
	printf("%s",TempMsg);
	printf("len:%d",strlen(TempMsg));
	HAL_UART_Transmit(&huart3,(uint8_t*)TempMsg,strlen(TempMsg),1000);
}
void connect()
{
	HAL_Delay(5000);
	//	uint8_t str[]={0x41, 0x54,0x0D,0xA};
	//	HAL_UART_Transmit(&huart3,(uint8_t*)str,sizeof str/sizeof str[0],1000);
	//	wait();
	//	read();
//	char str[]="AT\r\n";
//	HAL_UART_Transmit(&huart3,(uint8_t*)str,sizeof str/sizeof str[0],1000);
//	HAL_Delay(1000);
//	read();
	char SetMode[]="AT+CWMODE=1\r\n";
	HAL_UART_Transmit(&huart3,(uint8_t*)SetMode,sizeof SetMode/sizeof SetMode[0],1000);
	wait();
	char CntHotport[]="AT+CWJAP=\"max8\",\"12345678\"\r\n";
	HAL_UART_Transmit(&huart3,(uint8_t*)CntHotport,sizeof CntHotport/sizeof CntHotport[0],1000);
	wait();
	char SetCipmux[]="AT+CIPMUX=0\r\n";
	HAL_UART_Transmit(&huart3,(uint8_t*)SetCipmux,sizeof SetCipmux/sizeof SetCipmux[0],1000);
	wait();
	char TCPPierce[]= "AT+CIPMODE=1\r\n";
	HAL_UART_Transmit(&huart3,(uint8_t*)TCPPierce,sizeof TCPPierce/sizeof TCPPierce[0],1000);
	wait();
	char TCPCnt[]= "AT+CIPSTART=\"TCP\",\"192.168.76.186\",8088\r\n";
	printf("%s",TCPCnt);
	HAL_UART_Transmit(&huart3,(uint8_t*)TCPCnt,sizeof TCPCnt/sizeof TCPCnt[0],5000);
	wait();
	read();

	//char CheckIp[]= "AT+CIFSR\r\n";
	//HAL_UART_Transmit(&huart3,(uint8_t*)CheckIp,sizeof CheckIp/sizeof CheckIp[0],1000);
	//HAL_Delay(1000);
	//read();
	//char message[] ="mom";
	send_to_tcpserver();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_TIM7_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	
	//Time3PWM(1000);

	HAL_UART_Receive_IT(&huart1,&myrebuf1[mywritept1],1);//开启串口接收中断
	HAL_UART_Receive_IT(&huart3,&myrebuf3[mywritept3],1);//开启串口接收中断
	
	
	HAL_ADCEx_Calibration_Start(&hadc1);//开启转换前，先进行校准
	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&my_adbuff1[0][0],3*10);//开启ADC转换转换
	HAL_ADC_Start_IT(&hadc1);//开启ADC1中断
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	//int mydata;
	ssd1306_Init(&hi2c1);

	int mydata1,mydata2,mydata3;
	int flag=0;
	
	connect();
  BeepPlay('S');
	while (1)
  {

		mydata1=mydata2=mydata3=0.0;
		for(int i=0;i<10;i++)
		{
			mydata2=mydata2+my_adbuff1[i][1];
		}
		float x1,x2,x3;
		x2=mydata2/10.0/4095*3.3;
		temp=(1.40-x2)/0.0043+25;
		tmpdata=temp;
		ssd1306_SetCursor(0, 36);
	  char scren[]="";
		sprintf(scren,"Tem:%.2f",tmpdata);
		ssd1306_WriteString(scren, Font_11x18, White);
		ssd1306_UpdateScreen(&hi2c1);
		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_5);
		//printf("this Is tmp2：%.2f",temp);
		if(temp>=30.0||temp<=27.0){
			BeepPlay('A');
			sprintf(scren,"warning!");
			ssd1306_WriteString(scren, Font_11x18, White);
			ssd1306_UpdateScreen(&hi2c1);
		}
		else 
		{
			BeepPlay('S');
		}
		if(temp-31>0)
		{
			temp=8;
		}
		else  if( temp -24<=0){
			temp=1;
		}
		else {
			temp=temp-24;
		}
		if(flag<=0){BreaVelo=(int)(40*temp);}
		flag=flag-1;;
		HAL_Delay(500);

		//HAL_UART_Transmit(&huart1,(uint8_t*)TempMsg,sizeof TempMsg/sizeof TempMsg[0],50000);
		//printf("data2=%d,%.2f,sp%d" ,mydata2/10,temp,BreaVelo);
		
		
		while(myreadpt3<mywritept3)
		{
			HAL_UART_Transmit(&huart1,&myrebuf3[myreadpt3++],1,1000);
			
		}
		if(myreadpt3>mywritept3)
			myreadpt3=0;
		
		//UART1接收的数据转发给UART3
		while(myreadpt1<mywritept1)
		{
			HAL_UART_Transmit(&huart3,&myrebuf1[myreadpt1++],1,1000);
			
		}
		if(myreadpt1>mywritept1)
			myreadpt1=0;
		
		
		//HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_5);
		//HAL_UART_Transmit(&huart1,myrebuf3,100,1000);
		
		//串口3接收到的帧数据解析
		//10H,XX,YY,CRC,16H,其中XX为低字节，yy为高自己，CRC为校验不用了
		
	while(myreadpt32<mywritept3)
		{
			if(myrebuf3[myreadpt32]==0X10 && myrebuf3[myreadpt32+4]==0x16)
			{
				if(myrebuf3[myreadpt32+1]==0X11&&myrebuf3[myreadpt32+2]==0x12)
				{
					HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_5);
					BreaVelo=330;
					flag=3;
				}
				else{
					
				}
			}
			myreadpt32++; //读指针移动
			
		}
		if(myreadpt32>mywritept3) //读指针校正
			myreadpt32=0;
		HAL_Delay(500);
		send_message();
		HAL_Delay(500);
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim6)
	{
		int mypwmduty_max=10000;
		if(myflag==1)
		{
			pwmVal=pwmVal+BreaVelo;
			if(pwmVal>=mypwmduty_max)
			{
				myflag=0;
				pwmVal=mypwmduty_max;
			}
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, pwmVal);    //?????,????
		}
		if(myflag==0)
		{
			pwmVal=pwmVal-BreaVelo;
		
			if(pwmVal<=0)
			{
				myflag=1;
				pwmVal=0;
			}
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, pwmVal);    //?????,?????
		}
		//HAL_GPIO_TogglePin(led1_GPIO_Port,led1_Pin);
	}	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	//串口1接收处理
	if(huart==&huart1)
	{
		
		uint8_t ret = HAL_OK;  
   mywritept1++;    //写指针移动
	if(mywritept1>=780) mywritept1=0;
   
    do  
    {  
			ret = HAL_UART_Receive_IT(&huart1,&myrebuf1[mywritept1],1); //开启接收USART1函数
    }while(ret != HAL_OK);  
		
		
	}
	
	//串口3接收处理
	if(huart==&huart3)
	{
		
		uint8_t ret = HAL_OK;  
   mywritept3++;    //写指针移动
		if(mywritept3>=780) mywritept3=0;
   
    do  
    {  
			ret = HAL_UART_Receive_IT(&huart3,&myrebuf3[mywritept3],1); //开启接收USART3函数
    }while(ret != HAL_OK);  
		
		
	}
	
}


int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc==(&hadc1))
    {
     //获取AD值
        my_adbuff1[0][0]=HAL_ADC_GetValue(&hadc1);

    }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
