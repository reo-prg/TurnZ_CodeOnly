#pragma once
#include <Windows.h>
#include <chrono>

using namespace std::chrono;

class TimeKeeper
{
public:
	TimeKeeper();
	~TimeKeeper();
		// �x�[�X����̌o�ߎ��Ԃ��擾
		double GetTime(void);

		// �O�񂱂̊֐����Ă΂�Ă���̌o�ߎ���
		// 1�t���[����1�񂾂��Ă�ł�
		int GetDeltaFrame(void);
		
		// ���̃t���[���܂ő҂�
		void Wait(void);

		void SetBase(void);

		void DrawFPS(void);
	   
		// �t���[���̃J�E���^
		int frameCount_;

		static constexpr int frameMax_ = 60;	// �ő�t���[����
		static constexpr double loopDuration_ = 1.0 / static_cast<double>(frameMax_);
private:
	static TimeKeeper* sInstance;

	// ������\�p�t�H�[�}���X�J�E���^���g���邩
	bool canUsePc_;

	// �g����Ƃ�
	LARGE_INTEGER frequency_;				// ���g��
	LARGE_INTEGER baseCounter_;			// �N�����̃J�E���^
	LARGE_INTEGER deltaCounter_;
	LARGE_INTEGER fpsBaseCounter_;

	// �g���Ȃ��Ƃ�
	steady_clock::time_point tpBootTime_;	// �N�����̎���
	steady_clock::time_point tpDeltaTime_;
	steady_clock::time_point fpsBaseTime_;

	unsigned int fpsFrameCount_;

	double GetFpsTime(void);
	void ResetFpsBase(void);

	bool TimeKeeperInit(void);
};

