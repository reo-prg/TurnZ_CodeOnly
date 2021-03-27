#pragma once
#include <d3d12.h>
#include <d3dx12.h>

using namespace Microsoft::WRL;

#define RootCmAllocator MDx12CommandList::GetInstance().GetCommandAllocator()
#define RootCmList MDx12CommandList::GetInstance().GetCommandList()

class MDx12CommandList
{
public:
	static MDx12CommandList& GetInstance(void);
	void InitRootCommandList(void);

	const ComPtr<ID3D12CommandAllocator>& GetCommandAllocator(void);
	const ComPtr<ID3D12GraphicsCommandList>& GetCommandList(void);
private:
	MDx12CommandList();
	~MDx12CommandList();
	MDx12CommandList(const MDx12CommandList&) = delete;
	MDx12CommandList operator=(const MDx12CommandList&) = delete;

	ComPtr<ID3D12CommandAllocator> cmAllocator_;
	ComPtr<ID3D12GraphicsCommandList> cmList_;
};

namespace cmlist
{
	void CreateCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12PipelineState* pipeline, 
		ID3D12GraphicsCommandList* cmdList, ID3D12CommandAllocator* cmdAlloc);
}
