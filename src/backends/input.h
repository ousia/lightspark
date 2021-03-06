/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009-2013  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef BACKENDS_INPUT_H
#define BACKENDS_INPUT_H 1

#include "compat.h"
#include "backends/geometry.h"
#include "threading.h"
#include "platforms/engineutils.h"
#include "swftypes.h"
#include "smartrefs.h"
#include "scripting/flash/ui/keycodes.h"
#include <vector>

namespace lightspark
{

class SystemState;
class DisplayObject;
class InteractiveObject;
class Sprite;
class MouseEvent;

class InputThread
{
private:
	SystemState* m_sys;
	EngineData* engineData;
	Thread* t;
	bool terminated;
	bool threaded;
	// this is called from mainloopthread
	bool worker(SDL_Event *event);

	std::vector<InteractiveObject* > listeners;
	Mutex mutexListeners;
	Mutex mutexDragged;

	_NR<Sprite> curDragged;
	_NR<InteractiveObject> currentMouseOver;
	_NR<InteractiveObject> lastMouseDownTarget;
	SDL_Keymod lastKeymod;
	SDL_Keycode lastKeyDown;
	SDL_Keycode lastKeyUp;
	const RECT* dragLimit;
	Vector2f dragOffset;
	class MaskData
	{
	public:
		DisplayObject* d;
		MATRIX m;
		MaskData(DisplayObject* _d, const MATRIX& _m):d(_d),m(_m){}
	};
	_NR<InteractiveObject> getMouseTarget(uint32_t x, uint32_t y, DisplayObject::HIT_TYPE type);
	void handleMouseDown(uint32_t x, uint32_t y, SDL_Keymod buttonState,bool pressed);
	void handleMouseDoubleClick(uint32_t x, uint32_t y, SDL_Keymod buttonState,bool pressed);
	void handleMouseUp(uint32_t x, uint32_t y, SDL_Keymod buttonState,bool pressed);
	void handleMouseMove(uint32_t x, uint32_t y, SDL_Keymod buttonState,bool pressed);
	void handleScrollEvent(uint32_t x, uint32_t y, uint32_t direction, SDL_Keymod buttonState,bool pressed);
	void handleMouseLeave();

	bool handleKeyboardShortcuts(const SDL_KeyboardEvent *keyevent);
	void sendKeyEvent(const SDL_KeyboardEvent *keyevent);

	Spinlock inputDataSpinlock;
	Vector2 mousePos;
public:
	InputThread(SystemState* s);
	~InputThread();
	void wait();
	void start(EngineData* data);
	void addListener(InteractiveObject* ob);
	void removeListener(InteractiveObject* ob);
	void startDrag(_R<Sprite> s, const RECT* limit, Vector2f dragOffset);
	void stopDrag(Sprite* s);

	Vector2 getMousePos()
	{
		SpinlockLocker locker(inputDataSpinlock);
		return mousePos;
	}
	bool handleEvent(SDL_Event *event)
	{
		return worker(event);
	}
	AS3KeyCode getLastKeyDown();
	AS3KeyCode getLastKeyUp();
	SDL_Keycode getLastKeyCode();
	SDL_Keymod getLastKeyMod();
};

}
#endif /* BACKENDS_INPUT_H */
