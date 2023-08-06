#include "hal.h"

const PALConfig pal_default_config = {
#ifdef PS2_DRIVER_INTERRUPT
    .setup[2] = {
        .DIR = (1 << PAL_PAD(C8))
    }
#endif
};

void __early_init(void) {
}

void boardInit(void) {
#ifdef PS2_DRIVER_INTERRUPT
    pal_lld_init_exti();
#endif
}
