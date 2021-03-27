#pragma once
#include <memory>
#include <Windows.h>

class IWindowsMng
{
public:
	IWindowsMng();
	~IWindowsMng();

	bool GenerateWindow(LPCWSTR classname, LPCTSTR title, long windowWidth, long windowHeight);
	HWND GetWindowHandle(LPCWSTR classname);
	bool Update(void);
private:
	class WindowsMng;
	std::unique_ptr<WindowsMng> wndmng_;
};

