/**
  ******************************************************************************
  * @file    stm32f4xx_hal_conf.h
  * @brief   HAL configuration file for the stm32f411-blink project.
  *          Derived from Inc/stm32f4xx_hal_conf_template.h of the
  *          STM32F4xx HAL driver, trimmed to the modules actually used.
  ******************************************************************************
  */

#ifndef __STM32F4xx_HAL_CONF_H
#define __STM32F4xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ########################################################################## */
/* ### Module Selection ##################################################### */
/* ########################################################################## */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED

/* ########################################################################## */
/* ### Oscillator Values adaptation ######################################### */
/* ########################################################################## */
/**
  * @brief  Adjust the value of External High Speed oscillator (HSE) used
  *         in your application. This value is used by the RCC HAL module
  *         to compute the system frequency (when HSE is used as system
  *         clock source, directly or through the PLL).
  */
#if !defined  (HSE_VALUE)
#define HSE_VALUE              ((uint32_t)25000000U) /*!< Default HSE = 25 MHz (WeAct/STM32F411 Discovery class boards) */
#endif /* HSE_VALUE */

#if !defined  (HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT    ((uint32_t)100U)      /*!< Time out for HSE start up, in ms */
#endif /* HSE_STARTUP_TIMEOUT */

/**
  * @brief  Internal High Speed oscillator (HSI) value.
  *         This value is used by the RCC HAL module to compute the system
  *         frequency (when HSI is used as system clock source, directly
  *         or through the PLL).
  */
#if !defined  (HSI_VALUE)
#define HSI_VALUE              ((uint32_t)16000000U) /*!< Value of the internal oscillator in Hz */
#endif /* HSI_VALUE */

/**
  * @brief  Internal Low Speed oscillator (LSI) value.
  */
#if !defined  (LSI_VALUE)
#define LSI_VALUE              ((uint32_t)32000U)
#endif /* LSI_VALUE */

/**
  * @brief  External Low Speed oscillator (LSE) value.
  */
#if !defined  (LSE_VALUE)
#define LSE_VALUE              ((uint32_t)32768U)
#endif /* LSE_VALUE */

#if !defined  (LSE_STARTUP_TIMEOUT)
#define LSE_STARTUP_TIMEOUT    ((uint32_t)5000U)
#endif /* LSE_STARTUP_TIMEOUT */

/**
  * @brief  External clock source for I2S/SAI peripheral (used by
  *         HAL_RCCEx_GetPeriphCLKFreq, required by the HAL RCC driver).
  */
#if !defined  (EXTERNAL_CLOCK_VALUE)
#define EXTERNAL_CLOCK_VALUE   ((uint32_t)12288000U) /*!< Value of the External oscillator in Hz */
#endif /* EXTERNAL_CLOCK_VALUE */

/* ########################################################################## */
/* ### Global features ###################################################### */
/* ########################################################################## */
/*!< Tick interrupt priority: lowest, so the 1 ms HAL tick never preempts
     user ISRs running at any priority. */
#define  TICK_INT_PRIORITY            ((uint16_t)0x0FU)

/*!< Enable use of the Prefetch buffer. */
#define  USE_PREFETCH                 1U

/*!< Enable instruction cache (ART accelerator). */
#define  INSTRUCTION_CACHE_ENABLE     1U

/*!< Enable data cache. */
#define  DATA_CACHE_ENABLE            1U

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. (Not used here: code runs from internal Flash.) */
/* #define VECT_TAB_SRAM */

/*!< This variable is updated in three ways:
      1) SystemCoreClockUpdate(): read the RCC registers and update it
      2) HAL_RCC_GetHCLKFreq(): called by HAL_Delay()/HAL_GetTick()
      3) by user code, via SystemCoreClockUpdate() */
/* #define USE_FULL_ASSERT  1U */

/* ########################################################################## */
/* ### Includes ############################################################# */
/* ########################################################################## */
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32f4xx_hal_rcc.h"
#endif /* HAL_RCC_MODULE_ENABLED */

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32f4xx_hal_gpio.h"
#endif /* HAL_GPIO_MODULE_ENABLED */

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32f4xx_hal_cortex.h"
#endif /* HAL_CORTEX_MODULE_ENABLED */

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32f4xx_hal_flash.h"
#endif /* HAL_FLASH_MODULE_ENABLED */

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32f4xx_hal_pwr.h"
#endif /* HAL_PWR_MODULE_ENABLED */

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f4xx_hal_dma.h"
#endif /* HAL_DMA_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32f4xx_hal_tim.h"
#endif /* HAL_TIM_MODULE_ENABLED */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_CONF_H */
