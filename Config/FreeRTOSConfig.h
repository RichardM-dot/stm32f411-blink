/**
  ******************************************************************************
  * @file    FreeRTOSConfig.h
  * @brief   FreeRTOS kernel V11.1.0 configuration for STM32F411xE (Cortex-M4F).
  *
  *          Interrupt handler wiring (no Core/ changes required):
  *            startup_stm32f411xe.s points SVC/PendSV/SysTick vectors directly
  *            at vPortSVCHandler / xPortPendSVHandler / xPortSysTickHandler.
  *            The HAL time base runs on TIM3 (Config/stm32f4xx_hal_timebase.c).
  *
  *          Rules (Cortex-M4F, 4 priority bits):
  *            - ISRs that call FreeRTOS "FromISR" APIs must use a priority
  *              numerically >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY (5).
  *            - Priorities 0..4 must never call FreeRTOS APIs.
  ******************************************************************************
  */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>
extern uint32_t SystemCoreClock;

/* ########################################################################## */
/* ### Scheduler / tick ##################################################### */
/* ########################################################################## */
#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ( SystemCoreClock )
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                    7
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 128 ) /* words */
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configSTACK_DEPTH_TYPE                  uint32_t

/* ########################################################################## */
/* ### Synchronisation primitives ########################################### */
/* ########################################################################## */
#define configUSE_TASK_NOTIFICATIONS            1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIME_SLICING                  1

/* ########################################################################## */
/* ### Software timers ###################################################### */
/* ########################################################################## */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               ( configMAX_PRIORITIES - 1 )
#define configTIMER_TASK_STACK_DEPTH            256
#define configTIMER_QUEUE_LENGTH                10

/* ########################################################################## */
/* ### Memory (heap_4: pvPortMalloc/vPortFree, free-block coalescing) ####### */
/* ########################################################################## */
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 32 * 1024 ) )
#define configAPPLICATION_ALLOCATED_HEAP        0
#define configUSE_NEWLIB_REENTRANT              0

/* ########################################################################## */
/* ### Hooks / diagnostics ################################################## */
/* ########################################################################## */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            1
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0
#define configUSE_TRACE_FACILITY                0
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0
#define configENABLE_BACKWARD_COMPATIBILITY     0

/* ########################################################################## */
/* ### Interrupt priorities (STM32F4: 4 bits, 0 = highest) ################## */
/* ########################################################################## */
#define configPRIO_BITS                                     4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY             15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY        5
#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* ########################################################################## */
/* ### Optional API surface ################################################# */
/* ########################################################################## */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_uxTaskGetStackHighWaterMark2    1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_xTimerPendFunctionCall          0

/* ########################################################################## */
/* ### Assertion ############################################################ */
/* ########################################################################## */
extern void vAssertCalled( const char * const pcFileName, unsigned long ulLine );
#define configASSERT( x )    if( ( x ) == 0 ) vAssertCalled( __FILE__, ( unsigned long ) __LINE__ )

#endif /* FREERTOS_CONFIG_H */
