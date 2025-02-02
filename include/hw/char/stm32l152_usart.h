/*
 * STM32L152 USART
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

#ifndef HW_STM32L152_USART_H
#define HW_STM32L152_USART_H

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "qom/object.h"

#define STM32L152_USART1_BITCLR(a, bit) (a & ~(1ull << bit))
#define STM32L152_USART1BITSET(a, bit) (a = (a | 1ull << bit))
#define STM32L152_USART1BITREV(a, bit) (a ^ (1ull << bit))

#define TYPE_STM32L152_USART1 "stm32l152-usart1"
OBJECT_DECLARE_SIMPLE_TYPE(Stm32l152Usart1State, STM32L152_USART1)

struct Stm32l152Usart1State {
    SysBusDevice parent_obj;

    MemoryRegion usart1_mmio;
    MemoryRegion gpiob_mmio;

    struct __attribute__((packed)) {
        uint32_t sr;
        uint32_t dr;  /* data register - used for TX and RX */
        uint32_t brr;
        uint32_t cr1;
        uint32_t cr2;
        uint32_t cr3;
        uint32_t gtpr;
    } state;

    qemu_irq usart_irq;
};

#endif /* HW_STM32L152_USART_H */
