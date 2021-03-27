#include "MDx12DepthStencil.h"
#include "MDx12Device.h"
#include "../Application.h"
#include "../Window/DisplayException.h"

MDx12DepthStencil& MDx12DepthStencil::GetInstance(void)
{
	static MDx12DepthStencil d;
	return d;
}

const ComPtr<ID3D12Resource>& MDx12DepthStencil::GetDSBuffer(void)
{
	if (depthStencilBuffer_ == nullptr)
	{
		Initialize();
	}
	return depthStencilBuffer_;
}

const ComPtr<ID3D12DescriptorHeap>& MDx12DepthStencil::GetDSDescriptorHeap(void)
{
	if (depthStencilHeap_ == nullptr)
	{
		Initialize();
	}
	return depthStencilHeap_;
}

const D3D12_DEPTH_STENCIL_VIEW_DESC& MDx12DepthStencil::GetDSDesc(void)
{
	return depthStencilView_;
}

void MDx12DepthStencil::Initialize(void)
{
	HRESULT result;

	unsigned int width, height;
	Application::GetInstance().GetWindowSize(width, height);

	CD3DX12_HEAP_PROPERTIES dsHeap(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
		1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	CD3DX12_CLEAR_VALUE cvalue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
	result = DxDevice->CreateCommittedResource(&dsHeap, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &cvalue, IID_PPV_ARGS(depthStencilBuffer_.GetAddressOf()));
	if (FAILED(result)) { DisplayException::DisplayError(L"Create DepthStencil Resource is failed :("); return; }
	D3D12_DESCRIPTOR_HEAP_DESC dsdDesc_ = {};
	dsdDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsdDesc_.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsdDesc_.NumDescriptors = 1;
	dsdDesc_.NodeMask = 0;
	result = DxDevice->CreateDescriptorHeap(&dsdDesc_, IID_PPV_ARGS(depthStencilHeap_.GetAddressOf()));
	if (FAILED(result)) { DisplayException::DisplayError(L"Create DepthStencil Heap is failed :("); return; }

	depthStencilView_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilView_.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilView_.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = depthStencilHeap_->GetCPUDescriptorHandleForHeapStart();
	DxDevice->CreateDepthStencilView(depthStencilBuffer_.Get(), &depthStencilView_, handle);
}

MDx12DepthStencil::MDx12DepthStencil():depthStencilBuffer_(nullptr), depthStencilHeap_(nullptr)
{
}
