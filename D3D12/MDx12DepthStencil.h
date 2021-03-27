#pragma once
#include <d3dx12.h>

using namespace Microsoft::WRL;

#define DxDSBuffer MDx12DepthStencil::GetInstance().GetDSBuffer()
#define DxDSHeap MDx12DepthStencil::GetInstance().GetDSDescriptorHeap()
#define DxDSView MDx12DepthStencil::GetInstance().GetDSDesc()

class MDx12DepthStencil
{
public:
	static MDx12DepthStencil& GetInstance(void);

	const ComPtr<ID3D12Resource>& GetDSBuffer(void);
	const ComPtr<ID3D12DescriptorHeap>& GetDSDescriptorHeap(void);
	const D3D12_DEPTH_STENCIL_VIEW_DESC& GetDSDesc(void);

	void Initialize(void);
private:
	MDx12DepthStencil();
	~MDx12DepthStencil() = default;

	ComPtr<ID3D12Resource> depthStencilBuffer_;
	ComPtr<ID3D12DescriptorHeap> depthStencilHeap_;
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilView_ = {};
};

