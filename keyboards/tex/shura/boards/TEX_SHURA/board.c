#include "hal.h"

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE)
#define ESSR0_MASK(line) (PAL_PAD(line) >= 8 ? UINT32_C(0) : HT32_PAL_IDX(PAL_PORT(line)) << (PAL_PAD(line) * 4))
#define ESSR1_MASK(line) (PAL_PAD(line) < 8 ? UINT32_C(0) : HT32_PAL_IDX(PAL_PORT(line)) << ((PAL_PAD(line) - 8) * 4))

palevent_t _pal_events[1];

OSAL_IRQ_HANDLER(HT32_EXTI2_3_IRQ_VECTOR) {
    OSAL_IRQ_PROLOGUE();
    EXTI->EDGEFLGR = UINT32_C(0x00000008);
    EXTI->EDGESR = UINT32_C(0x00000008);
    _pal_isr_code(0);
    OSAL_IRQ_EPILOGUE();
}

static void ps2_lld_init(void) {
    // enable EXTI
    CKCU->APBCCR0 |= CKCU_APBCCR0_EXTIEN;

    for (unsigned i = 0; i < sizeof(_pal_events) / sizeof(_pal_events[0]); ++i) {
        _pal_init_event(i);
    }

    nvicEnableVector(EXTI2_3_IRQn, 2);
}

void _pal_lld_enablelineevent(ioline_t line, ioeventmode_t mode) {
    uint32_t mask = 0;

    switch (mode & PAL_EVENT_MODE_EDGES_MASK) {
    case PAL_EVENT_MODE_RISING_EDGE:
        mask = UINT32_C(0x30000000);
        break;
    case PAL_EVENT_MODE_FALLING_EDGE:
        mask = UINT32_C(0x20000000);
        break;
    case PAL_EVENT_MODE_BOTH_EDGES:
        mask = UINT32_C(0x40000000);
        break;
    }

    EXTI->CFGR3 = mask;
    EXTI->CR = UINT32_C(0x00000008);
}

void _pal_lld_disablelineevent(ioline_t line) {
    EXTI->CR = UINT32_C(0x00000000);
}

#else
#define ESSR0_MASK(line) UINT32_C(0)
#define ESSR1_MASK(line) UINT32_C(0)
#define ps2_lld_init() do {} while(0)
#endif

const PALConfig pal_default_config = {
    .setup[2] = {
        .DIR = (1 << PAL_PAD(C8))
    },
    .ESSR[0] = ESSR0_MASK(PS2_CLOCK_PIN),
    .ESSR[1] = ESSR1_MASK(PS2_CLOCK_PIN)
};

void __early_init(void) {
}

void boardInit(void) {
    ps2_lld_init();
}
