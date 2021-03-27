#include "Object2D.h"
#include "../../D3D12/MDx12Device.h"
#include "../../Graphic/Drawer.h"
#include "../../Graphic/ImageManager.h"
#include "../../Utility/utility.h"

Object2D::Object2D():
	vertexBuffer_(nullptr), indexBuffer_(nullptr), constantBuffer_(nullptr), constantHeap_(nullptr)
{
}

Object2D::Object2D(int zBuffer) : zBuffer_(zBuffer),
	vertexBuffer_(nullptr), indexBuffer_(nullptr), constantBuffer_(nullptr), constantHeap_(nullptr)
{
}

Object2D::~Object2D()
{
	vertexBuffer_->Unmap(0, nullptr);
	constantBuffer_->Unmap(0, nullptr);
}

void Object2D::AddRotation(float angle)
{
	desc_->rotation_ *= DirectX::XMMatrixRotationZ(angle);
}

void Object2D::SetRotation(float angle)
{
	desc_->rotation_ = DirectX::XMMatrixRotationZ(angle);
}

void Object2D::AddCoordinate(float x, float y, float z)
{
	desc_->coordinate_.x += x;
	desc_->coordinate_.y += y;
	desc_->coordinate_.z += z;
}

void Object2D::SetCoordinate(float x, float y, float z)
{
	desc_->coordinate_.x = x;
	desc_->coordinate_.y = y;
	desc_->coordinate_.z = z;
}

void Object2D::SetColor(const DirectX::XMFLOAT3& color)
{
	desc_->color_.x = color.x;
	desc_->color_.y = color.y;
	desc_->color_.z = color.z;
}

void Object2D::SetTransparency(float trans)
{
	desc_->color_.w = trans;
}

void Object2D::SetUV(float top, float left, float bottom, float right)
{
}

void Object2D::AddUV(float x, float y)
{
	for (auto& v : vertices_)
	{
		v.uv_.x += x;
		v.uv_.y += y;
	}
	SetVertex();
}

void Object2D::Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans)
{
	cList->SetDescriptorHeaps(1, &trans);
	cList->SetGraphicsRootDescriptorTable(0, trans->GetGPUDescriptorHandleForHeapStart());
	cList->SetDescriptorHeaps(1, constantHeap_.GetAddressOf());
	cList->SetGraphicsRootDescriptorTable(1, constantHeap_->GetGPUDescriptorHandleForHeapStart());
	cList->SetDescriptorHeaps(1, textureHeap_.GetAddressOf());
	cList->SetGraphicsRootDescriptorTable(2, textureHeap_->GetGPUDescriptorHandleForHeapStart());

	cList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cList->IASetIndexBuffer(&indexBufferView_);
	cList->DrawIndexedInstanced(static_cast<UINT>(index_.size()), 1, 0, 0, 0);
}

void Object2D::CreateVertexResource(void)
{
	HRESULT result;

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices_.size() * sizeof(vertices_[0]));

	// ’¸“_î•ñ‚Ìì¬
	result = DxDevice->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE, &resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertexBuffer_.GetAddressOf()));
	assert(SUCCEEDED(result));

	result = vertexBuffer_->Map(0, nullptr, (void**)&vermap_);
	assert(SUCCEEDED(result));

	std::copy(vertices_.begin(), vertices_.end(), vermap_);
}

void Object2D::CreateIndexResource(void)
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

void Object2D::CreateConstantResource(const DirectX::XMFLOAT4& coord,
	const DirectX::XMFLOAT4& color,
	const DirectX::XMMATRIX& rotation)
{
	HRESULT result;

	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(AlignTo(sizeof(ObjectDesc2D), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

	result = DxDevice->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(constantBuffer_.GetAddressOf()));
	assert(SUCCEEDED(result));

	constantBuffer_->Map(0, nullptr, (void**)&desc_);

	desc_->color_ = color;
	desc_->coordinate_ = coord;
	desc_->rotation_ = rotation;
}

void Object2D::CreateConstantBufferDescriptor(void)
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

void Object2D::CreateVertexBufferView(void)
{
	if (vertices_.empty() || vertexBuffer_ == nullptr)
	{
		OutputDebugString(L"Vertices not set");
		return;
	}

	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(vertices_[0]) * vertices_.size());
	vertexBufferView_.StrideInBytes = static_cast<UINT>(sizeof(vertices_[0]));
}

void Object2D::CreateIndexBufferView(void)
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

void Object2D::CreateConstantBufferView(void)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = constantHeap_->GetCPUDescriptorHandleForHeapStart();
	auto cbDesc = constantBuffer_->GetDesc();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constantBuffer_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(cbDesc.Width);

	DxDevice->CreateConstantBufferView(&cbvDesc, handle);
}

bool Object2D::CreateTexture(const std::string& filename)
{
	return Drawer::GetInstance().GetImageManager()->LoadImageData(filename, textureBuffer_);
}

void Object2D::CreateTexture(unsigned char r, unsigned char g, unsigned char b)
{
	Drawer::GetInstance().GetImageManager()->CreateColorTexture(r, g, b, textureBuffer_);
}

void Object2D::CreateTextureDescriptor(void)
{
	HRESULT result;

	D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
	dhd.NumDescriptors = 1;
	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	dhd.NodeMask = 0;

	result = DxDevice->CreateDescriptorHeap(&dhd,
		IID_PPV_ARGS(textureHeap_.GetAddressOf()));
	assert(SUCCEEDED(result));
}

void Object2D::CreateTextureShaderResourceView(void)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureBuffer_->GetDesc().Format;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = textureHeap_->GetCPUDescriptorHandleForHeapStart();
	DxDevice->CreateShaderResourceView(textureBuffer_.Get(), &srvDesc, handle);
}

void Object2D::SetVertex(void)
{
	std::copy(vertices_.begin(), vertices_.end(), vermap_);
}

