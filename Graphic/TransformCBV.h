#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <memory>

using namespace Microsoft::WRL;

#define TransHeap TransformCBV::GetInstance().GetDescriptorHeap()

struct TransMatrix;
struct Trans2DMatrix;
class TransformCBV
{
public:
	TransformCBV();
	~TransformCBV() = default;
	void Initialize(unsigned int width, unsigned int height);
	const ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap(void);
	const ComPtr<ID3D12DescriptorHeap>& Get2DDescriptorHeap(void);

	const TransMatrix* GetTransMatrix(void);
	Trans2DMatrix* GetTrans2DMatrix(void);
	void SetCameraCoordinate(const DirectX::XMFLOAT2& coord);
private:
	TransMatrix* mat_;
	ComPtr<ID3D12Resource> buffer_;
	ComPtr<ID3D12DescriptorHeap> heap_;

	Trans2DMatrix* mat2d_;
	ComPtr<ID3D12Resource> buffer2d_;
	ComPtr<ID3D12DescriptorHeap> heap2d_;

	void CreateConstantBuffer(void);
	void CreateTransformMatrix(unsigned int width, unsigned int height);
};

struct TransMatrix
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct Trans2DMatrix
{
	DirectX::XMMATRIX scale_;
	DirectX::XMMATRIX translation_;
	DirectX::XMFLOAT2 camera_;
};
