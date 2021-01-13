#include "dinkc_console_renderer.h"
#include "dinkc_console.h"
#include "gfx_fonts.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

void dinkc_console_renderer_render() {
	if (console_active == 1) {
		char* line = dinkc_console_get_cur_line();
		FONTS_SetTextColor(0, 0, 0);
		print_text_wrap_debug(line, 20, 380);
		
		char retval[20+1];
		sprintf(retval, "%d", dinkc_console_get_last_retval());
		FONTS_SetTextColor(255, 0, 0);
		print_text_wrap_debug(retval, 20, 360);
	}
}
