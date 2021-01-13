#ifndef TOUCH_H
#define TOUCH_H

#include "SDL.h"

class IOTouchDragAnywhere {
public:
	SDL_TouchFingerEvent anchor;
	bool up, down, left, right;

	IOTouchDragAnywhere();
	void convertInput(SDL_Event* ev);
};

#endif
