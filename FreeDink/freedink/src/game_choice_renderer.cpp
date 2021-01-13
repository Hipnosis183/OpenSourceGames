#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "game_choice_renderer.h"

#include "game_choice.h"
#include "SDL.h"
#include "dinkini.h"
#include "gfx.h"
#include "gfx_fonts.h"
#include "gfx_sprites.h"
#include "log.h"
#include "game_engine.h"

void game_choice_renderer_render() {
	if (!game_choice.active)
		return;

	int sx = 184;
	int x_depth = 335;

	int px = 48, py = 44;
	int curxl = 126;
	int curxr = 462;
	int curyr = 200;
	int curyl = 200;
	int y_last = 0;
	
	check_seq_status(30);

	// Draw background
	{
		SDL_Rect dst;
		dst.x = px; dst.y = py;
		IOGFX_backbuffer->blit(GFX_k[seq[30].frame[2]].k, NULL, &dst);
	}
	{
		SDL_Rect dst;
		dst.x = px + 169; dst.y = py + 42;
		IOGFX_backbuffer->blit(GFX_k[seq[30].frame[3]].k, NULL, &dst);
	}
	{
		SDL_Rect dst;
		dst.x = px+169+180; dst.y = py+1;
		if (IOGFX_backbuffer->blit(GFX_k[seq[30].frame[4]].k, NULL, &dst) < 0)
			log_error("Could not draw sprite %d: %s", seq[30].frame[4], SDL_GetError());
	}

	// Draw dialog title, if any
	if (strlen(game_choice.buffer) > 0)
		{
			rect rcRect;
			rect_set(&rcRect, sx, 94, 463, 400);
			/* if using an explicit "set_y" after "choice_start()": */
			if (game_choice.newy != -5000)
				rcRect.bottom = game_choice.newy + 15;

			/* Same of in text_draw, except for #1 and default */
			// support for custom colors
			SDL_Color fg;
			if (game_choice.color >= 1 && game_choice.color <= 15) {
				fg = font_colors[game_choice.color];
			} else {
				if (dversion >= 108)
					fg = {255,255,255};
				else
					fg = {255,255,2};
			}

			std::vector<TextCommand> cmds;

			SDL_Color bg = {8,14,21};
			FONTS_SetTextColor(bg.r, bg.g, bg.b);
			print_text_wrap(game_choice.buffer, &rcRect, 1, 0, FONT_DIALOG);

			FONTS_SetTextColor(fg.r, fg.g, fg.b);
			rect_offset(&rcRect, 1, 1);
			print_text_wrap(game_choice.buffer, &rcRect, 1, 0, FONT_DIALOG);
		}

	// Draw choices
	for (int i = game_choice.cur_view; i <= game_choice.cur_view_end; i++)
		{
			//lets figure out where to draw this line
			rect rcRect;
			rect_set(&rcRect, sx, game_choice.choices_y, 463, x_depth + 100);

			SDL_Color fg;
			if (i == game_choice.cur)
				fg = {255,255,255};
			else
				fg = {255,255,2};

			std::vector<TextCommand> cmds;

			SDL_Color bg = {8,14,21};
			FONTS_SetTextColor(bg.r, bg.g, bg.b);
			print_text_wrap(game_choice.line[i], &rcRect, 1, 0, FONT_DIALOG);
			rect_offset(&rcRect, -2, -2);
			print_text_wrap(game_choice.line[i], &rcRect, 1, 0, FONT_DIALOG);

			FONTS_SetTextColor(fg.r, fg.g, fg.b);
			rect_offset(&rcRect, 1, 1);
			y_last = print_text_wrap(game_choice.line[i], &rcRect, 1, 0, FONT_DIALOG);

			if (i == game_choice.cur) {
				curyl = game_choice.choices_y-4;
				curyr = game_choice.choices_y-4;
			}
			game_choice.choices_y += y_last;
		}

	// Draw arrows
	{
		SDL_Rect dst;
		dst.x = curxl; dst.y = curyl;
		IOGFX_backbuffer->blit(GFX_k[seq[456].frame[game_choice.curf]].k, NULL, &dst);
		dst.x = curxr; dst.y = curyr;
		IOGFX_backbuffer->blit(GFX_k[seq[457].frame[game_choice.curf]].k, NULL, &dst);
	}
}
