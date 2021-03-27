#pragma once
#include <d3d12.h>
#include <d3dx12.h>

using namespace Microsoft::WRL;

#define RootCmQueue MDx12CommandQueue::GetInstance().GetCommandQueue()

class MDx12CommandQueue
{
public:
	static MDx12CommandQueue& GetInstance(void);
	void Initialize(void);

	const ComPtr<ID3D12CommandQueue>& GetCommandQueue(void);
private:
	MDx12CommandQueue();
	~MDx12CommandQueue();
	MDx12CommandQueue(const MDx12CommandQueue&) = delete;
	MDx12CommandQueue operator=(const MDx12CommandQueue&) = delete;
	
	ComPtr<ID3D12CommandQueue> commandQueue_;
};

