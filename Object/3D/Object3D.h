#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>

using namespace Microsoft::WRL;

struct ObjectVertex;
struct ObjectDesc;
class Object3DDrawer;
class Object3D
{
	friend Object3DDrawer;
public:
	Object3D();
	virtual ~Object3D() = default;

	virtual void CreateObject(const DirectX::XMFLOAT4& coord = { 0.0f, 0.0f, 0.0f, 1.0f },
		const DirectX::XMFLOAT4& color = { 0.0f, 1.0f, 0.0f, 1.0f },
		const DirectX::XMMATRIX& rotation = DirectX::XMMatrixIdentity(),
		const DirectX::XMFLOAT4& size = { 1.0f, 1.0f, 1.0f, 1.0f }) = 0;

	void AddRotation(float pitch, float yaw, float roll);
protected:
	void Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* wvp);

	std::vector<ObjectVertex> vertices_;
	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};

	std::vector<unsigned short> index_;
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	ObjectDesc* desc_;
	ComPtr<ID3D12Resource> constantBuffer_;
	ComPtr<ID3D12DescriptorHeap> constantHeap_;

	void CreateVertexResource(void);
	void CreateIndexResource(void);
	void CreateConstantResource(const DirectX::XMFLOAT4& coord, 
		const DirectX::XMFLOAT4& color,
		const DirectX::XMMATRIX& rotation,
		const DirectX::XMFLOAT4& size);
	void CreateConstantBufferDescriptor(void);
	void CreateVertexBufferView(void);
	void CreateIndexBufferView(void);
	void CreateConstantBufferView(void);
};

struct ObjectVertex
{
	DirectX::XMFLOAT3 pos_;
	DirectX::XMFLOAT3 normal_;
};

struct ObjectDesc
{
	DirectX::XMFLOAT4 coordinate_;
	DirectX::XMFLOAT4 color_;
	DirectX::XMFLOAT4 size_;
	DirectX::XMMATRIX rotation_;
};
