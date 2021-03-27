#include "Application.h"
#include "Audio/AudioManager.h"
#include "D3D12/MDx12Device.h"
#include "D3D12/MDxgiFactory.h"
#include "D3D12/MDx12CommandList.h"
#include "D3D12/MDx12CommandQueue.h"
#include "D3D12/MDx12DepthStencil.h"
#include "D3D12/MDx12Fence.h"
#include "D3D12/MDxgiSwapChain.h"
#include "Graphic/Drawer.h"
#include "Graphic/MEffekseer.h"
#include "Input/InputIntegrator.h"
#include "Object/2D/Object2DDrawer.h"
#include "Object/3D/Object3DDrawer.h"
#include "Scene/SceneManager.h"
#include "Scene/SettingScene.h"
#include "Window/DisplayException.h"
#include "Window/WindowsMng.h"
#include "TimeKeeper.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DxGuid.lib")

Application& Application::GetInstance(void)
{
	static Application app;
	return app;
}

void Application::GetWindowSize(unsigned int& width, unsigned int& height)
{
	width = MainWindowWidth;
	height = MainWindowHeight;
}

bool Application::Initialize(void)
{
	HRESULT result;
	result = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(result)) { return false; }
	winmng_ = std::make_unique<IWindowsMng>();
	if (!winmng_->GenerateWindow(L"main", L"TurnZ", MainWindowWidth, MainWindowHeight))
	{
		return false;
	}	
	ShowWindow(winmng_->GetWindowHandle(L"main"), SW_MAXIMIZE);
	DisplayException::terminate_ = false;

	InitializeDirectX();
	if (DisplayException::terminate_) { return false; }

	time_.reset(new TimeKeeper());
	AudioIns;
	if (!Drawer::GetInstance().Initialize(MainWindowWidth, MainWindowHeight)) { return false; };
	EfkIns.Initialize(DxDevice, RootCmQueue);
	InputIns;
	scene_ = std::make_unique<SceneManager>();
	SettingScene::LoadAndApplySetting();
	return true;
}

void Application::Execute(void)
{
	while (winmng_->Update())
	{
		// Œo‰ßƒtƒŒ[ƒ€‚ÌŽæ“¾
		int frame = time_->GetDeltaFrame();
		if (frame > 0)
		{
			SetCursor(LoadCursor(nullptr, IDC_ARROW));
			InputIns.Update();
			for (int i = 0; i < frame; i++)
			{
				scene_->Update();
				if (DisplayException::terminate_) { goto Exception; }
			}
			EfkIns.Update(RootCmList, frame);
			DrawerIns.Update();
			DrawerIns.Draw();
			AudioIns.Update();
#ifdef _DEBUG
			time_->DrawFPS();
#endif
		Exception:;
		}
	}
}

void Application::Terminate(void)
{
	MDx12Fence::GetInstance().WaitSignal();

	CoUninitialize();
	UnregisterClass(L"main", GetModuleHandle(0));
}

void Application::InitializeDirectX(void)
{
	MDxgiFactory::GetInstance().Initialize();
	if (DisplayException::terminate_) { return; }
	MDx12Device::GetInstance().Initialize();
	if (DisplayException::terminate_) { return; }
	MDx12CommandList::GetInstance().InitRootCommandList();
	if (DisplayException::terminate_) { return; }
	MDx12CommandQueue::GetInstance().Initialize();
	if (DisplayException::terminate_) { return; }
	MDxgiSwapChain::GetInstance().Initialize(2, MainWindowWidth, MainWindowHeight, winmng_->GetWindowHandle(L"main"));
	if (DisplayException::terminate_) { return; }
	MDx12Fence::GetInstance().Initialize();
	if (DisplayException::terminate_) { return; }
	MDx12DepthStencil::GetInstance().Initialize();
	if (DisplayException::terminate_) { return; }
}
