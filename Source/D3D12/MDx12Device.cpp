#include "MDx12Device.h"
#include <cassert>
#include <vector>
#include "MDxgiFactory.h"
#include "../Window/DisplayException.h"

MDx12Device& MDx12Device::GetInstance(void)
{
	static MDx12Device d;
	return d;
}

const ComPtr<ID3D12Device>& MDx12Device::GetDevice(void)
{
	if (device_ == nullptr)
	{ 
		Initialize(); 
	}
	return device_;
}

void MDx12Device::Initialize(void)
{
	HRESULT result;

	ID3D12Device* tester = nullptr;
	const D3D_FEATURE_LEVEL requiredMinLevel = D3D_FEATURE_LEVEL_11_0;
	std::vector<IDXGIAdapter1*> adapters;
	IDXGIAdapter1* adapter = nullptr;
	IDXGIAdapter1* warpAdapter = nullptr;
	DXGI_ADAPTER_DESC1 wds = {};
	bool warpFounded = false;
	std::wstring wan;
	IDXGIFactory7* factory = DxgiFactory.Get();
	unsigned int idx = 0;
	while (factory->EnumAdapters1(idx, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(adapter);
		idx++;
	}
	result = factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
	if (SUCCEEDED(result))
	{
		warpAdapter->GetDesc1(&wds);
		warpFounded = true;
		wan = wds.Description;
	}
	adapter = nullptr;
	size_t maxVideoMemory = 0;
	for (auto& adp : adapters)
	{
		DXGI_ADAPTER_DESC1 ds = {};
		adp->GetDesc1(&ds);
		std::wstring an = ds.Description;
		if (warpFounded && an == wan) { continue; }
		result = D3D12CreateDevice(adp, requiredMinLevel, IID_PPV_ARGS(&tester));
		tester->Release();
		if (SUCCEEDED(result))
		{
			if (ds.DedicatedVideoMemory > maxVideoMemory)
			{
				adapter = adp;
				maxVideoMemory = ds.DedicatedVideoMemory;
			}
		}
	}

	if (adapter == nullptr)
	{
		if (warpFounded)
		{
			result = D3D12CreateDevice(warpAdapter, requiredMinLevel, IID_PPV_ARGS(&tester));
			if (FAILED(result))
			{
				DisplayException::DisplayError(L"Oops!\n Your adapters can not use Feature Level 11_0 :(");
				return;
			}
			adapter = warpAdapter;
		}
		else
		{
			for (auto& adp : adapters)
			{
				adp->Release();
			}
			DisplayException::DisplayError(L"Oops!\n Your adapters can not use Feature Level 11_0 :(");
			return;
		}
	}

	const D3D_FEATURE_LEVEL featureLevel[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	for (auto& level : featureLevel)
	{
		result = D3D12CreateDevice(adapter, level, IID_PPV_ARGS(device_.GetAddressOf()));
		if (SUCCEEDED(result))
		{
			break;
		}
	}

	if (FAILED(result))
	{
		DisplayException::DisplayError(L"Oops!\n Your adapters can not use Feature Level 11_0 :(");
	}
	for (auto& adp : adapters)
	{
		adp->Release();
	}
}

MDx12Device::MDx12Device():device_(nullptr)
{
	HRESULT result;
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug_ = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug_));
	assert(SUCCEEDED(result));
	debug_->EnableDebugLayer();
	debug_->Release();
#endif
}

MDx12Device::~MDx12Device()
{
}
