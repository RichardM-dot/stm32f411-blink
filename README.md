# stm32f411-blink

STM32F411xE 裸机（bare-metal）+ HAL 工程模板：板载 LED（PA5）以 500 ms 周期闪烁。
使用 **make + 本地 ARM GNU 工具链** 构建，不依赖任何 IDE。

## 工程结构

```
stm32f411-blink/
├── Makefile                  # 构建/反汇编/符号表等全部目标，指向本地工具链
├── STM32F411xE_FLASH.ld      # GNU ld 链接脚本（512K Flash / 128K SRAM）
├── startup_stm32f411xe.s     # ST 官方启动文件（向量表、Reset_Handler）
├── Config/                   # RTOS 集成配置（位于 Core/ 之外）
│   ├── FreeRTOSConfig.h          # FreeRTOS V11.1.0 内核配置
│   └── stm32f4xx_hal_timebase.c  # HAL 时基(TIM3) + FreeRTOS 钩子函数
├── Core/
│   ├── Inc/
│   │   ├── main.h                # 应用层配置（LED 引脚定义）
│   │   ├── stm32f4xx_hal_conf.h  # HAL 模块裁剪配置（RCC/GPIO/DMA/TIM/FLASH/PWR/CORTEX）
│   │   └── stm32f4xx_it.h        # 中断服务函数原型
│   └── Src/
│       ├── main.c                # 应用入口：时钟树 + GPIO 初始化 + 主循环
│       ├── stm32f4xx_it.c        # Cortex-M4 异常处理（应用侧，用户自写区域）
│       ├── stm32f4xx_hal_msp.c   # HAL MSP 底层初始化钩子
│       └── system_stm32f4xx.c    # ST 系统初始化（FPU/VTOR）
├── Middlewares/Third_Party/FreeRTOS/   # FreeRTOS-Kernel V11.1.0（fork 副本）
│   ├── Source/{*.c, include/}          # 内核核心（tasks/queue/list/timers/...）
│   └── portable/GCC/ARM_CM4F/          # Cortex-M4F 端口（port.c）
│   └── portable/MemMang/heap_4.c       # 内存管理（32 KB 堆）
├── Drivers/                      # 第三方库（本地副本，随工程 vendored）
│   ├── CMSIS/
│   │   ├── Include/              # CMSIS-Core 6.1 头文件（core_cm4.h 等）
│   │   └── Device/ST/STM32F4xx/  # ST 设备头文件（stm32f4xx.h、stm32f411xe.h）
│   └── STM32F4xx_HAL_Driver/     # STM32F4 HAL 驱动（Inc/Src，按需编译）
└── build/                        # 构建产物（git 忽略）
```

## 库文件来源（provenance）

| 工程内路径 | 来源 |
|---|---|
| `Middlewares/Third_Party/FreeRTOS/` | [RichardM-dot 仓库](https://github.com/FreeRTOS/FreeRTOS-Kernel) 的 FreeRTOS-Kernel **V11.1.0**（本地：`references/rtos/freertos`，含中文注释） |
| `Drivers/CMSIS/Device/ST/STM32F4xx/` | [RichardM-dot/cmsis-device-f4](https://github.com/RichardM-dot/cmsis-device-f4)（ST 官方仓库的 fork，已克隆至 `libraries/stm32/cmsis-device-f4/`） |
| `Drivers/CMSIS/Include/` | `libraries/stm32/cmsis/cmsis-core/`（ARM CMSIS-Core 官方仓库） |
| `Drivers/STM32F4xx_HAL_Driver/` | `libraries/stm32/STM32F411/stm32f4xx-hal-driver/`（ST 官方 HAL 驱动） |

## 工具链

本地 ARM GNU Toolchain **15.3.Rel2**（darwin-arm64）：

```
/Users/theo-richard/workspace/toolchains/arm-none-eabi
```

Makefile 中的所有编译器/链接器二进制均通过 `TOOLCHAIN_DIR` 变量指向该目录，
无需全局 PATH。目标核心配置：`-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard`，
链接 newlib-nano（`--specs=nano.specs --specs=nosys.specs`）并开启 `--gc-sections`。

## 构建

```bash
make            # 默认目标：.elf + .hex + .bin + size 汇总
make dis        # 生成 build/stm32f411_blink.dis（含源码交错反汇编）
make symbols    # 生成 build/stm32f411_blink.sym（按体积排序的符号表）
make ccdb       # 生成 compile_commands.json（编辑器代码导航，见下）
make clean      # 清理
```

### 编辑器代码导航

所有库（HAL 99 头 + 94 源、CMSIS 69 头、FreeRTOS）都是工程内的实体文件，
编译只使用工程内相对路径——整个工程自包含。要让编辑器能**跳转到定义**
（HAL_Delay、xTaskCreate 等），跑一次：

```bash
make ccdb       # 在工程根目录生成 compile_commands.json
```

- VS Code：安装 **clangd** 扩展，打开工程根目录即可
- CLion / Zed：原生读取 compile_commands.json，无需配置
- 增删源文件后重新 `make ccdb` 刷新

当前固件尺寸：**text 4476 B + data 20 B**（Flash 占用 4496 B / 512 KB ≈ 0.86%），
**bss 1572 B**（RAM 1.21%）。链接期打印 RAM/FLASH 占用（`--print-memory-usage`）。

> 链接时出现的 `_close/_read/_write ... will always fail` 警告来自 newlib 的
> nosys 系统调用桩，裸机工程属正常现象；不使用 printf 等流 IO 时无任何影响。

## 硬件配置

- 目标 MCU：**STM32F411xE**（512 KB Flash / 128 KB SRAM，Cortex-M4F）
- LED：**PA5** 推挽输出（Nucleo-F411RE 的 LD2）；若用 WeAct BlackPill 请改
  `Core/Inc/main.h` 中的 `LED_GPIO_PORT/LED_PIN`（PC13、低电平点亮）
- 时钟：**HSI 16 MHz → PLL(M=16, N=200, P=2) → SYSCLK 100 MHz**，AHB/1，
  APB1/2 = 50 MHz，APB2/1，Flash 3 等待周期。不依赖 HSE 晶振，任何 F411 板子可直接运行
- HAL 时基：SysTick 1 ms（`TICK_INT_PRIORITY = 0x0F`）

## 固件逆向分析（验证构建正确性）

构建后使用 `make dis && make symbols` 生成分析材料，核心结论：

| 层级 | 验证点 | 结果 |
|---|---|---|
| 镜像 | `.bin` 前 4 字节 = `0x20020000`（初始 SP = SRAM 顶），复位向量 `0x08000d81`（Thumb 位） | ✅ |
| 镜像 | Intel HEX 扩展地址记录 `:020000040800F2` → 数据位于 0x0800xxxx Flash 基址 | ✅ |
| ELF | `.data` VMA 0x20000000 / LMA 0x08001184；`.bss` NOLOAD；入口 0x08000d81 | ✅ |
| 启动 | `Reset_Handler`：装载 SP → `bl SystemInit` → .data 从 Flash 拷贝循环 → .bss 清零循环 → `bl main` | ✅ |
| 系统 | `SystemInit`：`SCB->CPACR \|= 0x00F00000`（CP10/CP11 全访问，使能 FPU）；`SCB->VTOR = 0x08000000` | ✅ |
| 应用 | `main`：`HAL_Init → SystemClock_Config → MX_GPIO_Init`，主循环 `TogglePin(GPIOA=0x40020000, 0x20)` + `HAL_Delay(500)` | ✅ |
| 时钟 | APB1ENR `\|= 0x10000000`（PWR）；`PWR->CR \|= 0xC000`（SCALE1）；OscInitStruct 字段 M=16/N=200/P=2/Q=4；`FLASH_LATENCY_3`；APB1 分频 0x1000 | ✅ |
| GPIO | `AHB1ENR \|= 1`（GPIOA 时钟）；`HAL_GPIO_WritePin(0x20, RESET)` 灭灯；Mode=1（推挽输出） | ✅ |
| GPIO | `HAL_GPIO_TogglePin`：读 ODR（+0x14）→ 高 16 位复位 + 低 16 位置位写 BSRR（+0x18）原子翻转 | ✅ |
| 节拍 | `HAL_SYSTICK_Config`：LOAD=ticks-1、VAL=0、CTRL=7；时钟切换后 `HAL_RCC_ClockConfig` 内部重调 SysTick（16 kHz→100 kHz 计数） | ✅ |

分析示例（`build/stm32f411_blink.dis`）：

```asm
800034c <main>:
  bl  8000428 <HAL_Init>
  bl  80002b0 <SystemClock_Config>
  bl  8000258 <MX_GPIO_Init>
  movs r1, #32            ; GPIO_PIN_5 = 0x20
  ldr  r0, [pc, #12]      ; 0x40020000 = GPIOA
  bl   8000768 <HAL_GPIO_TogglePin>
  mov.w r0, #500
  bl   8000478 <HAL_Delay>
  b.n  <loop>

8000768 <HAL_GPIO_TogglePin>:       ; ODR→BSRR 原子翻转
  ldr  r3, [r0, #20]      ; GPIOA->ODR
  and.w r2, r1, r3
  bic.w r1, r1, r3
  orr.w r1, r1, r2, lsl #16
  str  r1, [r0, #24]      ; GPIOA->BSRR
```

## FreeRTOS 集成（V11.1.0，Core/ 零改动）

调度器与时钟的所有权划分（STM32CubeMX 同款模式）：

| 资源 | 归属 | 说明 |
|---|---|---|
| **SysTick** | FreeRTOS | `startup_stm32f411xe.s` 向量表直接指向 `vPortSVCHandler` / `xPortPendSVHandler` / `xPortSysTickHandler` |
| **TIM3 @ 1 kHz** | HAL 时基 | `Config/stm32f4xx_hal_timebase.c` 重写 `HAL_InitTick/HAL_SuspendTick/HAL_ResumeTick`，`uwTick/HAL_GetTick/HAL_Delay` 不受 RTOS 影响 |
| **堆** | heap_4 | `configTOTAL_HEAP_SIZE = 32 KB`，任务/队列/信号量用 `xTaskCreate` 等标准 API 分配 |

`Core/` 文件夹未做任何添加或修改——应用代码（任务、队列、自己的 it.c 处理函数）由你自己编写。

### 使用方法（在你自己的代码里）

```c
#include "FreeRTOS.h"
#include "task.h"

static void my_task(void *arg)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));       /* 1 kHz SysTick 节拍 */
    }
}

/* 在 main() 的外设初始化之后： */
xTaskCreate(my_task, "blink", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
vTaskStartScheduler();                        /* 之后不再返回 */
```

### 中断优先级规则（Cortex-M4F，4 位优先级）

- 需要调用 FreeRTOS `FromISR` API 的中断：抢占优先级数值必须 **≥ 5**
  （即 `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`）
- 优先级 0–4（更高）：禁止调用任何 FreeRTOS API
- TIM3（HAL 时基）当前为最低优先级 15，仅调用 `HAL_IncTick`，无此限制

### 当前链接状态说明

`--gc-sections` 会把尚未被引用的内核对象裁掉（当前 `ucHeap` 未链入，
RAM 仅 1840 B）。一旦你的代码调用 `xTaskCreate/vTaskStartScheduler`，
完整内核 + 32 KB 堆会自动链接进来（Flash 约 +20 KB / RAM +33 KB）。

## 扩展指南

- 新增 HAL 模块（如 UART）：在 `Core/Inc/stm32f4xx_hal_conf.h` 打开
  `HAL_xxx_MODULE_ENABLED`，在 Makefile 的 `C_SOURCES` 加入对应
  `Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_xxx.c`
- 烧录：`build/stm32f411_blink.bin` 从 0x08000000 开始（ST-Link/OpenOCD/dfu-util 均可）
