#include "MDxgiSwapChain.h"
#include "MDxgiFactory.h"
#include "MDx12CommandQueue.h"
#include "../Window/DisplayException.h"

MDxgiSwapChain& MDxgiSwapChain::GetInstance(void)
{
	static MDxgiSwapChain s;
	return s;
}

void MDxgiSwapChain::Initialize(unsigned int bufcnt, unsigned int width, unsigned int height, HWND hwnd)
{
	if (swapChain_ != nullptr)
	{
		OutputDebugString(L"Create SwapChain multiply\n");
		return;
	}
	HRESULT result;

	DXGI_SWAP_CHAIN_DESC1 scd = {};
	scd.BufferCount = bufcnt;
	scd.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scd.Width = width;
	scd.Height = height;
	scd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scd.Flags = 0;
	scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Scaling = DXGI_SCALING_STRETCH;
	scd.Stereo = false;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ComPtr<IDXGISwapChain1> sc = nullptr;
	result = DxgiFactory->CreateSwapChainForHwnd(RootCmQueue.Get(), hwnd, &scd, nullptr, nullptr, sc.GetAddressOf());
	if (FAILED(result)) { DisplayException::DisplayError(L"Create SwapChain is failed :("); return; }
	result = sc.As(&swapChain_);
	if (FAILED(result)) { DisplayException::DisplayError(L"Move SwapChain is failed :("); return; }
}

const ComPtr<IDXGISwapChain4>& MDxgiSwapChain::GetSwapChain(void)
{
	if (swapChain_ == nullptr)
	{
		OutputDebugString(L"SwapChain is not initialized");
	}
	return swapChain_;
}

MDxgiSwapChain::MDxgiSwapChain():swapChain_(nullptr)
{
}

MDxgiSwapChain::~MDxgiSwapChain()
{
}
