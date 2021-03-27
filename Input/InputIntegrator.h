#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#define InputIns InputIntegrator::GetInstance()

enum class InputType
{
	XInput,
	Keyboard,
};

struct Input
{
	InputType type_;
	unsigned int key_;
};

class XInputState;
class KeyboardState;
class InputIntegrator
{
public:
	static InputIntegrator& GetInstance(void);

	void Update(void);

	bool Pressed(const std::string& key);
	bool Triggered(const std::string& key);
	bool Released(const std::string& key);

	bool AnyPressed(void);
	bool AnyTriggered(void);

	void SetKeyboardConfig(unsigned int(&key)[8]);
private:
	InputIntegrator();
	InputIntegrator(const InputIntegrator&) = delete;
	InputIntegrator operator=(const InputIntegrator&) = delete;
	~InputIntegrator();

	void Initialize(void);
	void Register(const std::string& key, const std::vector<Input>& list);

	std::unique_ptr<XInputState> xinput_;
	std::unique_ptr<KeyboardState> key_;

	std::unordered_map<std::string, std::vector<Input>> keyConfig_;
};

