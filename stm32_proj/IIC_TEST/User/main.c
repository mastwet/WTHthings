 
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./delay/core_delay.h" 
#include "./mpu/bsp_mpu.h" 
#include "./usart/bsp_debug_usart.h"
#include "./i2c/bsP_i2c_ee.h"

#define  DATA_Size			256
#define  EEP_Firstpage      0x00
uint8_t I2c_Buf_Write[DATA_Size];
uint8_t I2c_Buf_Read[DATA_Size];
uint8_t IIC_test_2(void);
uint8_t I2C_Test(void);


int main(void)
{ 
	/* ϵͳʱ�ӳ�ʼ����480MHz */
	SystemClock_Config();

   /* Ĭ�ϲ����� MPU������Ҫ�������ܣ������� MPU ��ʹ�� 
   DMA ʱ��ע�� Cache �� �ڴ�����һ���Ե����⣬
   ����ע��������ο����׽̵̳� MPU ��������½� */
//  Board_MPU_Config(0, MPU_Normal_WT, 0xD0000000, MPU_32MB);
//  Board_MPU_Config(1, MPU_Normal_WT, 0x24000000, MPU_512KB);
  
  SCB_EnableICache();    // ʹ��ָ�� Cache
//  SCB_EnableDCache();    // ʹ������ Cache
  
	LED_GPIO_Config();
	/* ���ô���1Ϊ��115200 8-N-1 */
	DEBUG_USART_Config();	 
	printf("\r\n IIC Encode & Decode \r\n");

	/* I2C �����(AT24C02)ʼ�� */
	I2C_EE_Init();
	IIC_test_2();

	while (1)
	{
			
	}  
}


uint8_t IIC_test_2(void)
{
	unsigned char a[7],b[4],r[5];
	char ch;
	unsigned int i = 0,flg;
	printf("��ʪ��IIC����д����Գ���");
	printf("\n\r");
	printf("��������Դ��룺��ʽ��A��+5������,�Ի��н���");
	printf("\n\r");
	while(a[6] != '/')
	{
		HAL_UART_Receive(&UartHandle, (uint8_t *)&a, 7, 1000);
	}
	for(int i = 1;i<6;i++)
	{
		b[i-1]=(a[i]<<1);//encode
		printf("\n\r");
		printf("���ܵ�����Ϊ��%c",b[i-1]);
	}
	I2C_EE_BufferWrite( b, EEP_Firstpage, 5);
	printf("\n\r");
	EEPROM_INFO("������...");
	printf("\n\r");
	I2C_EE_BufferRead(r, EEP_Firstpage, 5);
	for(int i = 0;i<5;i++)
	{
		printf("���ܵ�����Ϊ��%c",r[i]>>1);
		printf("\n\r");
	}
	printf("\n\r");
	printf("done!");
}


/**
  * @brief  System Clock ����
  *         system Clock ��������: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 480000000 (CPU Clock)
	*            HCLK(Hz)             = 240000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  120MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  120MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  120MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  120MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 192
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** ���õ�Դ���ø���
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** ����������ѹ�������ѹ
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** ��ʼ��CPU��AHB��APB����ʱ��
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
		while(1);
  }
  /** ��ʼ��CPU��AHB��APB����ʱ��
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
		while(1);
  }
}
/****************************END OF FILE***************************/
