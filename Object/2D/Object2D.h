#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

using namespace Microsoft::WRL;

struct ObjectVertex2D;
struct ObjectDesc2D;
class Object2DDrawer;
class Object2D
{
	friend Object2DDrawer;
public:
	Object2D();
	Object2D(int zBuffer);
	virtual ~Object2D();

	/// <summary>
	/// ��]�����Z
	/// </summary>
	/// <param name="angle">�p�x(���W�A��)</param>
	void AddRotation(float angle);

	/// <summary>
	/// ��]��ݒ�
	/// </summary>
	/// <param name="angle">�p�x(���W�A��)</param>
	void SetRotation(float angle);

	/// <summary>
	/// �|���S���̃T�C�Y�̕ύX
	/// </summary>
	virtual void resetSize(float size_x, float size_y) = 0;

	/// <summary>
	/// �ʒu�̉��Z
	/// </summary>
	void AddCoordinate(float x, float y, float z = 0.0f);

	/// <summary>
	/// �ʒu�̐ݒ�
	/// </summary>
	void SetCoordinate(float x, float y, float z = 0.0f);

	/// <summary>
	/// �x�[�X�J���[�̐ݒ�
	/// </summary>
	/// <param name="color">RGB�F</param>
	void SetColor(const DirectX::XMFLOAT3& color);

	/// <summary>
	/// �A���t�@�l�̐ݒ�
	/// </summary>
	void SetTransparency(float trans);

	/// <summary>
	/// UV�l�̐ݒ�
	/// </summary>
	virtual void SetUV(float top, float left, float bottom, float right);

	/// <summary>
	/// UV�l�̉��Z
	/// </summary>
	void AddUV(float x, float y);

	int zBuffer_ = 0;
	bool isBackGround_ = false;

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cList">�R�}���h���X�g</param>
	/// <param name="trans">�ϊ��s��̃q�[�v</param>
	void Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans);
protected:

	// ���_
	std::vector<ObjectVertex2D> vertices_;
	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	ObjectVertex2D* vermap_ = nullptr;
	
	// �C���f�b�N�X
	std::vector<unsigned short> index_;
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	// �ό`�s��
	ObjectDesc2D* desc_;
	ComPtr<ID3D12Resource> constantBuffer_;
	ComPtr<ID3D12DescriptorHeap> constantHeap_;

	// �e�N�X�`��
	ComPtr<ID3D12Resource> textureBuffer_;
	ComPtr<ID3D12DescriptorHeap> textureHeap_;

	// �R�}���h���X�g


	void CreateVertexResource(void);
	void CreateIndexResource(void);
	void CreateConstantResource(const DirectX::XMFLOAT4& coord,
		const DirectX::XMFLOAT4& color,
		const DirectX::XMMATRIX& rotation);

	void CreateConstantBufferDescriptor(void);
	void CreateVertexBufferView(void);
	void CreateIndexBufferView(void);
	void CreateConstantBufferView(void);

	bool CreateTexture(const std::string& filename);
	void CreateTexture(unsigned char r, unsigned char g, unsigned char b);
	void CreateTextureDescriptor(void);
	void CreateTextureShaderResourceView(void);

	void SetVertex(void);
};

struct ObjectVertex2D
{
	DirectX::XMFLOAT3 pos_;
	DirectX::XMFLOAT2 uv_;
};

struct ObjectDesc2D
{
	DirectX::XMFLOAT4 coordinate_;
	DirectX::XMFLOAT4 color_;
	DirectX::XMMATRIX rotation_;
};
