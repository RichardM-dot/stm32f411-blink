/**
  ******************************************************************************
  * @file    main.h
  * @brief   Header for main application (STM32F411xE, bare-metal + HAL).
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* On-board LED:
   - Nucleo-F411RE: LD2 (green) on PA5, active high
   - WeAct "BlackPill" F411: LED on PC13, active low
   Adjust LED_PORT/LED_PIN/LED_ON below if you use a different board. */
#define LED_PIN                GPIO_PIN_5
#define LED_GPIO_PORT          GPIOA
#define LED_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED_ON_STATE           GPIO_PIN_SET     /*!< GPIO_PIN_SET for active-high LED */

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
