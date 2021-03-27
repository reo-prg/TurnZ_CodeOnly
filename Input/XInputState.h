#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <memory>
#pragma comment(lib, "Xinput.lib")

// ƒ{ƒ^ƒ“‚ÌŽí—Þ‚ðˆê“x‚É‹æ•Ê‚·‚é‚½‚ß‚Ì‚à‚Ì
constexpr unsigned int XINPUT_GAMEPAD_LEFT_TRIGGER			= 0x10000;
constexpr unsigned int XINPUT_GAMEPAD_RIGHT_TRIGGER		= 0x20000;
constexpr unsigned int XINPUT_GAMEPAD_LEFT_THUMB_UP		= 0x40000;
constexpr unsigned int XINPUT_GAMEPAD_LEFT_THUMB_DOWN		= 0x80000;
constexpr unsigned int XINPUT_GAMEPAD_LEFT_THUMB_LEFT		= 0x100000;
constexpr unsigned int XINPUT_GAMEPAD_LEFT_THUMB_RIGHT		= 0x200000;
constexpr unsigned int XINPUT_GAMEPAD_RIGHT_THUMB_UP		= 0x400000;
constexpr unsigned int XINPUT_GAMEPAD_RIGHT_THUMB_DOWN		= 0x800000;
constexpr unsigned int XINPUT_GAMEPAD_RIGHT_THUMB_LEFT		= 0x1000000;
constexpr unsigned int XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT	= 0x2000000;

struct Matrix3;

class XInputState
{
public:
	XInputState();
	~XInputState();

	void Update(void);

	bool Pressed(unsigned int button, unsigned int buffer = defaultBuffer_);
	bool Triggered(unsigned int button);
	bool Released(unsigned int button);
private:
	XINPUT_STATE state_[2];
	unsigned int currentBuffer_;
	static constexpr unsigned int defaultBuffer_ = UINT_MAX;

	bool TriggerPressed(unsigned int button, unsigned int buffer);
	static constexpr unsigned char TRIGGER_DEADZONE = 230;
	bool TriggerTransformToBoolean(unsigned char value);

	static constexpr float THUMB_ROTATION_ANGLE = -45.0f;
	std::shared_ptr<Matrix3> rotation_;
	bool ThumbTilted(unsigned int button, unsigned int buffer);
	unsigned int ThumbDirection(float x, float y);
};

