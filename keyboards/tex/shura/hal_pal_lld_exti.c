#include "hal.h"

#define ESSR_MASK(pad, mask) ((mask) << (((uint32_t)pad) * 4))

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE)
palevent_t _pal_events[PAL_IOPORTS_WIDTH];

static void irq_handler(iopadid_t first, iopadid_t last) {
    uint32_t pad_mask = (UINT32_C(1) << last) - (UINT32_C(1) << first);

    chSysLockFromISR();
    uint32_t irq_mask = EXTI->EDGEFLGR & pad_mask;
    EXTI->EDGEFLGR = pad_mask;
    EXTI->EDGESR = pad_mask;
    chSysUnlockFromISR();

    for (; first != last; ++first) {
        if (irq_mask & (UINT32_C(1) << first)) {
            _pal_isr_code(first);
        }
    }
}

OSAL_IRQ_HANDLER(HT32_EXTI0_1_IRQ_VECTOR) {
    OSAL_IRQ_PROLOGUE();
    irq_handler(0, 2);
    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(HT32_EXTI2_3_IRQ_VECTOR) {
    OSAL_IRQ_PROLOGUE();
    irq_handler(2, 4);
    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(HT32_EXTI4_15_IRQ_VECTOR) {
    OSAL_IRQ_PROLOGUE();
    irq_handler(4, 16);
    OSAL_IRQ_EPILOGUE();
}

void _pal_lld_init_exti(void) {
    for (iopadid_t i = 0; i < PAL_IOPORTS_WIDTH; ++i) {
        _pal_init_event(i);
    }

    CKCU->APBCCR0 |= CKCU_APBCCR0_EXTIEN;
    nvicEnableVector(EXTI0_1_IRQn, HT32_EXTI0_1_IRQ_PRIORITY);
    nvicEnableVector(EXTI2_3_IRQn, HT32_EXTI2_3_IRQ_PRIORITY);
    nvicEnableVector(EXTI4_15_IRQn, HT32_EXTI4_15_IRQ_PRIORITY);
}

void _pal_lld_enablelineevent(ioline_t line, ioeventmode_t mode) {
    osalDbgAssert(!(EXTI->CR & (UINT32_C(1) << PAL_PAD(line))), "channel already in use");

    uint32_t mode_mask = 0;
    switch (mode & PAL_EVENT_MODE_EDGES_MASK) {
    case PAL_EVENT_MODE_RISING_EDGE:
        mode_mask = UINT32_C(0x30000000);
        break;
    case PAL_EVENT_MODE_FALLING_EDGE:
        mode_mask = UINT32_C(0x20000000);
        break;
    case PAL_EVENT_MODE_BOTH_EDGES:
        mode_mask = UINT32_C(0x40000000);
        break;
    }

    iopadid_t essr_idx = PAL_PAD(line) / 8;
    iopadid_t essr_pad = PAL_PAD(line) % 8;
    uint32_t essr = AFIO->ESSR[essr_idx];
    essr &= ~ESSR_MASK(essr_pad, 0xf);
    essr |= ESSR_MASK(essr_pad, HT32_PAL_IDX(PAL_PORT(line)));
    AFIO->ESSR[essr_idx] = essr;

    switch (PAL_PAD(line)) {
    case 0: EXTI->CFGR0 = mode_mask; break;
    case 1: EXTI->CFGR1 = mode_mask; break;
    case 2: EXTI->CFGR2 = mode_mask; break;
    case 3: EXTI->CFGR3 = mode_mask; break;
    case 4: EXTI->CFGR4 = mode_mask; break;
    case 5: EXTI->CFGR5 = mode_mask; break;
    case 6: EXTI->CFGR6 = mode_mask; break;
    case 7: EXTI->CFGR7 = mode_mask; break;
    case 8: EXTI->CFGR8 = mode_mask; break;
    case 9: EXTI->CFGR9 = mode_mask; break;
    case 10: EXTI->CFGR10 = mode_mask; break;
    case 11: EXTI->CFGR11 = mode_mask; break;
    case 12: EXTI->CFGR12 = mode_mask; break;
    case 13: EXTI->CFGR13 = mode_mask; break;
    case 14: EXTI->CFGR14 = mode_mask; break;
    case 15: EXTI->CFGR15 = mode_mask; break;
    }

    EXTI->CR |= UINT32_C(1) << PAL_PAD(line);
}

void _pal_lld_disablelineevent(ioline_t line) {
    EXTI->CR &= ~(UINT32_C(1) << PAL_PAD(line));
}
#endif
