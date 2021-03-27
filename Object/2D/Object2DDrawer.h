#pragma once
#include <d3dx12.h>
#include <memory>
#include <vector>

using namespace Microsoft::WRL;

class Object2D;
class Object2DDrawer
{
public:
	Object2DDrawer() = default;
	~Object2DDrawer() = default;

	/// <summary>
	/// パイプラインの初期化
	/// </summary>
	void CreatePipelineState(void);

	/// <summary>
	/// パイプラインのセット
	/// </summary>
	void SetPipelineState(ID3D12GraphicsCommandList* cList);

	/// <summary>
	/// 背景の描画
	/// </summary>
	void DrawBG(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans);

	/// <summary>
	/// オブジェクトの登録
	/// </summary>
	void RegisterObject(const std::shared_ptr<Object2D>& object);

	/// <summary>
	/// オブジェクトの登録解除
	/// </summary>
	void UnregisterObject(const std::shared_ptr<Object2D>& object);
private:
	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;

	std::vector<std::weak_ptr<Object2D>> objects_;
};

