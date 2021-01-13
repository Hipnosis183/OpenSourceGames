
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "IOTouchDragAnywhere.h"

#include "SDL.h"

IOTouchDragAnywhere::IOTouchDragAnywhere()
	: up(false), down(false),
	  left(false), right(false) {
	memset(&anchor, 0, sizeof(anchor));
}

void IOTouchDragAnywhere::convertInput(SDL_Event* ev) {
	SDL_TouchFingerEvent* tev = &ev->tfinger;
	if (tev->type == SDL_FINGERDOWN && tev->fingerId == 0) {
		anchor.touchId = tev->touchId;
		anchor.fingerId = tev->fingerId;
		anchor.x = tev->x;
		anchor.y = tev->y;
	} else if (tev->type == SDL_FINGERUP) {
		if (tev->touchId == anchor.touchId && tev->fingerId == anchor.fingerId) {
			tev->touchId = 0;
			tev->fingerId = 0;
			
			if (up == true) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYUP;
				ev.key.state = SDL_RELEASED;
				ev.key.keysym.scancode = SDL_SCANCODE_UP;
				SDL_PushEvent(&ev);
			}
			if (down == true) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYUP;
				ev.key.state = SDL_RELEASED;
				ev.key.keysym.scancode = SDL_SCANCODE_DOWN;
				SDL_PushEvent(&ev);
			}
			if (left == true) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYUP;
				ev.key.state = SDL_RELEASED;
				ev.key.keysym.scancode = SDL_SCANCODE_LEFT;
				SDL_PushEvent(&ev);
			}
			if (right == true) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYUP;
				ev.key.state = SDL_RELEASED;
				ev.key.keysym.scancode = SDL_SCANCODE_RIGHT;
				SDL_PushEvent(&ev);
			}
			
			up = down = left = right = false;
		}
	} else if (tev->type == SDL_FINGERMOTION) {
		if (tev->touchId == anchor.touchId && tev->fingerId == anchor.fingerId) {
			bool new_up = false, new_down = false,
				new_left = false, new_right = false;
			float x = tev->x - anchor.x;
			float y = tev->y - anchor.y;
			float treshold = 0.05;

			if (y < -treshold) {
				new_up = true;
			}
			if (y > +treshold) {
				new_down = true;
			}
			if (x < -treshold) {
				new_left = true;
			}
			if (x > +treshold) {
				new_right = true;
			}

			if (up == false && new_up == true) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYDOWN;
				ev.key.state = SDL_PRESSED;
				ev.key.keysym.scancode = SDL_SCANCODE_UP;
				SDL_PushEvent(&ev);
			}
			if (down == false && new_down == true) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYDOWN;
				ev.key.state = SDL_PRESSED;
				ev.key.keysym.scancode = SDL_SCANCODE_DOWN;
				SDL_PushEvent(&ev);
			}
			if (left == false && new_left == true) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYDOWN;
				ev.key.state = SDL_PRESSED;
				ev.key.keysym.scancode = SDL_SCANCODE_LEFT;
				SDL_PushEvent(&ev);
			}
			if (right == false && new_right == true) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYDOWN;
				ev.key.state = SDL_PRESSED;
				ev.key.keysym.scancode = SDL_SCANCODE_RIGHT;
				SDL_PushEvent(&ev);
			}

			if (up == true && new_up == false) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYUP;
				ev.key.state = SDL_RELEASED;
				ev.key.keysym.scancode = SDL_SCANCODE_UP;
				SDL_PushEvent(&ev);
			}
			if (down == true && new_down == false) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYUP;
				ev.key.state = SDL_RELEASED;
				ev.key.keysym.scancode = SDL_SCANCODE_DOWN;
				SDL_PushEvent(&ev);
			}
			if (left == true && new_left == false) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYUP;
				ev.key.state = SDL_RELEASED;
				ev.key.keysym.scancode = SDL_SCANCODE_LEFT;
				SDL_PushEvent(&ev);
			}
			if (right == true && new_right == false) {
				SDL_Event ev; memset(&ev, 0, sizeof(ev));
				ev.type = SDL_KEYUP;
				ev.key.state = SDL_RELEASED;
				ev.key.keysym.scancode = SDL_SCANCODE_RIGHT;
				SDL_PushEvent(&ev);
			}

			up = new_up;
			down = new_down;
			left = new_left;
			right = new_right;
		}
	}
}
