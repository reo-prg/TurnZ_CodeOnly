#pragma once
#include <Windows.h>

class KeyboardState
{
public:
	KeyboardState();
	~KeyboardState();

	void Update(void);

	bool Pressed(unsigned int button, unsigned int buffer = defaultBuffer_);
	bool Triggered(unsigned int button);
	bool Released(unsigned int button);

	bool AnyPressed(void);
	bool AnyTriggered(void);
private:
	unsigned char state_[2][256];
	unsigned int currentBuffer_;
	static constexpr size_t defaultBuffer_ = UINT_MAX;
};

