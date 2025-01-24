#include "qemu/osdep.h"
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


// No new virtual functions: we can reuse the typedef for the
// superclass.
OBJECT_DECLARE_TYPE(STM32L152State, STM32L152Class, STM32L152_SOC)

static void stm32l152_soc_init(Object *stm32l152_obj) {
    STM32L152State* sc = STM32L152_SOC(stm32l152_obj);
    Error* errp = NULL;

    Object* armv7_object = OBJECT(&sc->armv7_cpu);
    object_initialize_child(stm32l152_obj, "armv7m", &sc->armv7_cpu, TYPE_ARMV7M);

    // Initiailize ARMv7 CPU state
    if (!object_property_set_link(armv7_object, "memory", OBJECT(get_system_memory()), &errp)) {
        verbose_report_err(errp);
        abort();
    }

    if (!object_property_set_str(armv7_object, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m3"), &errp)) {
        verbose_report_err(errp);
        abort();
    }

    Clock* sysclk = clock_new(stm32l152_obj, "SYSCLK");
    clock_set_hz(sysclk, STM32L152_CPU_CLOCK_HZ);
    qdev_init_clock_out(DEVICE(stm32l152_obj), "sysclk");

    // Initiailize SOC state
    sc->sysclk = sysclk; 
}

static void stm32l152_soc_realize(DeviceState *dev, Error **errp) {
    /* TODO: Realize and connect everything here */
    STM32L152State* sc = STM32L152_SOC(dev);

    /* No need to call the sysbus realize since it is empty */

    qdev_connect_clock_in(DEVICE(&sc->armv7_cpu), "cpuclk", sc->sysclk);

    
    if (!qdev_realize(DEVICE(&sc->armv7_cpu), sysbus_get_default(), errp)) {
        /* Assume error is handled and printed at the machine */
        return;
    }
}

static void stm32l152_soc_class_init(ObjectClass *klass, void *data) {
    // klass->interfaces
    STM32L152Class* my_class = STM32L152_SOC_CLASS(klass);
    my_class->parent_class.parent_class.realize = &stm32l152_soc_realize;
}

static const TypeInfo stm32l152_soc_info = {
    .parent = TYPE_SYS_BUS_DEVICE,
    .name = TYPE_STM32L152_SOC,
    .instance_size = sizeof(STM32L152State),
    .instance_init = stm32l152_soc_init,
    .class_init = stm32l152_soc_class_init,
    .class_size = sizeof(STM32L152Class), /* Default class size*/
    // TODO: Implement class_init and implement the realize function inside of it
};

static void
stm32l152_soc_register_types(void)
{
    type_register_static(&stm32l152_soc_info);
}
type_init(stm32l152_soc_register_types);
