#pragma once
#include<d3d12.h>
#include <d3dx12.h>

using namespace Microsoft::WRL;

#define DxFence MDx12Fence::GetInstance().GetFence()
#define DxFenceValue MDx12Fence::GetInstance().GetFenceValue()

class MDx12Fence
{
public:
	static MDx12Fence& GetInstance(void);
	void Initialize(void);

	const ComPtr<ID3D12Fence1>& GetFence(void);
	uint64_t& GetFenceValue(void);
	void WaitSignal(void);
private:
	MDx12Fence();
	~MDx12Fence();
	MDx12Fence(const MDx12Fence&) = delete;
	MDx12Fence operator=(const MDx12Fence&) = delete;

	ComPtr<ID3D12Fence1> fence_;
	uint64_t fenceValue_;
};

