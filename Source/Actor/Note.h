#pragma once
#include <DirectXMath.h>
#include <string>
#include <memory>
#include "State.h"

class Object2DContainer;
class Note
{
public:
	Note(const DirectX::XMFLOAT2& size);
	~Note();

	void Update(void);
	void SetCoordinate(float x, float y);
	void GetCoordinate(float& x, float& y);
	void SetDir(unsigned char dir);
	Direction GetDir(void)const;
	void SetDuration(unsigned int appear, unsigned int disappaer);
	void SetJudgeFrame(unsigned short frame);
	const unsigned short GetJudgeFrame(void);

	static void SetType(unsigned char type);
	static unsigned char GetType(void);

	void Start(void);
	void Kill(void); 
	bool IsDead(void);
	bool IsActive(void);
	void Hit(void);

	static constexpr unsigned int typeCount_ = 2; 
private:
	void Appear(void);
	void Idle(void);
	void DisAppear(void);
	void (Note::*updater_)(void);

	DirectX::XMFLOAT2 coord_;
	DirectX::XMFLOAT2 size_;
	Direction dir_;

	unsigned short judgeframe_;

	unsigned int appearDuration_;
	unsigned int disappearDuration_;
	unsigned int counter_;

	bool dead_;
	bool active_;

	static unsigned char noteType_;

	std::unique_ptr<Object2DContainer> obj_;
};

namespace
{
	const std::string notePath[Note::typeCount_] =
	{
		"note_origin.png",
		"note.png"
	};
}
