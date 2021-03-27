#include "MDx12CommandList.h"
#include "MDx12Device.h"
#include "../Window/DisplayException.h"

MDx12CommandList& MDx12CommandList::GetInstance(void)
{
	static MDx12CommandList c;
	return c;
}

void MDx12CommandList::InitRootCommandList(void)
{
	HRESULT result;
	result = DxDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmAllocator_.GetAddressOf()));
	if (FAILED(result)) { DisplayException::DisplayError(L"Create Command Allocator is failed :("); return; }
	result = DxDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmAllocator_.Get(),
		nullptr, IID_PPV_ARGS(cmList_.GetAddressOf()));
	if (FAILED(result)) { DisplayException::DisplayError(L"Create Command List is failed :("); return; }
	cmList_->Close();
}

const ComPtr<ID3D12CommandAllocator>& MDx12CommandList::GetCommandAllocator(void)
{
	if (cmAllocator_ == nullptr)
	{
		InitRootCommandList();
	}
	return cmAllocator_;
}

const ComPtr<ID3D12GraphicsCommandList>& MDx12CommandList::GetCommandList(void)
{
	if (cmList_ == nullptr)
	{
		InitRootCommandList();
	}
	return cmList_;
}

MDx12CommandList::MDx12CommandList():cmList_(nullptr), cmAllocator_(nullptr)
{
}

MDx12CommandList::~MDx12CommandList()
{
}

namespace cmlist
{
	void CreateCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12PipelineState* pipeline, 
		ID3D12GraphicsCommandList* cmdList, ID3D12CommandAllocator* cmdAlloc)
	{

	}
}
