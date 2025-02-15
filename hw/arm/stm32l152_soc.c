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


typedef struct {
    const char* name;
    hwaddr base;
    uint64_t size;
} MemoryArea;

MemoryArea unimplemented_memory_regions[MAX_MEMORY_REGIONS] = {
    {
        .name = NULL,
        .base = 0,
        .size = 0,
    }
};

// No new virtual functions: we can reuse the typedef for the
// superclass.
OBJECT_DECLARE_TYPE(STM32L152State, STM32L152Class, STM32L152_SOC)

static void stm32l152_soc_init(Object *stm32l152_obj) {
    STM32L152State* sc = STM32L152_SOC(stm32l152_obj);
    Error* errp = NULL;

    Object* armv7_object = OBJECT(&sc->armv7_cpu);
    object_initialize_child(stm32l152_obj, "armv7m", &sc->armv7_cpu, TYPE_ARMV7M);

    // Initiailize ARMv7 CPU state
    CHECK_AND_ABORT(object_property_set_link(armv7_object, "memory", OBJECT(get_system_memory()), &errp));
    CHECK_AND_ABORT(object_property_set_str(armv7_object, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m3"), &errp));

    /* Initiailize SOC state */
    Clock* sysclk = clock_new(stm32l152_obj, "SYSCLK");
    clock_set_hz(sysclk, STM32L152_CPU_CLOCK_HZ);
    qdev_init_clock_out(DEVICE(stm32l152_obj), "sysclk");

    sc->sysclk = sysclk; 

    /*                         */
    /* Initialize memory areas */
    /*                         */

    /* Non-Volatile memory */
    memory_region_init(&sc->non_volatile_container, stm32l152_obj, "non volatile memory", VOLATILE_MEMORY_SIZE);
    memory_region_add_subregion(get_system_memory(), NON_VOLATILE_MEMORY_BASE, &sc->non_volatile_container);

    /* Volatile memory */
    memory_region_init(&sc->volatile_container, stm32l152_obj, "volatile memory", VOLATILE_MEMORY_SIZE);
    memory_region_add_subregion(get_system_memory(), NON_VOLATILE_MEMORY_BASE, &sc->volatile_container);


    /* Flash Memory */
    CHECK_AND_ABORT(memory_region_init_ram(&sc->flash_memory, stm32l152_obj, "flash memory", STM32L152_FLASH_SIZE, &errp));

    /* Flash Memory Alias */
    memory_region_init_alias(&sc->flash_memory_alias,stm32l152_obj, "flash memory alias", &sc->flash_memory, 0, STM32L152_FLASH_SIZE);

    /* Add all non-volatiles to container */
    memory_region_add_subregion(&sc->non_volatile_container, STM32L152_FLASH_BASE, &sc->flash_memory);
    memory_region_add_subregion(&sc->non_volatile_container, 0, &sc->flash_memory_alias);


    /* Peripheral Memory */
    memory_region_init(&sc->peripherals_container, stm32l152_obj, "peripheral registers memory", STM32L152_PERIPHERALS_SIZE);

    /* RCC Memory */
    object_initialize_child(stm32l152_obj, "rcc", &sc->rcc, TYPE_STM32L152_RCC);

    /* USART1 Memory */
    object_initialize_child(stm32l152_obj, "usart1", &sc->usart1, TYPE_STM32L152_USART1);

    /* PWR Memory */
    // TODO: Turn this to MMIO
    CHECK_AND_ABORT(memory_region_init_ram(&sc->pwr_memory, stm32l152_obj, "PWR register memory", STM32L152_PWR_SIZE, &errp));

    /* Flash Registers Memory */
    // TODO: Turn this to MMIO
    CHECK_AND_ABORT(memory_region_init_ram(&sc->flash_registers_memory, stm32l152_obj, "Flash Registers memory", STM32L152_FLASH_REGISTERS_SIZE, &errp));
    
    /* SRAM Memory */
    CHECK_AND_ABORT(memory_region_init_ram(&sc->sram_memory, stm32l152_obj, "SRAM memory", STM32L152_SRAM_SIZE, &errp));

    /* Add the peripherals to the volatile container */
    memory_region_add_subregion(&sc->volatile_container, STM32L152_PERIPHERALS_BASE - NON_VOLATILE_MEMORY_BASE, &sc->peripherals_container);
    memory_region_add_subregion(&sc->volatile_container, STM32L152_SRAM_BASE, &sc->sram_memory);
    memory_region_add_subregion(&sc->peripherals_container, STM32L152_RCC_BASE - STM32L152_PERIPHERALS_BASE, &sc->rcc.mmio);
    memory_region_add_subregion(&sc->peripherals_container, STM32L152_USART1_BASE - STM32L152_PERIPHERALS_BASE, &sc->usart1.usart1_mmio);
    memory_region_add_subregion(&sc->peripherals_container, STM32L152_GPIOB_BASE - STM32L152_PERIPHERALS_BASE, &sc->usart1.gpiob_mmio);
    memory_region_add_subregion(&sc->peripherals_container, STM32L152_PWR_BASE - STM32L152_PERIPHERALS_BASE, &sc->pwr_memory);
    memory_region_add_subregion(&sc->peripherals_container, STM32L152_FLASH_REGISTERS_BASE - STM32L152_PERIPHERALS_BASE, &sc->flash_registers_memory);


    /* Generate unimplemented device */
    for (size_t i = 0; i < sizeof(unimplemented_memory_regions) / sizeof(MemoryArea); ++i) {
        if (unimplemented_memory_regions[i].name == NULL) {
            break;
        }
        create_unimplemented_device(unimplemented_memory_regions[i].name,
                            unimplemented_memory_regions[i].base,
                            unimplemented_memory_regions[i].size);
    }
}

static void stm32l152_soc_realize(DeviceState *dev, Error **errp) {
    /* TODO: Realize and connect everything here */
    STM32L152State* sc = STM32L152_SOC(dev);

    /* No need to call the sysbus realize since it is empty */

    qdev_connect_clock_in(DEVICE(&sc->armv7_cpu), "cpuclk", sc->sysclk);

    
    if (!qdev_realize(DEVICE(&sc->rcc), sysbus_get_default(), errp)) {
        /* Assume error is handled and printed at the machine */
        return;
    }


    // TODO: Make the USART get set using a property
    if (!qdev_realize(DEVICE(&sc->usart1), sysbus_get_default(), errp)) {
        /* Assume error is handled and printed at the machine */
        return;
    }

    
    if (!qdev_realize(DEVICE(&sc->armv7_cpu), sysbus_get_default(), errp)) {
        /* Assume error is handled and printed at the machine */
        return;
    }

    // TODO: soc_reset(dev);
}

static void stm32l152_soc_class_init(ObjectClass *klass, void *data) {
    SysBusDeviceClass* my_class = SYS_BUS_DEVICE_CLASS(klass);
    my_class->parent_class.realize = &stm32l152_soc_realize;
}

static const TypeInfo stm32l152_soc_info = {
    .parent = TYPE_SYS_BUS_DEVICE,
    .name = TYPE_STM32L152_SOC,
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
