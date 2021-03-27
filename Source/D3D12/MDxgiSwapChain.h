#pragma once
#include <d3dx12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

#define DxgiSwapChain MDxgiSwapChain::GetInstance().GetSwapChain()

class MDxgiSwapChain
{
public:
	static MDxgiSwapChain& GetInstance(void);
	void Initialize(unsigned int bufcnt, unsigned int width, unsigned int height, HWND hwnd);

	const ComPtr<IDXGISwapChain4>& GetSwapChain(void);
private:
	MDxgiSwapChain();
	~MDxgiSwapChain();
	MDxgiSwapChain(const MDxgiSwapChain&) = delete;
	MDxgiSwapChain operator=(const MDxgiSwapChain&) = delete;

	ComPtr<IDXGISwapChain4> swapChain_;
};

