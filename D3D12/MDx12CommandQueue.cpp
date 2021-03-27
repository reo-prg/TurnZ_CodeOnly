#include "MDx12CommandQueue.h"
#include "MDx12Device.h"
#include "../Window/DisplayException.h"

MDx12CommandQueue& MDx12CommandQueue::GetInstance(void)
{
	static MDx12CommandQueue q;
	return q;
}

const ComPtr<ID3D12CommandQueue>& MDx12CommandQueue::GetCommandQueue(void)
{
	if (commandQueue_ == nullptr)
	{
		Initialize();
	}
	return commandQueue_;
}

MDx12CommandQueue::MDx12CommandQueue():commandQueue_(nullptr)
{
}

MDx12CommandQueue::~MDx12CommandQueue()
{
}

void MDx12CommandQueue::Initialize(void)
{
	if (commandQueue_ != nullptr)
	{
		OutputDebugString(L"Create CommandQueue multiply\n");
		return;
	}

	HRESULT result;
	D3D12_COMMAND_QUEUE_DESC cqd = {};
	cqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqd.NodeMask = 0;
	cqd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cqd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = DxDevice->CreateCommandQueue(&cqd, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	if (FAILED(result)) { DisplayException::DisplayError(L"Create Command Que is failed :("); return; }
}
