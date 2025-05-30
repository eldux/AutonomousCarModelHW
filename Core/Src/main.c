/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "print.h"
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
 TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

osThreadId defaultTaskHandle;
osThreadId printingTaskHandle;
osSemaphoreId usart2TXAvailableHandle;
osStaticSemaphoreDef_t usart2AvailableControlBlock;
/* USER CODE BEGIN PV */
uint16_t rc_steering = 1500;
uint16_t rc_acceleration = 1500;

uint16_t distance_ch1_us = 0;
uint16_t distance_ch2_us = 0;
uint16_t distance_ch3_us = 0;
uint16_t distance_ch1_right_mm = 0;
uint16_t distance_ch2_left_mm = 0;
uint16_t distance_ch3_mm = 0;
uint8_t autonomous = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
void StartDefaultTask(void const * argument);
void printTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM1_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of usart2TXAvailable */
  osSemaphoreStaticDef(usart2TXAvailable, &usart2AvailableControlBlock);
  usart2TXAvailableHandle = osSemaphoreCreate(osSemaphore(usart2TXAvailable), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  osSemaphoreRelease(usart2TXAvailableHandle);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of printingTask */
  osThreadDef(printingTask, printTask, osPriorityBelowNormal, 0, 512);
  printingTaskHandle = osThreadCreate(osThread(printingTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 64;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10500;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 64;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 10;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t servo_in_ch1_first = 0;
	static uint16_t servo_in_ch1_first_captured = 0;
	static uint16_t servo_in_ch1_second = 0;
	static uint16_t servo_in_ch2_first = 0;
	static uint16_t servo_in_ch2_first_captured = 0;
	static uint16_t servo_in_ch2_second = 0;
	static uint16_t echo_in_ch1_first = 0;
	static uint16_t echo_in_ch1_first_captured = 0;
	static uint16_t echo_in_ch1_second = 0;
	static uint16_t echo_in_ch2_first = 0;
	static uint16_t echo_in_ch2_first_captured = 0;
	static uint16_t echo_in_ch2_second = 0;
	static uint16_t echo_in_ch3_first = 0;
	static uint16_t echo_in_ch3_first_captured = 0;
	static uint16_t echo_in_ch3_second = 0;

	if (htim->Instance == TIM1) {
	    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	    {
	    	if (!servo_in_ch1_first_captured)
			{
				servo_in_ch1_first = htim1.Instance->CCR3;
				if (servo_in_ch1_first < 5000)
				{
					htim1.Instance->CCR3 = 0;
				}
				else
				{
					servo_in_ch1_first_captured = 1;
				}
			}
			else
			{
				servo_in_ch1_second = htim1.Instance->CCR3;
				if (servo_in_ch1_second < 2500)
				{
					servo_in_ch1_second += 10500;
				}
				if (servo_in_ch1_second > servo_in_ch1_first)
				{
					rc_acceleration = servo_in_ch1_second - servo_in_ch1_first;
					if (rc_acceleration < 900) rc_acceleration = 1500;
					else if (rc_acceleration < 1000) rc_acceleration = 1000;
					else if (rc_acceleration > 2100) rc_acceleration = 1500;
					else if (rc_acceleration > 2000) rc_acceleration = 2000;
					//htim1.Instance->CCR2 = rc_acceleration;
					if ((rc_acceleration > 1800) || (rc_acceleration < 1200))
					{
						autonomous = 0;
					}
					htim1.Instance->CCR3 = 0;
					servo_in_ch1_first_captured = 0;
				}
				else
				{
					servo_in_ch1_first = servo_in_ch1_second;
				}
			}
	    }
	    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			if (!servo_in_ch2_first_captured)
			{
				servo_in_ch2_first = htim1.Instance->CCR4;
				if (servo_in_ch2_first < 5000)
				{
					htim1.Instance->CCR4 = 0;
				}
				else
				{
					servo_in_ch2_first_captured = 1;
				}
			}
			else
			{
				servo_in_ch2_second = htim1.Instance->CCR4;
				if (servo_in_ch2_second < 2500)
				{
					servo_in_ch2_second += 10500;
				}
				if (servo_in_ch2_second > servo_in_ch2_first)
				{
					rc_steering = servo_in_ch2_second - servo_in_ch2_first;
					if (rc_steering < 900) rc_steering = 1500;
					else if (rc_steering < 1000) rc_steering = 1000;
					else if (rc_steering > 2000) rc_steering = 2000;
					else if (rc_steering > 2100) rc_steering = 1500;
					//htim1.Instance->CCR1 = rc_steering;
					if ((rc_steering > 1800) || (rc_steering < 1200))
					{
						autonomous = 1;
					}
					htim1.Instance->CCR4 = 0;
					servo_in_ch2_first_captured = 0;
				}
				else
				{
					servo_in_ch2_first = servo_in_ch2_second;
				}
			}
		}
	}
	// Read ultrasonic range sensors
	if (htim->Instance == TIM3) {
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if (!echo_in_ch1_first_captured)
			{
				echo_in_ch1_first = htim3.Instance->CCR2;
				echo_in_ch1_first_captured = 1;
			}
			else
			{
				echo_in_ch1_second = htim3.Instance->CCR2;
				if (echo_in_ch1_second > echo_in_ch1_first)
				{
					distance_ch1_us = echo_in_ch1_second - echo_in_ch1_first;
					htim3.Instance->CCR2 = 0;
					echo_in_ch1_first_captured = 0;
					distance_ch1_right_mm = (uint16_t)(((uint32_t)distance_ch1_us * 10)/58);
				}
				else
				{
					echo_in_ch1_first = echo_in_ch1_second;
				}
			}
		}
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			if (!echo_in_ch2_first_captured)
			{
				echo_in_ch2_first = htim3.Instance->CCR3;
				echo_in_ch2_first_captured = 1;
			}
			else
			{
				echo_in_ch2_second = htim3.Instance->CCR3;
				if (echo_in_ch2_second > echo_in_ch2_first)
				{
					distance_ch2_us = echo_in_ch2_second - echo_in_ch2_first;
					htim3.Instance->CCR3 = 0;
					echo_in_ch2_first_captured = 0;
					distance_ch2_left_mm = (uint16_t)(((uint32_t)distance_ch2_us * 10)/58);
				}
				else
				{
					echo_in_ch2_first = echo_in_ch2_second;
				}
			}
		}
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			if (!echo_in_ch3_first_captured)
			{
				echo_in_ch3_first = htim3.Instance->CCR4;
				echo_in_ch3_first_captured = 1;
			}
			else
			{
				echo_in_ch3_second = htim3.Instance->CCR4;
				if (echo_in_ch3_second > echo_in_ch3_first)
				{
					distance_ch3_us = echo_in_ch3_second - echo_in_ch3_first;
					htim3.Instance->CCR4 = 0;
					echo_in_ch3_first_captured = 0;
					distance_ch3_mm = (uint16_t)((uint32_t)distance_ch3_us * 10)/58;
				}
				else
				{
					echo_in_ch3_first = echo_in_ch3_second;
				}
			}
		}
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
	uint16_t steering = 1560;
	uint16_t acceleration = 1500;
	int16_t steering_direction = 10;
	int16_t acceleration_direction = 1;

	osDelay(100);

	/* Start Servo control for accelerator and steering */
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

	/* Start RC reading */
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4);

	/* Start Ultrasonic measurements */
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // Trigger
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2); // Echo 1
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3); // Echo 2
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_4); // Echo 3
  /* Infinite loop */
  for(;;)
  {
      osDelay(100);
      print("Distance right: %d mm, left: %d mm\n", distance_ch1_right_mm, distance_ch2_left_mm);
      if (autonomous)
      {
		  if ((distance_ch1_right_mm < 200) && (distance_ch2_left_mm > 200))
		  {
			  htim1.Instance->CCR1 = 1000;
			  htim1.Instance->CCR2 = 1570;
		  }
		  else if ((distance_ch1_right_mm > 200) && (distance_ch2_left_mm < 200))
		  {
			  htim1.Instance->CCR1 = 2000;
			  htim1.Instance->CCR2 = 1570;
		  }
		  else if ((distance_ch1_right_mm < 200) && (distance_ch2_left_mm < 200))
		  {
			  htim1.Instance->CCR1 = 1000;
			  htim1.Instance->CCR2 = 1500;
			  osDelay(100);
			  htim1.Instance->CCR2 = 1400;
			  osDelay(800);
		  }
		  else
		  {
			  htim1.Instance->CCR1 = 1550;
			  htim1.Instance->CCR2 = 1570;
		  }
      }
      else
      {
    	  htim1.Instance->CCR1 = 1550;
    	  htim1.Instance->CCR2 = 1500;
      }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_printTask */
/**
* @brief Function implementing the printingTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_printTask */
__weak void printTask(void const * argument)
{
  /* USER CODE BEGIN printTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END printTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
