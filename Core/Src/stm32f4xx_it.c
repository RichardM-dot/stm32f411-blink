/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt service routines (Cortex-M4 core exceptions).
  *          Overrides the weak Default_Handler stubs of
  *          startup_stm32f411xe.s via the vector table.
  ******************************************************************************
  */

#include "main.h"
#include "stm32f4xx_it.h"

/**
  * @brief  This function handles NMI exception.
  */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief  This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief  This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief  This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief  This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug monitor exception.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles System tick timer: 1 ms HAL time base.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}
