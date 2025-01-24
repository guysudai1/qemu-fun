#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "qemu/qemu-print.h"
#include "qemu/typedefs.h"
#include "qom/object.h"
#include "hw/boards.h"
#include "target/arm/cpu-qom.h"
#include "qemu/units.h"

#include "hw/arm/stm32l152_soc.h"
#include "hw/qdev-clock.h"
#include "hw/qdev-core.h"
#include "hw/arm/boot.h"
#include "hw/core/cpu.h"


#define TYPE_STM32L152 "stm32l152"

static void stm32l152_machine_init(MachineState *machine)
{
    machine->ram_size = machine->maxram_size = 32 * KiB;
    machine->cpu_type = TYPE_STM32L152_SOC;
    Error* errp = NULL;

    /* This clock doesn't need migration because it is fixed-frequency */
    DeviceState* cpu_dev = qdev_new(TYPE_STM32L152_SOC);
    object_property_add_child(OBJECT(machine), "soc", OBJECT(cpu_dev));

    if (!qdev_realize_and_unref(cpu_dev, sysbus_get_default(), &errp)) {
        verbose_report_err(errp);
    }
    
    armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename,
                       STM32L152_FLASH_BASE, STM32L152_FLASH_SIZE);
}


static void stm32l152_machine_class_init(MachineClass *mc)
{
    static const char * const valid_cpu_types[] = {
        ARM_CPU_TYPE_NAME("cortex-m3"),
        NULL
    };

    mc->desc = "STM32L152 (Cortex-M3)";
    mc->init = stm32l152_machine_init;
    mc->default_cpu_type = valid_cpu_types[0];
    mc->valid_cpu_types = valid_cpu_types;
    mc->max_cpus = 1;
    mc->min_cpus = 1;
    mc->default_ram_size = 32 * KiB;
}

DEFINE_MACHINE(TYPE_STM32L152, stm32l152_machine_class_init)
