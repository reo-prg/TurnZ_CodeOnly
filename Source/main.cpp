#include <Windows.h>
#include "Application.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	auto& app = Application::GetInstance();
	if (!app.Initialize())
	{
		return -1;
	}
	app.Execute();
	app.Terminate();
	return 0;
}