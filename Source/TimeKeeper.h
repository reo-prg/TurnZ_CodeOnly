#pragma once
#include <Windows.h>
#include <chrono>

using namespace std::chrono;

class TimeKeeper
{
public:
	TimeKeeper();
	~TimeKeeper();
		// ベースからの経過時間を取得
		double GetTime(void);

		// 前回この関数が呼ばれてからの経過時間
		// 1フレームに1回だけ呼んでね
		int GetDeltaFrame(void);
		
		// 次のフレームまで待つ
		void Wait(void);

		void SetBase(void);

		void DrawFPS(void);
	   
		// フレームのカウンタ
		int frameCount_;

		static constexpr int frameMax_ = 60;	// 最大フレーム数
		static constexpr double loopDuration_ = 1.0 / static_cast<double>(frameMax_);
private:
	static TimeKeeper* sInstance;

	// 高分解能パフォーマンスカウンタが使えるか
	bool canUsePc_;

	// 使えるとき
	LARGE_INTEGER frequency_;				// 周波数
	LARGE_INTEGER baseCounter_;			// 起動時のカウンタ
	LARGE_INTEGER deltaCounter_;
	LARGE_INTEGER fpsBaseCounter_;

	// 使えないとき
	steady_clock::time_point tpBootTime_;	// 起動時の時間
	steady_clock::time_point tpDeltaTime_;
	steady_clock::time_point fpsBaseTime_;

	unsigned int fpsFrameCount_;

	double GetFpsTime(void);
	void ResetFpsBase(void);

	bool TimeKeeperInit(void);
};

