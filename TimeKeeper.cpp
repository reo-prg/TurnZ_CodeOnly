#include "TimeKeeper.h"
#include <string>

TimeKeeper* TimeKeeper::sInstance = nullptr;

double TimeKeeper::GetTime(void)
{
	if (canUsePc_)
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);

		return static_cast<double>(now.QuadPart - baseCounter_.QuadPart) / static_cast<double>(frequency_.QuadPart);
	}
	duration<double> dur = duration_cast<milliseconds>(steady_clock::now() - tpBootTime_);

	return dur.count();
}

int TimeKeeper::GetDeltaFrame(void)
{
	int deltatime = static_cast<int>(GetTime() / loopDuration_);
	int delta = deltatime - frameCount_;
	frameCount_ = deltatime;
	return delta;
}

void TimeKeeper::Wait(void)
{

}

void TimeKeeper::SetBase(void)
{
	frameCount_ = -1;
	if (canUsePc_)
	{
		QueryPerformanceCounter(&baseCounter_);
	}
	else
	{
		tpBootTime_ = steady_clock::now();
	}
}

double TimeKeeper::GetFpsTime(void)
{
	if (canUsePc_)
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);

		return static_cast<double>(now.QuadPart - fpsBaseCounter_.QuadPart) / static_cast<double>(frequency_.QuadPart);
	}
	duration<double> dur = duration_cast<milliseconds>(steady_clock::now() - fpsBaseTime_);

	return dur.count();
}

void TimeKeeper::ResetFpsBase(void)
{
	fpsFrameCount_ = 0;
	if (canUsePc_)
	{
		QueryPerformanceCounter(&fpsBaseCounter_);
	}
	else
	{
		fpsBaseTime_ = steady_clock::now();
	}
}

void TimeKeeper::DrawFPS(void)
{
	static int count = 0;
	static int fps = 0;
	static double time = 0.0;

	if (GetFpsTime() > 1.0)
	{
		fps = count;
		count = 0;
		ResetFpsBase();

		OutputDebugString((TCHAR*)L"fps : ");
		OutputDebugString((LPCWSTR)(std::to_wstring(fps).c_str()));
		OutputDebugString((TCHAR*)L"\n");
	}
	count++;
}

TimeKeeper::TimeKeeper()
{
	canUsePc_ = TimeKeeperInit();
}

TimeKeeper::~TimeKeeper()
{

}

bool TimeKeeper::TimeKeeperInit(void)
{
	frameCount_ = -1;
	fpsFrameCount_ = 0;

	if (!QueryPerformanceFrequency(&frequency_))
	{
		tpBootTime_ = steady_clock::now();
		tpDeltaTime_ = 
		fpsBaseTime_ = steady_clock::now();
		return false;
	}

	QueryPerformanceCounter(&baseCounter_);
	QueryPerformanceCounter(&deltaCounter_);
	QueryPerformanceCounter(&fpsBaseCounter_);

	return true;
}
