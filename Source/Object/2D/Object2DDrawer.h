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
	/// �p�C�v���C���̏�����
	/// </summary>
	void CreatePipelineState(void);

	/// <summary>
	/// �p�C�v���C���̃Z�b�g
	/// </summary>
	void SetPipelineState(ID3D12GraphicsCommandList* cList);

	/// <summary>
	/// �w�i�̕`��
	/// </summary>
	void DrawBG(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans);

	/// <summary>
	/// �I�u�W�F�N�g�̓o�^
	/// </summary>
	void RegisterObject(const std::shared_ptr<Object2D>& object);

	/// <summary>
	/// �I�u�W�F�N�g�̓o�^����
	/// </summary>
	void UnregisterObject(const std::shared_ptr<Object2D>& object);
private:
	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;

	std::vector<std::weak_ptr<Object2D>> objects_;
};

