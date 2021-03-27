#include "Square2D.h"

using namespace DirectX;

Square2D::~Square2D()
{
}

void Square2D::CreateObject(float center_x, float center_y, float size_x, float size_y, const DirectX::XMFLOAT4& color, const DirectX::XMMATRIX& rotation)
{
	size_x = size_x / 2.0f;
	size_y = size_y / 2.0f;
	vertices_.resize(vertexCount_);
	for (unsigned int i = 0; i < vertexCount_; i++)
	{
		vertices_[i].pos_.x = (i & 0b001 ? size_x : -size_x);
		vertices_[i].pos_.y = (i & 0b010 ? -size_y : size_y);
		vertices_[i].pos_.z = 0.0f;
		vertices_[i].uv_.x = (i & 0b001 ? 1.0f : 0.0f);
		vertices_[i].uv_.y = (i & 0b010 ? 0.0f : 1.0f);
	}
	index_.resize(indexCount_);
	for (unsigned int i = 0; i < indexCount_; i++)
	{
		index_[i] = indexData_[i];
	}

	XMFLOAT4 coord = { center_x, center_y, 0.0f, 1.0f };
	CreateVertexResource();
	CreateIndexResource();
	CreateConstantResource(coord, color, rotation);
	CreateConstantBufferDescriptor();
	CreateVertexBufferView();
	CreateIndexBufferView();
	CreateConstantBufferView();
	CreateTexture(0xff, 0xff, 0xff);
	CreateTextureDescriptor();
	CreateTextureShaderResourceView();
}

void Square2D::CreateObject(std::string filename, 
	float center_x, float center_y, float size_x, float size_y, const DirectX::XMMATRIX& rotation)
{
	size_x = size_x / 2.0f;
	size_y = size_y / 2.0f;
	vertices_.resize(vertexCount_);
	for (unsigned int i = 0; i < vertexCount_; i++)
	{
		vertices_[i].pos_.x = (i & 0b001 ? size_x : -size_x);
		vertices_[i].pos_.y = (i & 0b010 ? -size_y : size_y);
		vertices_[i].pos_.z = 0.0f;
		vertices_[i].uv_.x = (i & 0b001 ? 1.0f : 0.0f);
		vertices_[i].uv_.y = (i & 0b010 ? 0.0f : 1.0f);
	}
	index_.resize(indexCount_);
	for (unsigned int i = 0; i < indexCount_; i++)
	{
		index_[i] = indexData_[i];
	}

	XMFLOAT4 coord = { center_x, center_y, 0.0f, 1.0f };
	CreateVertexResource();
	CreateIndexResource();
	CreateConstantResource(coord, { 1.0f, 1.0f, 1.0f, 1.0f }, rotation);
	CreateConstantBufferDescriptor();
	CreateVertexBufferView();
	CreateIndexBufferView();
	CreateConstantBufferView();
	if (!CreateTexture(filename))
	{
		CreateTexture(0xff, 0xff, 0xff);
	}
	CreateTextureDescriptor();
	CreateTextureShaderResourceView();
}

void Square2D::resetSize(float size_x, float size_y)
{
	size_x = size_x / 2.0f;
	size_y = size_y / 2.0f;
	for (unsigned int i = 0; i < vertexCount_; i++)
	{
		vertices_[i].pos_.x = (i & 0b001 ? size_x : -size_x);
		vertices_[i].pos_.y = (i & 0b010 ? -size_y : size_y);
		vertices_[i].pos_.z = 0.0f;
		vertices_[i].uv_.x = (i & 0b001 ? 1.0f : 0.0f);
		vertices_[i].uv_.y = (i & 0b010 ? 0.0f : 1.0f);
	}
	
	SetVertex();
}

void Square2D::SetUV(float top, float left, float bottom, float right)
{
	for (unsigned int i = 0; i < vertexCount_; i++)
	{
		vertices_[i].uv_.x = (i & 0b001 ? right : left);
		vertices_[i].uv_.y = (i & 0b010 ? top : bottom);
	}
	SetVertex();
}
