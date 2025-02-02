
#include "qemu/osdep.h"
#include "hw/qdev-core.h"
#include "exec/memory.h"
#include "hw/char/stm32l152_usart.h"
#include "qemu/qemu-print.h"
#include "hw/arm/stm32l152_soc.h"
#include <stdint.h>

uint64_t stm32l152_usart1_read(void *opaque, hwaddr addr, unsigned size);
void stm32l152_usart1_write(void *opaque, hwaddr addr, uint64_t data, unsigned size);


/* Called whenever someone reads from the USART1 MMIO region*/
uint64_t stm32l152_usart1_read(void *opaque, hwaddr addr, unsigned size) {
    // Stm32l152Usart1State* rc = STM32L152_USART1(opaque);
    uint64_t return_val = 0;
    qemu_printf("Someone tried to read from USART1. offset=0x%" PRIx64 "(size=%u). ", addr, size);

    if (size != 4) {
        qemu_printf("Bad size! Expcted 4 received size=%u\n", size);
        return 0;
    }

    switch (addr) {
        // case 0x0:
        //     return_val = rc->state.cr;
        //     break;
        // case 0x4:
        //     return_val = rc->state.icscr;
        //     break;
        // case 0x8:
        //     return_val = rc->state.cfgr;
        //     break;
        // case 0xc:
        //     return_val = rc->state.cir;
        //     break;
        // case 0x10:
        //     return_val = rc->state.ahbrstr;
        //     break;
        // case 0x14:
        //     return_val = rc->state.ahb2rstr;
        //     break;
        // case 0x18:
        //     return_val = rc->state.ahb1rstr;
        //     break;
        // case 0x1c:
        //     return_val = rc->state.ahbenr;
        //     break;
        // case 0x20:
        //     return_val = rc->state.apb2enr;
        //     break;
        // case 0x24:
        //     return_val = rc->state.apb1enr;
        //     break;
        // case 0x28:
        //     return_val = rc->state.ahblpenr;
        //     break;
        // case 0x2c:
        //     return_val = rc->state.ahb2lpenr;
        //     break;
        // case 0x30:
        //     return_val = rc->state.ahb1lpenr;
        //     break;
        // case 0x34:
        //     return_val = rc->state.csr;
        //     break;
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
    // Stm32l152Usart1State* rc = STM32L152_USART1(opaque);

    if (size != 4) {
        qemu_printf("Bad size! Expcted 4 received size=%u\n", size);
        return;
    }

    switch (addr) {

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
