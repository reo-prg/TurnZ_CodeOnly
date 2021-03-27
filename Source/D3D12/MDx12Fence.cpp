#include "MDx12Fence.h"
#include "MDx12Device.h"
#include "MDx12CommandQueue.h"
#include "../Window/DisplayException.h"

MDx12Fence& MDx12Fence::GetInstance(void)
{
	static MDx12Fence f;
	return f;
}

void MDx12Fence::Initialize(void)
{
	HRESULT result;
		result = DxDevice->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
		if (FAILED(result)) { DisplayException::DisplayError(L"Create Fence is failed :("); return; }
}

const ComPtr<ID3D12Fence1>& MDx12Fence::GetFence(void)
{
	if (fence_ == nullptr)
	{
		Initialize();
	}
	return fence_;
}

uint64_t& MDx12Fence::GetFenceValue(void)
{
	if (fence_ == nullptr)
	{
		Initialize();
	}
	return fenceValue_;
}

void MDx12Fence::WaitSignal(void)
{
	ID3D12CommandQueue* cQueue = RootCmQueue.Get();
		cQueue->Signal(fence_.Get(), ++fenceValue_);
	while (fence_->GetCompletedValue() != fenceValue_) {};
}

MDx12Fence::MDx12Fence():fence_(nullptr), fenceValue_(0)
{
}

MDx12Fence::~MDx12Fence()
{
}
