/**
 * Test touchscreen input transformation

 * Copyright (C) 2015  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "IOTouchDragAnywhere.h"
#include "SDL.h"

void pushEvent(IOTouchDragAnywhere *conv, SDL_EventType type, SDL_TouchID tid, SDL_FingerID fid, float x, float y) {
	SDL_Event ev;
	ev.type = type;
	ev.tfinger.touchId = tid;
	ev.tfinger.fingerId = fid;
	ev.tfinger.x = x;
	ev.tfinger.y = y;
	conv->convertInput(&ev);
}

class TestTouch : public CxxTest::TestSuite {
public:
	void setUp() {
	}
	void tearDown() {
	}


	void testTransform() {
		IOTouchDragAnywhere conv;

		// Set center of virtual joystick
		{
			pushEvent(&conv, SDL_FINGERDOWN, 9,0, .2,.7);
			TS_ASSERT_DELTA(conv.anchor.x, .2, 1e-7);
			SDL_Event getev;
			TS_ASSERT_EQUALS(0, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP));
		}

		// First motion above the center triggers KEYUP
		{
			pushEvent(&conv, SDL_FINGERMOTION, 9,0, .2,.5);
			SDL_Event getev;
			TS_ASSERT_EQUALS(1, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYDOWN));
			TS_ASSERT_EQUALS(getev.key.state, SDL_PRESSED);
			TS_ASSERT_EQUALS(getev.key.keysym.scancode, SDL_SCANCODE_UP);
			TS_ASSERT_EQUALS(0, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYUP, SDL_KEYDOWN));
		}
		
		// Next motions don't trigger nothin'
		{
			pushEvent(&conv, SDL_FINGERMOTION, 9,0, .2,.51);
			SDL_Event getev;
			TS_ASSERT_EQUALS(0, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP));
		}

		// Touching a different finger is ignored
		{
			pushEvent(&conv, SDL_FINGERDOWN, 9,1, .2,.5);
			pushEvent(&conv, SDL_FINGERMOTION, 9,1, .2,.7);
			pushEvent(&conv, SDL_FINGERUP, 9,1, .2,.5);
			pushEvent(&conv, SDL_FINGERMOTION, 9,1, .2,.71);
			pushEvent(&conv, SDL_FINGERUP, 9,1, .2,.5);
			SDL_Event getev;
			TS_ASSERT_EQUALS(0, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP));
		}

		// Going back to center triggers KEYUP
		{
			pushEvent(&conv, SDL_FINGERMOTION, 9,0, .2,.7);
			SDL_Event getev;
			TS_ASSERT_EQUALS(1, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYUP, SDL_KEYUP));
			TS_ASSERT_EQUALS(getev.key.state, SDL_RELEASED);
			TS_ASSERT_EQUALS(getev.key.keysym.scancode, SDL_SCANCODE_UP);
			TS_ASSERT_EQUALS(0, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP));
		}

		// Releasing the original finger stops tracking + triggers KEYUP
		{
			pushEvent(&conv, SDL_FINGERMOTION, 9,0, .2,.5);
			SDL_Event getev;
			TS_ASSERT_EQUALS(1, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYDOWN));
			TS_ASSERT_EQUALS(getev.key.state, SDL_PRESSED);
			TS_ASSERT_EQUALS(getev.key.keysym.scancode, SDL_SCANCODE_UP);
			TS_ASSERT_EQUALS(0, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP));
			
			pushEvent(&conv, SDL_FINGERUP, 9,0, .2,.5);
			TS_ASSERT_EQUALS(1, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYUP, SDL_KEYUP));
			TS_ASSERT_EQUALS(getev.key.state, SDL_RELEASED);
			TS_ASSERT_EQUALS(getev.key.keysym.scancode, SDL_SCANCODE_UP);
			TS_ASSERT_EQUALS(0, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP));
			pushEvent(&conv, SDL_FINGERUP, 9,0, .2,.5);
			TS_ASSERT_EQUALS(0, SDL_PeepEvents(&getev, 1, SDL_GETEVENT, SDL_KEYUP, SDL_KEYUP));
		}
	}
};
