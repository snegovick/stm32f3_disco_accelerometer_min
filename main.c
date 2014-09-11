/**
  ******************************************************************************
  * @file    GPIO_IOToggle/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "stm32f30x_gpio.h"
#include "stm32f30x_usart.h"
#include "stm32f30x_rcc.h"

#include "uart.h"
#include "util.h"
#include "stm32f3_discovery_lsm303dlhc.h"

/** @addtogroup STM32F3_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BSRR_VAL 0xC000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint32_t LSM303DLHC_TIMEOUT_UserCallback(void) {
  lprintf("LSM<->STM I2C Communication timeout\r\n");
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32f30x.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f30x.c file
  */

  /* GPIOE Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

  GPIO_InitTypeDef        GPIO_InitStructure;

  
  /* Configure PE14 and PE15 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  RCC_ClocksTypeDef rcc_clk;
  RCC_GetClocksFreq(&rcc_clk);
  uint32_t freq = rcc_clk.SYSCLK_Frequency;

  uart_init();

  LSM303DLHCAcc_InitTypeDef accel_init;
  accel_init.Power_Mode = LSM303DLHC_NORMAL_MODE;
  accel_init.AccOutput_DataRate = LSM303DLHC_ODR_1_HZ;
  accel_init.Axes_Enable = LSM303DLHC_AXES_ENABLE;
  accel_init.High_Resolution = LSM303DLHC_HR_DISABLE;
  accel_init.BlockData_Update = LSM303DLHC_BlockUpdate_Single;
  accel_init.Endianness = LSM303DLHC_BLE_LSB;
  accel_init.AccFull_Scale = LSM303DLHC_FULLSCALE_4G;

  LSM303DLHC_AccInit(&accel_init);
  
  /* To achieve GPIO toggling maximum frequency, the following  sequence is mandatory. 
  You can monitor PE14 and PE15 on the scope to measure the output signal. 
  If you need to fine tune this frequency, you can add more GPIO set/reset 
  cycles to minimize more the infinite loop timing.
  This code needs to be compiled with high speed optimization option.  */
  struct __attribute__((packed)) {
    int16_t x;
    int16_t y;
    int16_t z;
  } meas;

  while (1)
  {
    LSM303DLHC_Read(ACC_I2C_ADDRESS, LSM303DLHC_OUT_X_L_A, &meas, 6);
    int32_t x, y, z;
    int x_h, x_l, y_h, y_l, z_h, z_l;
    /* int16max => 4g
       a = i16m/32767/4 => (4, -4)
       b = a*98 => (392, -392)
     */
    x = meas.x;
    x = ((x*98)/(32767/4));
    x_h = x/10;
    x_l = uabs(x-x_h*10);

    y = meas.y;
    y = ((y*98)/(32767/4));
    y_h = y/10;
    y_l = uabs(y-y_h*10);

    z = meas.z;
    z = ((z*98)/(32767/4));
    z_h = z/10;
    z_l = uabs(z-z_h*10);

    lprintf("freq: %i, x: %i.%i, y: %i.%i, z: %i.%i\r\n", (int)freq, x_h, x_l, y_h, y_l, z_h, z_l);
    /* Set PE14 and PE15 */
    GPIOE->BSRR = BSRR_VAL;
    /* Reset PE14 and PE15 */
    volatile uint32_t i = 0;
    for (i = 0; i < freq/100; i++);
    GPIOE->BRR = BSRR_VAL;
    for (i = 0; i < freq/100; i++);
    
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
