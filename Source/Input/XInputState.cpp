#include "XInputState.h"
#include <cassert>
#include <cmath>
#include "BitUtility.h"
#include "../Geometry/MathUtility.h"
#include "../Geometry/Matrix.h"

bool XInputState::TriggerPressed(unsigned int button, unsigned int buffer)
{
	bool rtn = false;
	
	if (button & XINPUT_GAMEPAD_LEFT_TRIGGER)
	{
		if (TriggerTransformToBoolean(state_[buffer].Gamepad.bLeftTrigger))
		{
			rtn = true;
		}
	}
	if (button & XINPUT_GAMEPAD_RIGHT_TRIGGER)
	{
		if (TriggerTransformToBoolean(state_[buffer].Gamepad.bRightTrigger))
		{
			rtn = true;
		}
	}

	return rtn;
}

bool XInputState::TriggerTransformToBoolean(unsigned char value)
{
	return value >= TRIGGER_DEADZONE;
}

bool XInputState::ThumbTilted(unsigned int button, unsigned int buffer)
{
	bool rtn = false;

	if (button & (XINPUT_GAMEPAD_LEFT_THUMB_UP | XINPUT_GAMEPAD_LEFT_THUMB_DOWN | 
		XINPUT_GAMEPAD_LEFT_THUMB_LEFT | XINPUT_GAMEPAD_LEFT_THUMB_RIGHT))
	{
		short tx = state_[buffer].Gamepad.sThumbLX;
		short ty = state_[buffer].Gamepad.sThumbLY;
		if (sqrt(tx * tx + ty * ty) >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			Matrix3 tv = TranslationMatrix(tx, ty) * *rotation_;
			tx = static_cast<short>(tv.matrix_[2]);
			ty = static_cast<short>(tv.matrix_[5]);
			
			unsigned int m = XINPUT_GAMEPAD_LEFT_THUMB_UP << ThumbDirection(tx, ty);
			rtn = rtn || (button & m);
		}
	}
	if (button & (XINPUT_GAMEPAD_RIGHT_THUMB_UP | XINPUT_GAMEPAD_RIGHT_THUMB_DOWN |
		XINPUT_GAMEPAD_RIGHT_THUMB_LEFT | XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT))
	{
		short tx = state_[buffer].Gamepad.sThumbRX;
		short ty = state_[buffer].Gamepad.sThumbRY;
		if (sqrt(tx * tx + ty * ty) >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			Matrix3 tv = TranslationMatrix(tx, ty) * *rotation_;
			tx = static_cast<short>(tv.matrix_[2]);
			ty = static_cast<short>(tv.matrix_[5]);

			unsigned int m = XINPUT_GAMEPAD_RIGHT_THUMB_UP << ThumbDirection(tx, ty);
			rtn = rtn || (button & m);
		}
	}

	return rtn;
}

unsigned int XInputState::ThumbDirection(float x, float y)
{
	if (IsPositive(x)	&& IsPositive(y))	{ return 0; }		// ã
	else if (!IsPositive(x)	&& IsPositive(y))	{ return 1; }	// ¶
	else if (!IsPositive(x)	&& !IsPositive(y))	{ return 2; }	// ‰º
	else { return 3; }											// ‰E
}

XInputState::XInputState()
{
	RtlZeroMemory(state_, sizeof(XINPUT_STATE) * 2);
	currentBuffer_ = 0;

	rotation_ = std::make_shared<Matrix3>(RotationMatrix(THUMB_ROTATION_ANGLE));
}

XInputState::~XInputState()
{
	
}

void XInputState::Update(void)
{
	currentBuffer_ = 1 - currentBuffer_;
	RtlZeroMemory(&state_[currentBuffer_], sizeof(XINPUT_STATE));
	DWORD result;
	result = XInputGetState(0, &state_[currentBuffer_]);
	if (result != ERROR_SUCCESS)
	{
		RtlZeroMemory(&state_[currentBuffer_], sizeof(XINPUT_STATE));
		assert(result == ERROR_DEVICE_NOT_CONNECTED);
	}
}

bool XInputState::Pressed(unsigned int button, unsigned int buffer)
{
	bool rtn = false;

	if (buffer != defaultBuffer_ && buffer >= _countof(state_))
	{
		assert(false);
		return false;
	}
	unsigned int buf = buffer == defaultBuffer_ ? currentBuffer_ : buffer;

	unsigned short mask = button & 0xffff;

	while (mask != 0)
	{
		unsigned short b = mask & Integral2Complement(mask);
		rtn = rtn || (state_[buf].Gamepad.wButtons & b);
		mask -= b;
	}

	rtn = rtn || TriggerPressed(button, buf);
	rtn = rtn || ThumbTilted(button, buf);

	return rtn;
}

bool XInputState::Triggered(unsigned int button)
{
	bool rtn = false;

	while (button != 0)
	{
		unsigned short b = button & Integral2Complement(button);
		if (Pressed(b, currentBuffer_), !Pressed(b, 1 - currentBuffer_))
		{
			rtn = true;
			break;
		}
		button -= b;
	}

	return rtn;
}

bool XInputState::Released(unsigned int button)
{
	bool rtn = false;

	while (button != 0)
	{
		unsigned short b = button & Integral2Complement(button);
		if (!Pressed(b, currentBuffer_), Pressed(b, 1 - currentBuffer_))
		{
			rtn = true;
			break;
		}
	}

	return rtn;
}
