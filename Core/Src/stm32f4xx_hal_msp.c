/**
  ******************************************************************************
  * @file    stm32f4xx_hal_msp.c
  * @brief   HAL MSP (MCU Support Package) low-level init/cleanup hooks.
  *          Overrides the __weak definitions in stm32f4xx_hal.c.
  ******************************************************************************
  */

#include "main.h"

/**
  * @brief  Initialize the Global MSP (performed in HAL_Init()).
  * @retval None
  */
void HAL_MspInit(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
}

/**
  * @brief  DeInitialize the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
  __HAL_RCC_SYSCFG_CLK_DISABLE();
  __HAL_RCC_PWR_CLK_DISABLE();
}
