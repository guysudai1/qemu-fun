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

#include "hw/sysbus.h"
#include "hw/arm/armv7m.h"
#include "hw/qdev-core.h"
#include "qemu/typedefs.h"
#include "qom/object.h"
#include "hw/clock.h"
#include "qemu/units.h"

typedef struct STM32L152State
{
    SysBusDevice parent_obj;

    ARMv7MState armv7_cpu;

    // TODO: Add SOC state here
    Clock *sysclk;
    Clock *refclk;
} STM32L152State;

typedef struct STM32L152Class {
    SysBusDeviceClass parent_class;
} STM32L152Class;


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

#define STM32L152_FLASH_SIZE (256 * KiB)
#define STM32L152_FLASH_BASE (0x08000000u)

#define STM32L152_CPU_CLOCK_HZ (16 * MHz)

#endif 