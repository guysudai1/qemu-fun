/*
 * STM32L152 RCC (Reset and clock control)
 *
 * Copyright (c) 2025 Guy Sudai
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 * The reference used is the STMicroElectronics RM0351 Reference manual
 * for STM32L151xx and STM32L152xx advanced Arm ® -based 32-bit MCUs.
 *
 * Inspired by the STM32L4x5 by Aranaud Minier
 */

#ifndef HW_STM32L152_RCC_H
#define HW_STM32L152_RCC_H

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "qom/object.h"


#define MSIRDY_BIT (9)

#define STM32L152_BITCLR(a, bit) (a & ~(1ull << bit))
#define STM32L152_BITSET(a, bit) (a = (a | 1ull << bit))
#define STM32L152_BITREV(a, bit) (a ^ (1ull << bit))

#define TYPE_STM32L152_RCC "stm32l152-rcc"
OBJECT_DECLARE_SIMPLE_TYPE(Stm32l152RccState, STM32L152_RCC)

struct Stm32l152RccState {
    SysBusDevice parent_obj;

    MemoryRegion mmio;

    struct __attribute__((packed)) {
        uint32_t cr;
        uint32_t icscr;
        uint32_t cfgr;
        uint32_t cir;
        uint32_t ahbrstr;
        uint32_t ahb2rstr;
        uint32_t ahb1rstr;
        
        uint32_t ahbenr;
        uint32_t apb2enr;
        uint32_t apb1enr;
        
        uint32_t ahblpenr;
        uint32_t ahb2lpenr;
        uint32_t ahb1lpenr;

        uint32_t csr;
    } state;
};

#endif /* HW_STM32L152_RCC_H */
