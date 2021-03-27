#pragma once
#include <d3dx12.h>
#include <memory>
#include <vector>

using namespace Microsoft::WRL;

class Object3D;
class Object3DDrawer
{
public:
	Object3DDrawer() = default;
	~Object3DDrawer() = default;

	void CreatePipelineState(void);
	void SetPipelineState(ID3D12GraphicsCommandList* cList);
	void Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* wvp);

	void RegisterObject(const std::shared_ptr<Object3D>& object);
	void UnregisterObject(const std::shared_ptr<Object3D>& object);
private:
	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;

	std::vector<std::weak_ptr<Object3D>> objects_;
};

