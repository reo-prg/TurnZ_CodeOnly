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
	/// 回転を加算
	/// </summary>
	/// <param name="angle">角度(ラジアン)</param>
	void AddRotation(float angle);

	/// <summary>
	/// 回転を設定
	/// </summary>
	/// <param name="angle">角度(ラジアン)</param>
	void SetRotation(float angle);

	/// <summary>
	/// ポリゴンのサイズの変更
	/// </summary>
	virtual void resetSize(float size_x, float size_y) = 0;

	/// <summary>
	/// 位置の加算
	/// </summary>
	void AddCoordinate(float x, float y, float z = 0.0f);

	/// <summary>
	/// 位置の設定
	/// </summary>
	void SetCoordinate(float x, float y, float z = 0.0f);

	/// <summary>
	/// ベースカラーの設定
	/// </summary>
	/// <param name="color">RGB色</param>
	void SetColor(const DirectX::XMFLOAT3& color);

	/// <summary>
	/// アルファ値の設定
	/// </summary>
	void SetTransparency(float trans);

	/// <summary>
	/// UV値の設定
	/// </summary>
	virtual void SetUV(float top, float left, float bottom, float right);

	/// <summary>
	/// UV値の加算
	/// </summary>
	void AddUV(float x, float y);

	int zBuffer_ = 0;
	bool isBackGround_ = false;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cList">コマンドリスト</param>
	/// <param name="trans">変換行列のヒープ</param>
	void Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans);
protected:

	// 頂点
	std::vector<ObjectVertex2D> vertices_;
	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	ObjectVertex2D* vermap_ = nullptr;
	
	// インデックス
	std::vector<unsigned short> index_;
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	// 変形行列
	ObjectDesc2D* desc_;
	ComPtr<ID3D12Resource> constantBuffer_;
	ComPtr<ID3D12DescriptorHeap> constantHeap_;

	// テクスチャ
	ComPtr<ID3D12Resource> textureBuffer_;
	ComPtr<ID3D12DescriptorHeap> textureHeap_;

	// コマンドリスト


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
