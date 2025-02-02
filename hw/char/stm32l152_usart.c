
#include "qemu/osdep.h"
#include "hw/qdev-core.h"
#include "exec/memory.h"
#include "hw/char/stm32l152_usart.h"
#include "qemu/qemu-print.h"
#include "hw/arm/stm32l152_soc.h"
#include "hw/registerfields.h"
#include <stdint.h>

uint64_t stm32l152_usart1_read(void *opaque, hwaddr addr, unsigned size);
void stm32l152_usart1_write(void *opaque, hwaddr addr, uint64_t data, unsigned size);

REG32(SR, 0x00)
    FIELD(SR, CTS, 9, 1)    /* CTS Flag */
    FIELD(SR, LBD, 8, 1)    /* LIN break detection flag */
    FIELD(SR, TXE, 7, 1)    /* Transmit data register empty */
    FIELD(SR, TC, 6, 5)     /* Transmission complete */
    FIELD(SR, RXNE, 5, 5)   /* Read data register not empty */
    FIELD(SR, IDLE, 4, 1)   /* IDLE line detected */
    FIELD(SR, ORE, 3, 1)    /* Overrun error */
    FIELD(SR, NF, 2, 1)     /* Noise detected flag */
    FIELD(SR, FE, 1, 1)     /* Framing error */
    FIELD(SR, PE, 0, 1)     /* Parity error */
REG32(DR, 0x04)
    FIELD(DR, DR, 0, 9)     /* Data value */
REG32(BRR, 0x08)
    FIELD(BRR, Fraction, 4, 12)     /* mantissa of USARTDIV */
    FIELD(BRR, Mantissa, 0, 4)      /* fraction of USARTDIV */
REG32(CR1, 0x0C)
    FIELD(CR1, OVER8, 15, 1) /* Oversampling mode */
    FIELD(CR1, UE, 13, 1)   /* USART enable */
    FIELD(CR1, M0, 12, 1)    /* Word length (part 1, see M1) */
    FIELD(CR1, WAKE, 11, 1)  /* Receiver wakeup method */
    FIELD(CR1, PCE, 10, 1)   /* Parity control enable */
    FIELD(CR1, PS, 9, 1)     /* Parity selection */
    FIELD(CR1, PEIE, 8, 1)   /* PE interrupt enable */
    FIELD(CR1, TXEIE, 7, 1)  /* TXE interrupt enable */
    FIELD(CR1, TCIE, 6, 1)   /* Transmission complete interrupt enable */
    FIELD(CR1, RXNEIE, 5, 1) /* RXNE interrupt enable */
    FIELD(CR1, IDLEIE, 4, 1) /* IDLE interrupt enable */
    FIELD(CR1, TE, 3, 1)     /* Transmitter enable */
    FIELD(CR1, RE, 2, 1)     /* Receiver enable */
    FIELD(CR1, RWU, 1, 1)   /* Receiver wakeup */
    FIELD(CR1, SBK, 0, 1)   /* Send break */

/* Called whenever someone reads from the USART1 MMIO region*/
uint64_t stm32l152_usart1_read(void *opaque, hwaddr addr, unsigned size) {
    Stm32l152Usart1State* rc = STM32L152_USART1(opaque);
    uint64_t return_val = 0;
    qemu_printf("Someone tried to read from USART1. offset=0x%" PRIx64 "(size=%u). ", addr, size);

    if (size != 4) {
        qemu_printf("Bad size! Expcted 4 received size=%u\n", size);
        return 0;
    }

    switch (addr) {
         case 0x0: {
        return_val = rc->state.sr;
        break;
}
        case 0x4: {
        return_val = rc->state.dr;
        break;
}
        case 0x8: {
        return_val = rc->state.brr;
        break;
}
        case 0xc: {
        return_val = rc->state.cr1;
        break;
}
        case 0x10: {
        return_val = rc->state.cr2;
        break;
}
        case 0x14: {
        return_val = rc->state.cr3;
        break;
}
        case 0x18: {
        return_val = rc->state.gtpr;
        break;
}
        default: {
            qemu_printf("Bad offset! received offset=0x%" PRIx64 "\n", addr);
            return 0;
        }

    }

    qemu_printf("data=0x%" PRIx64 "\n", return_val);
    return return_val;
}

/* Called whenever someone writes to the USART1 MMIO region*/
void stm32l152_usart1_write(void *opaque, hwaddr addr, uint64_t data, unsigned size) {
    qemu_printf("Someone tried to write to USART1. offset=0x%" PRIx64 " with data=0x%" PRIx64 "(size=%u)\n", addr, data, size);
    Stm32l152Usart1State* rc = STM32L152_USART1(opaque);

    if (size != 4) {
        qemu_printf("Bad size! Expcted 4 received size=%u\n", size);
        return;
    }

    switch (addr) {
        case 0x0: {
        rc->state.sr = data;
        break;
}
        case 0x4: {
        rc->state.dr = data;
        if (FIELD_EX32(rc->state.sr, SR, TXE)) {
            rc->state.sr = FIELD_DP32(rc->state.sr, SR, TXE, 0);
            qemu_printf("Turned off TXE!\n");
        }
        // TODO: Send the data here and turn on TC after finishing
        break;
}
        case 0x8: {
        rc->state.brr = data;
        break;
}
        case 0xc: {
        rc->state.cr1 = data;
        if (FIELD_EX32(data, CR1, UE)) {
            // Turn on TXE
            rc->state.sr = FIELD_DP32(rc->state.sr, SR, TXE, 1);
            qemu_printf("Turned on TXE!\n");
        }
        break;
}
        case 0x10: {
        rc->state.cr2 = data;
        break;
}
        case 0x14: {
        rc->state.cr3 = data;
        break;
}
        case 0x18: {
        rc->state.gtpr = data;
        break;
}
        default: {
            qemu_printf("Bad offset! received offset=0x%" PRIx64 "\n", addr);
            return;
        }

    }
}

static MemoryRegionOps usart1_mops = {
    .endianness = DEVICE_NATIVE_ENDIAN,
    .read = &stm32l152_usart1_read,
    .write = &stm32l152_usart1_write,
};

// static MemoryRegionOps gpiob_mops = {
//     .endianness = DEVICE_NATIVE_ENDIAN
// };


static void stm32l152_usart1_init(Object *stm32l152_obj) {
    Stm32l152Usart1State* rc = STM32L152_USART1(stm32l152_obj);
    Error* errp = NULL;

    memory_region_init_io(&rc->usart1_mmio, OBJECT(stm32l152_obj), &usart1_mops, rc, "USART1", STM32L152_USART1_SIZE);

    memory_region_init_ram(&rc->gpiob_mmio, OBJECT(stm32l152_obj), "GPIOB", STM32L152_GPIOB_SIZE, &errp);
    // memory_region_init_io(&rc->gpiob_mmio, OBJECT(stm32l152_obj), &gpiob_mops, rc, "GPIOB", STM32L152_GPIOB_SIZE);
}

static void stm32l152_usart1_realize(DeviceState *dev, Error **errp) {
    Stm32l152Usart1State* rc = STM32L152_USART1(dev);

    qdev_init_gpio_out_named(dev, &rc->usart_irq, "usart1.gpio", 1);
}

static void stm32l152_usart1_class_init(ObjectClass *klass, void *data)
{
    SysBusDeviceClass* sysclass = SYS_BUS_DEVICE_CLASS(klass);
    sysclass->parent_class.realize = &stm32l152_usart1_realize;
}

static const TypeInfo stm32l152_usart1_types[] = {
    {
        .name           = TYPE_STM32L152_USART1,
        .parent         = TYPE_SYS_BUS_DEVICE,
        .instance_size  = sizeof(Stm32l152Usart1State),
        .instance_init  = stm32l152_usart1_init,
        .class_init     = stm32l152_usart1_class_init,
    }
};

DEFINE_TYPES(stm32l152_usart1_types)
