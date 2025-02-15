
#include "qemu/osdep.h"
#include "exec/memory.h"
#include "hw/misc/stm32l152_rcc.h"
#include "qemu/qemu-print.h"
#include "hw/arm/stm32l152_soc.h"
#include <stdint.h>
#include "exec/address-spaces.h"

uint64_t stm32l152_rcc_read(void *opaque, hwaddr addr, unsigned size);
void stm32l152_rcc_write(void *opaque, hwaddr addr, uint64_t data, unsigned size);


/* Called whenever someone reads from the RCC MMIO region*/
uint64_t stm32l152_rcc_read(void *opaque, hwaddr addr, unsigned size) {
    Stm32l152RccState* rc = STM32L152_RCC(opaque);
    uint64_t return_val = 0;
    qemu_printf("Someone tried to read from RCC. offset=0x%" PRIx64 "(size=%u). ", addr, size);

    if (size != 4) {
        qemu_printf("Bad size! Expcted 4 received size=%u\n", size);
        return 0;
    }

    switch (addr) {
        case 0x0:
            return_val = rc->state.cr;
            break;
        case 0x4:
            return_val = rc->state.icscr;
            break;
        case 0x8:
            return_val = rc->state.cfgr;
            break;
        case 0xc:
            return_val = rc->state.cir;
            break;
        case 0x10:
            return_val = rc->state.ahbrstr;
            break;
        case 0x14:
            return_val = rc->state.ahb2rstr;
            break;
        case 0x18:
            return_val = rc->state.ahb1rstr;
            break;
        case 0x1c:
            return_val = rc->state.ahbenr;
            break;
        case 0x20:
            return_val = rc->state.apb2enr;
            break;
        case 0x24:
            return_val = rc->state.apb1enr;
            break;
        case 0x28:
            return_val = rc->state.ahblpenr;
            break;
        case 0x2c:
            return_val = rc->state.ahb2lpenr;
            break;
        case 0x30:
            return_val = rc->state.ahb1lpenr;
            break;
        case 0x34:
            return_val = rc->state.csr;
            break;
        default: {
            qemu_printf("Bad offset! received offset=0x%" PRIx64 "\n", addr);
            return 0;
        }

    }

    qemu_printf("data=0x%" PRIx64 "\n", return_val);
    return return_val;
}

/* Called whenever someone writes to the RCC MMIO region*/
void stm32l152_rcc_write(void *opaque, hwaddr addr, uint64_t data, unsigned size) {
    qemu_printf("Someone tried to write to RCC. offset=0x%" PRIx64 " with data=0x%" PRIx64 "(size=%u)\n", addr, data, size);
    Stm32l152RccState* rc = STM32L152_RCC(opaque);

    if (size != 4) {
        qemu_printf("Bad size! Expcted 4 received size=%u\n", size);
        return;
    }

    switch (addr) {
        case 0x0: {
        rc->state.cr = data;
        break;
}
        case 0x4: {
        rc->state.icscr = data;
        STM32L152_BITSET(rc->state.cr, MSIRDY_BIT); // Set MSIRDY bit
        break;
}
        case 0x8: {
        rc->state.cfgr = data;
        break;
}
        case 0xc: {
        rc->state.cir = data;
        break;
}
        case 0x10: {
        rc->state.ahbrstr = data;
        break;
}
        case 0x14: {
        rc->state.ahb2rstr = data;
        break;
}
        case 0x18: {
        rc->state.ahb1rstr = data;
        break;
}
        case 0x1c: {
        rc->state.ahbenr = data;
        break;
}
        case 0x20: {
        rc->state.apb2enr = data;
        break;
}
        case 0x24: {
        rc->state.apb1enr = data;
        break;
}
        case 0x28: {
        rc->state.ahblpenr = data;
        break;
}
        case 0x2c: {
        rc->state.ahb2lpenr = data;
        break;
}
        case 0x30: {
        rc->state.ahb1lpenr = data;
        break;
}
        case 0x34: {
        rc->state.csr = data;
        break;
}
        default: {
            qemu_printf("Bad offset! received offset=0x%" PRIx64 "\n", addr);
            return;
        }

    }
}

static MemoryRegionOps mops = {
    .endianness = DEVICE_NATIVE_ENDIAN,
    .read = &stm32l152_rcc_read,
    .write = &stm32l152_rcc_write,
};

static void stm32l152_rcc_init(Object *stm32l152_obj) {
    Stm32l152RccState* rc = STM32L152_RCC(stm32l152_obj);

    memory_region_init_io(&rc->mmio, OBJECT(stm32l152_obj), &mops, rc, "RCC", STM32L152_RCC_SIZE);
    memory_region_add_subregion(get_system_memory(), STM32L152_RCC_BASE, &rc->mmio);
}


static const TypeInfo stm32l152_rcc_types = {
        .name           = TYPE_STM32L152_RCC,
        .parent         = TYPE_SYS_BUS_DEVICE,
        .instance_size  = sizeof(Stm32l152RccState),
        .instance_init  = stm32l152_rcc_init,
};

static void
stm32l152_rcc_register_types(void)
{
    type_register_static(&stm32l152_rcc_types);
}
type_init(stm32l152_rcc_register_types);
