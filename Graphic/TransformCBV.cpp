#include "TransformCBV.h"
#include "../D3D12/MDx12Device.h"
#include "../Utility/utility.h"
#include "../Application.h"
#include <cassert>

using namespace DirectX;

namespace
{
	bool initialized = false;
}

void TransformCBV::Initialize(unsigned int width, unsigned int height)
{
	if (initialized) { return; }
	initialized = true;
	CreateConstantBuffer();
	CreateTransformMatrix(width, height);
}

const ComPtr<ID3D12DescriptorHeap>& TransformCBV::GetDescriptorHeap(void)
{
	assert(heap_ != nullptr);
	return heap_;
}

const ComPtr<ID3D12DescriptorHeap>& TransformCBV::Get2DDescriptorHeap(void)
{
	assert(heap2d_ != nullptr);
	return heap2d_;
}

const TransMatrix* TransformCBV::GetTransMatrix(void)
{
	return mat_;
}

Trans2DMatrix* TransformCBV::GetTrans2DMatrix(void)
{
	return mat2d_;
}

void TransformCBV::SetCameraCoordinate(const DirectX::XMFLOAT2& coord)
{
	mat2d_->camera_ = coord;
}

TransformCBV::TransformCBV():mat_(nullptr), mat2d_(nullptr)
{
}

void TransformCBV::CreateConstantBuffer(void)
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(
		AlignTo(sizeof(TransMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
	result = DxDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(buffer_.GetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	result = DxDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(heap_.GetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = heap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = buffer_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(buffer_->GetDesc().Width);
	DxDevice->CreateConstantBufferView(&cbvDesc, handle);

	D3D12_HEAP_PROPERTIES heapProp2d = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc2d = CD3DX12_RESOURCE_DESC::Buffer(
		AlignTo(sizeof(Trans2DMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
	result = DxDevice->CreateCommittedResource(&heapProp2d, D3D12_HEAP_FLAG_NONE, &resDesc2d,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(buffer2d_.GetAddressOf()));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc2d = {};
	heapDesc2d.NumDescriptors = 1;
	heapDesc2d.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc2d.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc2d.NodeMask = 0;
	result = DxDevice->CreateDescriptorHeap(&heapDesc2d, IID_PPV_ARGS(heap2d_.GetAddressOf()));
	assert(SUCCEEDED(result));

	handle = heap2d_->GetCPUDescriptorHandleForHeapStart();
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc2d = {};
	cbvDesc2d.BufferLocation = buffer2d_->GetGPUVirtualAddress();
	cbvDesc2d.SizeInBytes = static_cast<UINT>(buffer2d_->GetDesc().Width);
	DxDevice->CreateConstantBufferView(&cbvDesc2d, handle);
}

void TransformCBV::CreateTransformMatrix(unsigned int width, unsigned int height)
{
	HRESULT result;
	result = buffer_->Map(0, nullptr, (void**)&mat_);
	assert(SUCCEEDED(result));
	mat_->world = XMMatrixIdentity();
	float w = static_cast<float>(width) / 2.0f, h = static_cast<float>(height) / 2.0f;
	mat_->view = XMMatrixLookAtRH({ -w, -h, -30.0f, 1.0f },
		{ -w, -h, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f });
	mat_->projection = XMMatrixOrthographicRH(static_cast<float>(width),
		 static_cast<float>(height), 0.1f, 1000.0f);

	result = buffer2d_->Map(0, nullptr, (void**)&mat2d_);
	assert(SUCCEEDED(result));

	mat2d_->scale_ = XMMatrixScaling(1.0f / (width / 2.0f), -1.0f / (height / 2.0f), 1.0f);
	mat2d_->translation_ = XMMatrixTranslation(-1.0f, 1.0f, 0.0f);
	mat2d_->camera_ = { 0.0f, 0.0f };
}