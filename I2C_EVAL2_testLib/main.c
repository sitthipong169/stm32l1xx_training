/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    16-May-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
// #include "SHT20.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private setup functions ---------------------------------------------------------*/
void RCC_setup(void);
void RCC_setup2(void);
void GPIO_setup(void);
static void USART_Config(void);
void usart1_putc(char c);
void usart1_puts(char *s);
void i2c1_DeInit(void);
void i2c1_Init(void);


/* Private user define functions ---------------------------------------------------------*/
void delay(unsigned long ms);
bool OEM_READ_PH(I2C_TypeDef* i2c, float* raw);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

char buffer[80] = {'\0'};

int main(void)
{
  // RCC_setup();
  RCC_setup2();
  // GPIO_setup();
  USART_Config();
  i2c1_Init();
  // float rawTemperature = 0;
  // float rawHumidity = 0;
  float rawPHsensor = 0;
  // double tempTemperature = 0.00;
  // double tempHumidity = 0.00;
  double tempPH = 0.00;
  while (1)
  {
    usart1_puts("start\r\n");
    
    /* LED at PB9 ON */
		// GPIO_SetBits(GPIOB,GPIO_Pin_7);
		/* Delay 0.5 sec */
		// delay(500);
		/* LED at PB9 OFF */
		// GPIO_ResetBits(GPIOB,GPIO_Pin_7);
		/* Delay 0.5 sec */
		// delay(500);
    usart1_puts("\rUSART1 ...\r\n");

    // if(SHT20ReadTemperature(I2C1, &rawTemperature))
    // {
    //   tempTemperature = rawTemperature; // * (175.72 / 65536.0) -46.85;
    //   sprintf(buffer, "Temp: %f\r\n", tempTemperature);
    //   usart1_puts(buffer);
    // }

    // if(SHT20ReadHumidity(I2C1, &rawHumidity))
    // {
    //   tempHumidity = rawHumidity; // * (125/ 65536.0) -6.0;
    //   sprintf(buffer, "Humidity: %f\r\n", tempHumidity);
    //   usart1_puts(buffer);
    // }

    if(OEM_READ_PH(I2C1, &rawPHsensor))
    {
      tempPH = rawPHsensor;
      sprintf(buffer, "PH: %f\r\n", tempPH);
      usart1_puts(buffer);
    }
    
  }
}
bool OEM_READ_PH(I2C_TypeDef* i2c, float* raw)
{
  unsigned long result=0;

  //address = 0x65 || 0110 0101
  //address = 0xCA || 1100 1010 
  uint8_t bit1=0;
  uint8_t bit2=0;
  uint8_t bit3=0;
  uint8_t bit4=0;

  // delay(100);
  I2C_AcknowledgeConfig(i2c,ENABLE);
  I2C_GenerateSTART(i2c,ENABLE);
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(i2c,0xCA, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(i2c,0x16);
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_GenerateSTOP(i2c, ENABLE);

  I2C_GenerateSTART(i2c,ENABLE);
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(i2c,0xCA, I2C_Direction_Receiver);
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_RECEIVED));
  bit1 = (uint8_t)I2C_ReceiveData(i2c);
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_RECEIVED));
  bit2 = (uint8_t)I2C_ReceiveData(i2c);
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_RECEIVED));
  bit3 = (uint8_t)I2C_ReceiveData(i2c);
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_RECEIVED));
  bit4 = (uint8_t)I2C_ReceiveData(i2c);
  I2C_GenerateSTOP(i2c, ENABLE);

  I2C_AcknowledgeConfig(i2c, DISABLE);
  I2C_GenerateSTOP(i2c, ENABLE);
  
  
  while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_RECEIVED));
  I2C_ReceiveData(i2c);

  // result = (msb << 8) | lsb;
  result = (bit1 << 24) | (bit2 << 16) | (bit3 << 8) | bit4;
  *raw = ((float)result)/1000;
  return true;
}
/*---------------------Function----------------------------*/
void RCC_setup(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable Internal Clock HSI */
  RCC_HSICmd(ENABLE);
  /* Wait till HSI is Ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==RESET);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div2);
  FLASH_SetLatency(FLASH_Latency_0);
  /* Enable PrefetchBuffer */
  FLASH_PrefetchBufferCmd(ENABLE);
  /* Set HSI Clock Source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  /* Wait Clock source stable */
  while(RCC_GetSYSCLKSource()!=0x04);
}

void RCC_setup2(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable Internal Clock HSI */
  RCC_HSICmd(ENABLE);
  /* Wait till HSI is Ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==RESET);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div2);
  FLASH_SetLatency(FLASH_Latency_0);
  /* Enable PrefetchBuffer */
  FLASH_PrefetchBufferCmd(ENABLE);
  /* Set HSI Clock Source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  /* Wait Clock source stable */
  while(RCC_GetSYSCLKSource()!=0x04);
}

void GPIO_setup(void)
{
	/* GPIO Sturcture */
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable Peripheral Clock AHB for GPIOB */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	/* Configure PC13 as Output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
// delay 1 ms per count @ Crystal 16.0 MHz 
void delay(unsigned long ms)
{
	volatile unsigned long i,j;
	for (i = 0; i < ms; i++ )
	for (j = 0; j < 1227; j++ );
}

static void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);
  
  /* Enable USART clock */
  USARTx_APBPERIPHCLOCK(USARTx_CLK, ENABLE);
  
  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
  
  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
  GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
  GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  /* USARTx configuration ----------------------------------------------------*/
  /* USARTx configured as follow:
  - BaudRate = 230400 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USARTx, ENABLE);
}


void i2c1_Init(void)
{
  I2C_DeInit(I2C1);
  I2C_InitTypeDef   I2C_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  // i2c1_DeInit();
  /*!< LM75_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
  /*!< LM75_I2C_SCL_GPIO_CLK, LM75_I2C_SDA_GPIO_CLK 
       and LM75_I2C_SMBUSALERT_GPIO_CLK Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);

  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);

  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); 

  /*!< Configure I2C1 pins: SCL and SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  I2C_Init(I2C1, &I2C_InitStructure);

  I2C_Cmd(I2C1, ENABLE);
}

//Function Usart1 send 1 character
void usart1_putc(char c)
{
  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET); //wait until transmittion readdy
  USART_SendData(USART2,(int)c);
}
//Function Usart1 send string
void usart1_puts(char *s)
{
  while(*s) // Check end of string
  {
    usart1_putc(*s++); //send character 1 time
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
// not sure 1ms/(1/Clock)*0.7675 - 1