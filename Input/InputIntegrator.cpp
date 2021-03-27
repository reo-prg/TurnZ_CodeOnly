#include "InputIntegrator.h"
#include "XInputState.h"
#include "KeyboardState.h"

InputIntegrator& InputIntegrator::GetInstance(void)
{
	static InputIntegrator i;
	return i;
}

InputIntegrator::InputIntegrator()
{
	xinput_.reset(new XInputState());
	key_.reset(new KeyboardState());
	Initialize();
}

InputIntegrator::~InputIntegrator()
{
}

void InputIntegrator::Update(void)
{
	xinput_->Update();
	key_->Update();
}

bool InputIntegrator::Pressed(const std::string& key)
{
	if (keyConfig_.find(key) == keyConfig_.end())
	{
		return false;
	}

	bool rtn = false;
	for (const auto& k : keyConfig_.at(key))
	{
		switch (k.type_)
		{
		case InputType::Keyboard:
			rtn |= key_->Pressed(k.key_);
			break;
		case InputType::XInput:
			rtn |= xinput_->Pressed(k.key_);
			break;
		default:
			break;
		}
	}
	return rtn;
}

bool InputIntegrator::Triggered(const std::string& key)
{
	if (keyConfig_.find(key) == keyConfig_.end())
	{
		return false;
	}

	bool rtn = false;
	for (const auto& k : keyConfig_.at(key))
	{
		switch (k.type_)
		{
		case InputType::Keyboard:
			rtn |= key_->Triggered(k.key_);
			break;
		case InputType::XInput:
			rtn |= xinput_->Triggered(k.key_);
			break;
		default:
			break;
		}
	}
	return rtn;
}

bool InputIntegrator::Released(const std::string& key)
{
	if (keyConfig_.find(key) == keyConfig_.end())
	{
		return false;
	}

	bool rtn = false;
	for (const auto& k : keyConfig_.at(key))
	{
		switch (k.type_)
		{
		case InputType::Keyboard:
			rtn |= key_->Released(k.key_);
			break;
		case InputType::XInput:
			rtn |= xinput_->Released(k.key_);
			break;
		default:
			break;
		}
	}
	return rtn;
}

bool InputIntegrator::AnyPressed(void)
{
	if (key_->AnyPressed()) { return true; }
	//if (xinput_->Pressed(0x3ffffff)) { return true; }
	return false;
}

bool InputIntegrator::AnyTriggered(void)
{
	if (key_->AnyTriggered()) { return true; }
	//if (xinput_->Triggered(0x3ffffff)) { return true; }
	return false;
}

void InputIntegrator::SetKeyboardConfig(unsigned int(&key)[8])
{
	unsigned int tmp = 0;
	for (auto& i : keyConfig_.at("up"))
	{
		if (i.type_ == InputType::Keyboard)
		{
			i.key_ = key[tmp];
			tmp += 4;
		}
	}
	tmp = 1;
	for (auto& i : keyConfig_.at("right"))
	{
		if (i.type_ == InputType::Keyboard)
		{
			i.key_ = key[tmp];
			tmp += 4;
		}
	}
	tmp = 2;
	for (auto& i : keyConfig_.at("down"))
	{
		if (i.type_ == InputType::Keyboard)
		{
			i.key_ = key[tmp];
			tmp += 4;
		}
	}
	tmp = 3;
	for (auto& i : keyConfig_.at("left"))
	{
		if (i.type_ == InputType::Keyboard)
		{
			i.key_ = key[tmp];
			tmp += 4;
		}
	}
}

void InputIntegrator::Initialize(void)
{
	Register("left",	{ { InputType::Keyboard, VK_LEFT },		{ InputType::Keyboard, 0x41 } ,		{ InputType::Keyboard, 0x4a } });
	Register("right",	{ { InputType::Keyboard, VK_RIGHT },	{ InputType::Keyboard, 0x44 } ,		{ InputType::Keyboard, 0x4c } });
	Register("up",		{ { InputType::Keyboard, VK_UP },		{ InputType::Keyboard, 0x57 } ,		{ InputType::Keyboard, 0x49 } });
	Register("down",	{ { InputType::Keyboard, VK_DOWN },		{ InputType::Keyboard, 0x53 } ,		{ InputType::Keyboard, 0x4b } });
	Register("ok",		{ { InputType::Keyboard, VK_SPACE } });
}

void InputIntegrator::Register(const std::string& key, const std::vector<Input>& list)
{
	if (keyConfig_.find(key) == keyConfig_.end())
	{
		keyConfig_.emplace(key, list);
	}
	else
	{
		for (const auto& l : list)
		{
			keyConfig_.at(key).emplace_back(l);
		}
	}
}
