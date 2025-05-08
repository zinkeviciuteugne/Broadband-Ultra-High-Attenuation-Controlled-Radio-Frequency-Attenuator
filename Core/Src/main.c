#include "main.h"
#include "ssd1331.h"

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile int32_t  encoder_position      = 0;
static   int32_t  last_encoder_position = 0;
volatile uint8_t  flag_update_display   = 0;
volatile uint8_t  flag_update_attn      = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */

static const uint8_t attenuationTable[32] = {
    0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,
    0x10,0x12,0x14,0x16,0x18,0x1A,0x1C,0x1E,
    0x20,0x22,0x24,0x26,0x28,0x2A,0x2C,0x2E,
    0x30,0x32,0x34,0x36,0x38,0x3A,0x3C,0x3E
};
// Read rotary encoder
static void Read_Encoder(void);

static void UpdateAllAttenuators(int32_t pos);
// Low-level write to one PE4312
static void Att_Write(uint8_t word6, uint16_t LE_Pin, GPIO_TypeDef *LE_Port);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void Read_Encoder(void) {
    static uint8_t last_AB = 0;
    uint8_t A = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
    uint8_t B = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);
    uint8_t cur = (A<<1)|B;
    if(cur != last_AB) {
        HAL_Delay(5);
        A = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
        B = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);
        cur = (A<<1)|B;
        if ((last_AB==0&&cur==1)||(last_AB==1&&cur==3)||(last_AB==3&&cur==2)||(last_AB==2&&cur==0))
            encoder_position++;
        else
            encoder_position--;

        if (encoder_position < 0) encoder_position = 0;
        if (encoder_position > 93) encoder_position = 93;

        last_AB = cur;
    }
}


static void Att_Write(uint8_t word6, uint16_t LE_Pin, GPIO_TypeDef *LE_Port) {
    HAL_GPIO_WritePin(LE_Port, LE_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &word6, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LE_Port, LE_Pin, GPIO_PIN_SET);
    __NOP(); __NOP();
    HAL_GPIO_WritePin(LE_Port, LE_Pin, GPIO_PIN_RESET);
}

static void UpdateAllAttenuators(int32_t pos) {
    // Užtikrinam ribas
    if (pos < 0) pos = 0;
    if (pos > 93) pos = 93; // Max 31 dB x 3 atenuatoriai

    // Paskirstom tolygiai po 3 atenuatorius
    int32_t base = pos / 3;
    int32_t rem  = pos % 3;

    int32_t idx0 = base + (rem > 0 ? 1 : 0);
    int32_t idx1 = base + (rem > 1 ? 1 : 0);
    int32_t idx2 = base;

    // Apsaugom nuo viršijimo
    if (idx0 > 31) idx0 = 31;
    if (idx1 > 31) idx1 = 31;
    if (idx2 > 31) idx2 = 31;

    uint8_t w0 = attenuationTable[idx0];
    uint8_t w1 = attenuationTable[idx1];
    uint8_t w2 = attenuationTable[idx2];

    // Debug
    printf("dB=%ld → idx0=%ld, idx1=%ld, idx2=%ld → w0=0x%02X, w1=0x%02X, w2=0x%02X\n",
           (long)pos, idx0, idx1, idx2, w0, w1, w2);

    // ---- Atenuatorius #1 ----
    HAL_GPIO_WritePin(LE_1_GPIO_Port, LE_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LE_2_GPIO_Port, LE_2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LE_3_GPIO_Port, LE_3_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi2, &w0, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LE_1_GPIO_Port, LE_1_Pin, GPIO_PIN_SET);
    __NOP(); __NOP();
    HAL_GPIO_WritePin(LE_1_GPIO_Port, LE_1_Pin, GPIO_PIN_RESET);

    // ---- Atenuatorius #2 ----
    HAL_SPI_Transmit(&hspi2, &w1, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LE_2_GPIO_Port, LE_2_Pin, GPIO_PIN_SET);
    __NOP(); __NOP();
    HAL_GPIO_WritePin(LE_2_GPIO_Port, LE_2_Pin, GPIO_PIN_RESET);

    // ---- Atenuatorius #3 ----
    HAL_SPI_Transmit(&hspi2, &w2, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LE_3_GPIO_Port, LE_3_Pin, GPIO_PIN_SET);
    __NOP(); __NOP();
    HAL_GPIO_WritePin(LE_3_GPIO_Port, LE_3_Pin, GPIO_PIN_RESET);
}

/* USER CODE END 0 */

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
  MX_SPI2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

   SSD1331_Init();
   HAL_TIM_Base_Start_IT(&htim2);
   SSD1331_DrawString(5, 5, "PASIRIKTAS ", 0xF60000);
   SSD1331_DrawString(5, 20, "SLOPINIMAS: ", 0xF60000);


   HAL_GPIO_WritePin(A1_B0_GPIO_Port, A1_B0_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A1_B1_GPIO_Port, A1_B1_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A1_B2_GPIO_Port, A1_B2_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A1_B3_GPIO_Port, A1_B3_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A1_B4_GPIO_Port, A1_B4_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A1_B5_GPIO_Port, A1_B5_Pin, GPIO_PIN_RESET);

   HAL_GPIO_WritePin(A2_B0_GPIO_Port, A2_B0_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A2_B1_GPIO_Port, A2_B1_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A2_B2_GPIO_Port, A2_B2_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A2_B3_GPIO_Port, A2_B3_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A2_B4_GPIO_Port, A2_B4_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A2_B5_GPIO_Port, A2_B5_Pin, GPIO_PIN_RESET);

   HAL_GPIO_WritePin(A3_B0_GPIO_Port, A3_B0_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A3_B1_GPIO_Port, A3_B1_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A3_B2_GPIO_Port, A3_B2_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A3_B3_GPIO_Port, A3_B3_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A3_B4_GPIO_Port, A3_B4_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(A3_B5_GPIO_Port, A3_B5_Pin, GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Read_Encoder();

	          if (encoder_position != last_encoder_position)
	          {

	              if (flag_update_attn) {
	                  UpdateAllAttenuators(encoder_position);
	                  flag_update_attn = 0;
	              }

	              if (flag_update_display) {
	            	  char buf_old[16], buf_new[16];

	            	  sprintf(buf_old, "%ld", last_encoder_position);
	            	   SSD1331_DrawString(40, 40, buf_old, 0x000000);

	            	   sprintf(buf_new, "%ld", encoder_position);
	            	  SSD1331_DrawString(40, 40, buf_new, 0xF60000);
	                  flag_update_display = 0;
	                  last_encoder_position = encoder_position;
	              }
	          }
//	        HAL_Delay(50);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim->Instance==TIM2) {
        static uint8_t tick=0;
        tick++;
        if((tick%2)==0) flag_update_display = 1;
        if((tick%3)==0) flag_update_attn    = 1;
    }
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8399;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 99;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CS_SC_Pin|RST_Pin|LE_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DC_Pin|LE_2_Pin|LE_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CS_SC_Pin RST_Pin LE_3_Pin */
  GPIO_InitStruct.Pin = CS_SC_Pin|RST_Pin|LE_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SIGNAL_Pin PHASE_Pin A3_B1_Pin A3_B2_Pin
                           A3_B3_Pin A2_B1_Pin A2_B2_Pin A2_B3_Pin
                           A2_B4_Pin A2_B5_Pin A2_B0_Pin */
  GPIO_InitStruct.Pin = SIGNAL_Pin|PHASE_Pin|A3_B1_Pin|A3_B2_Pin
                          |A3_B3_Pin|A2_B1_Pin|A2_B2_Pin|A2_B3_Pin
                          |A2_B4_Pin|A2_B5_Pin|A2_B0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : A3_B4_Pin A3_B5_Pin A3_B0_Pin A1_B1_Pin
                           A1_B2_Pin A1_B3_Pin A1_B4_Pin A1_B5_Pin
                           A1_B0_Pin */
  GPIO_InitStruct.Pin = A3_B4_Pin|A3_B5_Pin|A3_B0_Pin|A1_B1_Pin
                          |A1_B2_Pin|A1_B3_Pin|A1_B4_Pin|A1_B5_Pin
                          |A1_B0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DC_Pin LE_2_Pin LE_1_Pin */
  GPIO_InitStruct.Pin = DC_Pin|LE_2_Pin|LE_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


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

