#include "Box3D.h"

void Box3D::CreateObject(const DirectX::XMFLOAT4& coord, 
	const DirectX::XMFLOAT4& color, 
	const DirectX::XMMATRIX& rotation, 
	const DirectX::XMFLOAT4& size)
{
	vertices_.resize(vertexCount_);
	for (unsigned int i = 0; i < vertexCount_; i++)
	{
		unsigned int idx = i / 3;
		vertices_[i].pos_.x = (idx & 0b001 ? 0.5f : -0.5f);
		vertices_[i].pos_.y = (idx & 0b010 ? 0.5f : -0.5f);
		vertices_[i].pos_.z = (idx & 0b100 ? 0.5f : -0.5f);
	}
	index_.resize(indexCount_);
	for (unsigned int i = 0; i < indexCount_; i++)
	{
		index_[i] = indexData_[i];
	}

	CreateVertexResource();
	CreateIndexResource();
	CreateConstantResource(coord, color, rotation, size);
	CreateConstantBufferDescriptor();
	CreateVertexBufferView();
	CreateIndexBufferView();
	CreateConstantBufferView();
}
