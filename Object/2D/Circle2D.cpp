#include "Circle2D.h"

using namespace DirectX;

void Circle2D::CreateObject(float center_x, float center_y, float size_x, float size_y, unsigned int div, const DirectX::XMFLOAT4& color, const DirectX::XMMATRIX& rotation)
{
	if (div <= 2) 
	{ 
		OutputDebugString(L"Circle2D : Can not create Circle with 'div <= 2'");
		return;
	};
	div_ = div;
	float dur = XM_2PI / div;

	vertices_.resize(static_cast<size_t>(div + 1));
	vertices_[0].pos_.x = 0.0f;
	vertices_[0].pos_.y = 0.0f;
	vertices_[0].pos_.z = 0.0f;
	for (unsigned int i = 0; i < div; i++)
	{
		vertices_[static_cast<size_t>(i + 1)].pos_.x = cosf(dur * i) * size_x;
		vertices_[static_cast<size_t>(i + 1)].pos_.y = sinf(dur * i) * size_y;
		vertices_[static_cast<size_t>(i + 1)].pos_.z = 0.0f;
	}
	for (auto& v : vertices_)
	{
		v.uv_.x = 0.0f;
		v.uv_.y = 0.0f;
	}

	index_.resize(div * 3);
	for (unsigned int i = 0; i < div; i++)
	{
		index_[static_cast<size_t>(i * 3)] = 0;
		index_[static_cast<size_t>(i * 3 + 1)] = i + 1;
		index_[static_cast<size_t>(i * 3 + 2)] = (index_[static_cast<size_t>(i * 3 + 1)] % div) + 1;
	}

	XMFLOAT4 coord = { center_x, center_y, 0.0f, 1.0f };
	CreateVertexResource();
	CreateIndexResource();
	CreateConstantResource(coord, color, rotation);
	CreateConstantBufferDescriptor();
	CreateVertexBufferView();
	CreateIndexBufferView();
	CreateConstantBufferView();
	CreateTexture(0xff, 0x00, 0x00);
	CreateTextureDescriptor();
	CreateTextureShaderResourceView();
}

void Circle2D::resetSize(float size_x, float size_y)
{
	float dur = XM_2PI / div_;

	vertices_[0].pos_.x = 0.0f;
	vertices_[0].pos_.y = 0.0f;
	vertices_[0].pos_.z = 0.0f;
	for (unsigned int i = 0; i < div_; i++)
	{
		vertices_[static_cast<size_t>(i + 1)].pos_.x = cosf(dur * i) * size_x;
		vertices_[static_cast<size_t>(i + 1)].pos_.y = sinf(dur * i) * size_y;
		vertices_[static_cast<size_t>(i + 1)].pos_.z = 0.0f;
	}
	for (auto& v : vertices_)
	{
		v.uv_.x = 0.0f;
		v.uv_.y = 0.0f;
	}
	SetVertex();
}
