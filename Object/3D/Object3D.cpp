#include "Object3D.h"
#include "../../D3D12/MDx12Device.h"
#include "../../Utility/utility.h"

Object3D::Object3D():
	vertexBuffer_(nullptr), indexBuffer_(nullptr), constantBuffer_(nullptr), constantHeap_(nullptr)
{
}

void Object3D::AddRotation(float pitch, float yaw, float roll)
{
	desc_->rotation_ *= DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

void Object3D::Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* wvp)
{
	cList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cList->SetDescriptorHeaps(1, &wvp);
	cList->SetGraphicsRootDescriptorTable(0, wvp->GetGPUDescriptorHandleForHeapStart());
	cList->SetDescriptorHeaps(1 ,constantHeap_.GetAddressOf());
	cList->SetGraphicsRootDescriptorTable(1, constantHeap_->GetGPUDescriptorHandleForHeapStart());

	cList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cList->IASetIndexBuffer(&indexBufferView_);
	cList->DrawIndexedInstanced(static_cast<UINT>(index_.size()), 1, 0, 0, 0);
}

void Object3D::CreateVertexResource(void)
{
	HRESULT result;

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices_.size() * sizeof(vertices_[0]));

	// ’¸“_î•ñ‚Ìì¬
	result =DxDevice->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE, &resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertexBuffer_.GetAddressOf()));
	assert(SUCCEEDED(result));

	ObjectVertex* vermap = nullptr;
	result = vertexBuffer_->Map(0, nullptr, (void**)&vermap);
	assert(SUCCEEDED(result));

	std::copy(vertices_.begin(), vertices_.end(), vermap);
	vertexBuffer_->Unmap(0, nullptr);
}

void Object3D::CreateIndexResource(void)
{
	HRESULT result;

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(index_.size() * sizeof(index_[0]));

	result = DxDevice->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE, &resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(indexBuffer_.GetAddressOf()));
	assert(SUCCEEDED(result));

	unsigned short* idxdata = nullptr;
	result = indexBuffer_->Map(0, nullptr, (void**)&idxdata);
	assert(SUCCEEDED(result));

	std::copy(index_.begin(), index_.end(), idxdata);
	indexBuffer_->Unmap(0, nullptr);
}

void Object3D::CreateConstantResource(const DirectX::XMFLOAT4& coord, 
	const DirectX::XMFLOAT4& color, 
	const DirectX::XMMATRIX& rotation, 
	const DirectX::XMFLOAT4& size)
{
	HRESULT result;

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(AlignTo(sizeof(ObjectDesc), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

	result = DxDevice->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(constantBuffer_.GetAddressOf()));
	assert(SUCCEEDED(result));

	constantBuffer_->Map(0, nullptr, (void**)&desc_);

	desc_->color_ = color;
	desc_->coordinate_ = coord;
	desc_->rotation_ = rotation;
	desc_->size_ = size;
}

void Object3D::CreateConstantBufferDescriptor(void)
{
	HRESULT result;

	D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
	dhd.NumDescriptors = 1;
	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	dhd.NodeMask = 0;

	result = DxDevice->CreateDescriptorHeap(&dhd,
		IID_PPV_ARGS(constantHeap_.GetAddressOf()));
	assert(SUCCEEDED(result));
}

void Object3D::CreateVertexBufferView(void)
{
	if (vertices_.empty() || vertexBuffer_ == nullptr)
	{
		OutputDebugString(L"Vertices not set");
		return;
	}

	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(vertices_[0]) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(vertices_[0]);
}

void Object3D::CreateIndexBufferView(void)
{
	if (index_.empty() || indexBuffer_ == nullptr)
	{
		OutputDebugString(L"index not set");
		return;
	}

	indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(index_[0]) * index_.size());
	indexBufferView_.Format = DXGI_FORMAT_R16_UINT;
}

void Object3D::CreateConstantBufferView(void)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = constantHeap_->GetCPUDescriptorHandleForHeapStart();
	auto cbDesc = constantBuffer_->GetDesc();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constantBuffer_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(cbDesc.Width);

	DxDevice->CreateConstantBufferView(&cbvDesc, handle);
}
