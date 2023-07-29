#pragma once

#include_next <hal_pal_lld.h>

#define pal_lld_get_pad_event(port, pad) &_pal_events[0]; (void)(port); (void)pad
#define pal_lld_get_line_event(line)     &_pal_events[0]; (void)line
#define pal_lld_enablelineevent(line, mode) _pal_lld_enablelineevent(line, mode)
#define pal_lld_disablelineevent(line) _pal_lld_disablelineevent(line)

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE)
typedef uint16_t ioeventmode_t;

extern palevent_t _pal_events[1];

#ifdef __cplusplus
extern "C" {
#endif
void _pal_lld_enablelineevent(ioline_t line, ioeventmode_t mode);
void _pal_lld_disablelineevent(ioline_t line);
#ifdef __cplusplus
}
#endif
#endif
