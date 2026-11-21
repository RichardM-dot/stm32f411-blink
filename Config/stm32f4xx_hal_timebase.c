/**
  ******************************************************************************
  * @file    stm32f4xx_hal_timebase.c
  * @brief   HAL time base on TIM3 + FreeRTOS glue hooks.
  *
  *          FreeRTOS owns SysTick (see startup_stm32f411xe.s vectors), so the
  *          HAL time base (uwTick / HAL_GetTick / HAL_Delay timeouts) is moved
  *          to TIM3 @ 1 kHz. This overrides the weak HAL_InitTick in
  *          stm32f4xx_hal.c - same pattern as STM32CubeMX "FreeRTOS + TIM
  *          timebase" projects.
  *
  *          This file lives OUTSIDE Core/ on purpose: it is integration glue,
  *          not application code.
  ******************************************************************************
  */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

/* TIM3 @ 1 kHz: timer clock 100 MHz (APB1 = 50 MHz x2), count at 1 MHz. */
#define TIMEBASE_TIM_CLK_FREQ_HZ      1000000UL   /* after prescaler */
#define TIMEBASE_TICK_FREQ_HZ         1000UL      /* HAL tick resolution   */

static TIM_HandleTypeDef s_TimebaseTim = { .Instance = TIM3 };

/* Forward declarations of the hooks implemented below. */
void vAssertCalled( const char * const pcFileName, unsigned long ulLine );
void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName );

/**
  * @brief  Overrides the __weak HAL_InitTick(): starts TIM3 as the 1 ms HAL
  *         time base instead of SysTick (kept for FreeRTOS).
  * @param  TickPriority: TIM3 interrupt priority (0x0F = lowest by default).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick( uint32_t TickPriority )
{
  HAL_StatusTypeDef status = HAL_OK;
  RCC_ClkInitTypeDef clkInitStruct = { 0 };
  uint32_t flashLatency = 0U;
  uint32_t uwTimclock = 0U;
  uint32_t uwPrescaler = 0U;

  __HAL_RCC_TIM3_CLK_ENABLE();

  /* Timer clock = PCLK1 * 2 when APB1 is prescaled, else PCLK1. */
  HAL_RCC_GetClockConfig( &clkInitStruct, &flashLatency );
  uwTimclock = HAL_RCC_GetPCLK1Freq();
  if( clkInitStruct.APB1CLKDivider != RCC_HCLK_DIV1 )
  {
    uwTimclock *= 2U;
  }

  /* Count at 1 MHz, roll over every 1000 counts -> 1 kHz update. */
  uwPrescaler = ( uwTimclock / TIMEBASE_TIM_CLK_FREQ_HZ ) - 1U;

  s_TimebaseTim.Init.Prescaler         = uwPrescaler;
  s_TimebaseTim.Init.Period            = ( TIMEBASE_TIM_CLK_FREQ_HZ / TIMEBASE_TICK_FREQ_HZ ) - 1U;
  s_TimebaseTim.Init.CounterMode       = TIM_COUNTERMODE_UP;
  s_TimebaseTim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  s_TimebaseTim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  status = HAL_TIM_Base_Init( &s_TimebaseTim );
  if( status == HAL_OK )
  {
    status = HAL_TIM_Base_Start_IT( &s_TimebaseTim );
  }
  if( status == HAL_OK )
  {
    HAL_NVIC_SetPriority( TIM3_IRQn, TickPriority, 0U );
    HAL_NVIC_EnableIRQ( TIM3_IRQn );

    uwTickPrio = TickPriority;
    uwTickFreq = 1U;
    uwTick = 0U;
  }

  return status;
}

/**
  * @brief  Overrides the __weak HAL_SuspendTick(): stops the TIM3 update IRQ.
  */
void HAL_SuspendTick( void )
{
  __HAL_TIM_DISABLE_IT( &s_TimebaseTim, TIM_IT_UPDATE );
}

/**
  * @brief  Overrides the __weak HAL_ResumeTick(): re-enables the TIM3 update IRQ.
  */
void HAL_ResumeTick( void )
{
  __HAL_TIM_ENABLE_IT( &s_TimebaseTim, TIM_IT_UPDATE );
}

/**
  * @brief  TIM3 update interrupt: feeds the HAL tick.
  */
void TIM3_IRQHandler( void )
{
  HAL_TIM_IRQHandler( &s_TimebaseTim );
}

/**
  * @brief  Period elapsed callback (shared by all HAL TIM users).
  */
void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef * htim )
{
  if( htim->Instance == TIM3 )
  {
    HAL_IncTick();
  }
}

/* ########################################################################## */
/* ### FreeRTOS hooks required by Config/FreeRTOSConfig.h ################### */
/* ########################################################################## */

/**
  * @brief  configASSERT hook: an assertion failed - park with interrupts off
  *         so a debugger can inspect the call stack.
  */
void vAssertCalled( const char * const pcFileName, unsigned long ulLine )
{
  ( void ) pcFileName;
  ( void ) ulLine;
  taskENTER_CRITICAL();
  for( ; ; )
  {
  }
}

/**
  * @brief  configUSE_MALLOC_FAILED_HOOK: pvPortMalloc() returned NULL.
  */
void vApplicationMallocFailedHook( void )
{
  taskENTER_CRITICAL();
  for( ; ; )
  {
  }
}

/**
  * @brief  configCHECK_FOR_STACK_OVERFLOW == 2 hook: task stack overflowed.
  */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
{
  ( void ) xTask;
  ( void ) pcTaskName;
  taskENTER_CRITICAL();
  for( ; ; )
  {
  }
}
