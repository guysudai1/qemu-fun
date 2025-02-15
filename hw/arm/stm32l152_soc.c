#include "qemu/osdep.h"
#include "exec/memory.h"
#include "hw/arm/stm32l152_soc.h"
#include "hw/boards.h"
#include "hw/arm/boot.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "qemu/qemu-print.h"
#include "qom/object.h"
#include "hw/boards.h"
#include "target/arm/cpu-qom.h"
#include "hw/qdev-core.h"
#include "exec/address-spaces.h"
#include "hw/qdev-clock.h"
#include "hw/misc/unimp.h"
#include "hw/sysbus.h"


typedef struct {
    const char* name;
    bool is_volatile;
    hwaddr base;
    uint64_t size;
} MemoryArea;

MemoryArea unimplemented_memory_regions[] = {
    {
        .name = "Flash Registers memory",
        .base = STM32L152_FLASH_REGISTERS_BASE,
        .size = STM32L152_FLASH_REGISTERS_SIZE,
    },
    {
        .name = "PWR register memory",
        .base = STM32L152_PWR_BASE,
        .size = STM32L152_PWR_SIZE,
    },
    {
        .name = "SRAM memory",
        .base = STM32L152_SRAM_BASE,
        .size = STM32L152_SRAM_SIZE,
    }
};

// No new virtual functions: we can reuse the typedef for the
// superclass.
OBJECT_DECLARE_TYPE(STM32L152State, STM32L152Class, STM32L152_SOC)

static void stm32l152_soc_init(Object *stm32l152_obj) {
    STM32L152State* sc = STM32L152_SOC(stm32l152_obj);
    Error* errp = NULL;

    object_initialize_child(stm32l152_obj, "armv7m", &sc->armv7_cpu, TYPE_ARMV7M);

    /* RCC Memory */
    object_initialize_child(stm32l152_obj, "rcc", &sc->rcc, TYPE_STM32L152_RCC);

    /* USART1 Memory */
    object_initialize_child(stm32l152_obj, "usart1", &sc->usart1, TYPE_STM32L152_USART1);

    /* Initiailize SOC state */
    sc->sysclk = clock_new(stm32l152_obj, "SYSCLK");
    clock_set_hz(sc->sysclk, STM32L152_CPU_CLOCK_HZ);

    /*                         */
    /* Initialize memory areas */
    /*                         */

    /* Flash Memory */
    CHECK_AND_ABORT(memory_region_init_ram(&sc->flash_memory, stm32l152_obj, "flash memory", STM32L152_FLASH_SIZE, &errp));

    /* Flash Memory Alias */
    memory_region_init_alias(&sc->flash_memory_alias,stm32l152_obj, "flash memory alias", &sc->flash_memory, 0, STM32L152_FLASH_SIZE);

    /* Add all non-volatiles to container */
    memory_region_add_subregion(get_system_memory(), STM32L152_FLASH_BASE, &sc->flash_memory);
    memory_region_add_subregion(get_system_memory(), 0, &sc->flash_memory_alias);


    /* Peripheral Memory */
    memory_region_init(&sc->peripherals_container, stm32l152_obj, "peripheral registers memory", STM32L152_PERIPHERALS_SIZE);


    

    /* Add the peripherals to the volatile container */
    memory_region_add_subregion(get_system_memory(), STM32L152_PERIPHERALS_BASE - NON_VOLATILE_MEMORY_BASE, &sc->peripherals_container);
    memory_region_add_subregion(&sc->peripherals_container, STM32L152_RCC_BASE - STM32L152_PERIPHERALS_BASE, &sc->rcc.mmio);
    memory_region_add_subregion(&sc->peripherals_container, STM32L152_USART1_BASE - STM32L152_PERIPHERALS_BASE, &sc->usart1.usart1_mmio);
    memory_region_add_subregion(&sc->peripherals_container, STM32L152_GPIOB_BASE - STM32L152_PERIPHERALS_BASE, &sc->usart1.gpiob_mmio);

}

static bool stm32l152_realize_memory_areas(STM32L152State* sc, Error** errp) {
    /* Generate unimplemented device */
    for (size_t i = 0; i < sizeof(unimplemented_memory_regions) / sizeof(MemoryArea); ++i) {
        if (!memory_region_init_ram(&sc->unimplemented_memory_regions[i], OBJECT(sc), unimplemented_memory_regions[i].name, unimplemented_memory_regions[i].size, errp)) {
            return false;
        }
        memory_region_add_subregion(get_system_memory(), unimplemented_memory_regions[i].base, &sc->unimplemented_memory_regions[i]);
    }

    return true;
}

static void stm32l152_soc_realize(DeviceState *dev, Error **errp) {
    STM32L152State* sc = STM32L152_SOC(dev);

    if (!stm32l152_realize_memory_areas(sc, errp)) {
        return;
    }
    
    if (!sysbus_realize(SYS_BUS_DEVICE(&sc->rcc), errp)) {
        /* Assume error is handled and printed at the machine */
        return;
    }
    
    if (!sysbus_realize(SYS_BUS_DEVICE(&sc->usart1), errp)) {
        /* Assume error is handled and printed at the machine */
        return;
    }
    
    // Initiailize ARMv7 CPU state
    if (!object_property_set_link(OBJECT(&sc->armv7_cpu), "memory", OBJECT(get_system_memory()), errp)) {
        return;
    }
    if (!object_property_set_str(OBJECT(&sc->armv7_cpu), "cpu-type", ARM_CPU_TYPE_NAME("cortex-m3"), errp)) {
        return;
    }

    qdev_connect_clock_in(DEVICE(&sc->armv7_cpu), "cpuclk", sc->sysclk);

    if (!sysbus_realize(SYS_BUS_DEVICE(&sc->armv7_cpu), errp)) {
        /* Assume error is handled and printed at the machine */
        return;
    }
}

static void stm32l152_soc_class_init(ObjectClass *klass, void *data) {
    SysBusDeviceClass* my_class = SYS_BUS_DEVICE_CLASS(klass);
    my_class->parent_class.realize = &stm32l152_soc_realize;
}

static const TypeInfo stm32l152_soc_info = {
    .name = TYPE_STM32L152_SOC,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32L152State),
    .instance_init = stm32l152_soc_init,
    .class_init = stm32l152_soc_class_init,
};

static void
stm32l152_soc_register_types(void)
{
    type_register_static(&stm32l152_soc_info);
}
type_init(stm32l152_soc_register_types);
