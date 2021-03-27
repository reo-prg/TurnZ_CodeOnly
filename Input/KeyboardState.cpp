#include "KeyboardState.h"
#include <Windows.h>
#include <cassert>

KeyboardState::KeyboardState()
{
	currentBuffer_ = 0;
	for (int j = 0; j < 256; j++)
	{
		state_[0][j] = 0;
	}
	for (int j = 0; j < 256; j++)
	{
		state_[1][j] = 0x80;
	}
}

KeyboardState::~KeyboardState()
{
}

void KeyboardState::Update(void)
{
	currentBuffer_ = 1 - currentBuffer_;
	if (!GetKeyboardState(state_[currentBuffer_]))
	{
		OutputDebugString(L"Error on Get KeyState");
	}
}

bool KeyboardState::Pressed(unsigned int button, unsigned int buffer)
{
	bool rtn = false;

	if ((buffer != defaultBuffer_ && buffer >= _countof(state_)) || button >= 256)
	{
		return false;
	}
	size_t buf = buffer == defaultBuffer_ ? currentBuffer_ : buffer;

	rtn = state_[buf][button] & 0x80;

	return rtn;
}

bool KeyboardState::Triggered(unsigned int button)
{
	if (button >= 256)
	{
		return false;
	}
	return (Pressed(button) && (!Pressed(button, 1 - currentBuffer_)));
}

bool KeyboardState::Released(unsigned int button)
{
	if (button >= 256)
	{
		return false;
	}
	return ((!Pressed(button)) && Pressed(button, 1 - currentBuffer_));
}

bool KeyboardState::AnyPressed(void)
{
	for (int i = 0; i < 0xa6; i++)
	{
		if(Pressed(i)){ return true; }
	}
	return false;
}

bool KeyboardState::AnyTriggered(void)
{
	for (int i = 0; i < 0xa6; i++)
	{
		if (Triggered(i)) { return true; }
	}
	return false;
}
