/*
 * STM32L152 SoC
 *
 * Copyright (c) 2021 Alexandre Iooss <erdnaxe@crans.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef HW_ARM_STM32L152_SOC_H
#define HW_ARM_STM32L152_SOC_H

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/arm/armv7m.h"
#include "hw/qdev-core.h"
#include "qemu/typedefs.h"
#include "qom/object.h"
#include "hw/clock.h"
#include "qemu/units.h"
#include "hw/misc/stm32l152_rcc.h"
#include "hw/char/stm32l152_usart.h"


#define MAX_UNIMPLEMENTED_MEMORY_REGIONS (50)

typedef struct STM32L152State
{
    SysBusDevice parent_obj;

    Stm32l152RccState rcc;
    Stm32l152Usart1State usart1;

    ARMv7MState armv7_cpu;

    // TODO: Add SOC state here
    Clock *sysclk;
    Clock *refclk;

    MemoryRegion peripherals_container;
    
    MemoryRegion unimplemented_memory_regions[MAX_UNIMPLEMENTED_MEMORY_REGIONS];

    MemoryRegion flash_memory;
    MemoryRegion flash_memory_alias;
} STM32L152State;


#define CHECK_AND_ABORT(x) if (!(x)) { \
    verbose_report_err(errp); \
    abort(); \
}


#define TYPE_STM32L152_SOC "stm32l152-soc"

/* Memory Map - taken from reference manual */

/* 0x20000000 - 0xFFFFFFFF - Volatile Memory */
    /* 0x20000000 - 0x2000BFFF - SRAM */
    /* 0x40000000 - 0x400267FF - Peripherals */

/* 0x00000000 - 0x20000000 - Non Volatile Memory */
    /* 0x00000000 - 0x00060000 - Flash/SRAM/System Alias (means that the size is just based on the original mapped region)*/
    /* 0x08000000 - 0x08060000 - Flash Memory  */
    /*              RESERVED                   */
    /* 0x08080000 - 0x08082000 - Data EEPROM   */
    /*              RESERVED                   */
    /* 0x1FF00000 - 0x1FF02000 - System Memory */
    /*              RESERVED                   */
    /* 0x1FF80000 - 0x1FF80020 - Option byte   */
    /*              RESERVED                   */

#define NON_VOLATILE_MEMORY_BASE (0x00000000)
#define NON_VOLATILE_MEMORY_END  (0x20000000)
#define NON_VOLATILE_MEMORY_SIZE (NON_VOLATILE_MEMORY_END - NON_VOLATILE_MEMORY_BASE)


#define STM32L152_SRAM_BASE (0x20000000)
#define STM32L152_SRAM_END  (0x2000C000)
#define STM32L152_SRAM_SIZE (STM32L152_SRAM_END - STM32L152_SRAM_BASE)

#define STM32L152_PERIPHERALS_BASE (0x40000000)
#define STM32L152_PERIPHERALS_END  (0x40026800)
#define STM32L152_PERIPHERALS_SIZE (STM32L152_PERIPHERALS_END - STM32L152_PERIPHERALS_BASE)

#define STM32L152_RCC_BASE (0x40023800)
#define STM32L152_RCC_END  (0x40023C00)
#define STM32L152_RCC_SIZE (STM32L152_RCC_END - STM32L152_RCC_BASE)

#define STM32L152_USART1_BASE (0x40013800)
#define STM32L152_USART1_END  (0x40013C00)
#define STM32L152_USART1_SIZE (STM32L152_USART1_END - STM32L152_USART1_BASE)

#define STM32L152_GPIOB_BASE (0x40020400)
#define STM32L152_GPIOB_END  (0x40020800)
#define STM32L152_GPIOB_SIZE (STM32L152_GPIOB_END - STM32L152_GPIOB_BASE)

#define STM32L152_FLASH_REGISTERS_BASE (0x40023C00)
#define STM32L152_FLASH_REGISTERS_END  (0x40024000)
#define STM32L152_FLASH_REGISTERS_SIZE (STM32L152_FLASH_REGISTERS_END - STM32L152_FLASH_REGISTERS_BASE)


#define STM32L152_PWR_BASE (0x40007000)
#define STM32L152_PWR_END  (0x40007400)
#define STM32L152_PWR_SIZE (STM32L152_PWR_END - STM32L152_PWR_BASE)

#define VOLATILE_MEMORY_BASE (0x20000000u)
#define VOLATILE_MEMORY_END  (0xFFFFFFFFu)
#define VOLATILE_MEMORY_SIZE (VOLATILE_MEMORY_END - VOLATILE_MEMORY_BASE)

#define STM32L152_FLASH_BASE (0x08000000u)
#define STM32L152_FLASH_SIZE (256 * KiB)

#define STM32L152_CPU_CLOCK_HZ (16 * MHz)

#endif 