#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_map>

#ifdef _DEBUG
#pragma comment(lib, "Effekseerd.lib")
#pragma comment(lib, "EffekseerRendererDX12d.lib")
#pragma comment(lib, "LLGId.lib")
#else
#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "EffekseerRendererDX12.lib")
#pragma comment(lib, "LLGI.lib")
#endif

#define EfkIns MEffekseer::GetInstance()

struct PlayingEffect
{
	Effekseer::Handle handle_;
	Effekseer::Effect* effect_;
	float rx = 0.0f, ry = 0.0f, rz = 0.0f;
	float cx = 0.0f, cy = 0.0f, cz = 0.0f;
	float sx = 1.0f, sy = 1.0f, sz = 1.0f;
	bool loop_ = false;
};

class MEffekseer
{
public:
	static MEffekseer& GetInstance(void);

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, 
		const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& cQue);

	/// <summary>
	/// �J�����s��̃Z�b�g
	/// </summary>
	/// <param name="projection">�v���W�F�N�V�����s��</param>
	/// <param name="view">�r���[�s��</param>
	void SetCamera(const DirectX::XMMATRIX& projection,
		const DirectX::XMMATRIX& view);

	void LoadEffect(const std::u16string& filename, const std::string& key, float scale = 1.0f);

	int PlayEffect(const std::string& key, bool loop);
	void StopEffect(int handle);

	void SetRotation(int handle, float x, float y, float z);
	void SetCoordinate(int handle, float x, float y, float z);
	void SetScale(int handle, float x, float y, float z);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cList, int delta);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(void);
private:
	MEffekseer();
	MEffekseer(const MEffekseer&) = delete;
	MEffekseer operator=(const MEffekseer&) = delete;
	~MEffekseer();

	Effekseer::Manager* efkManager_;
	EffekseerRenderer::Renderer* efkRenderer_;
	EffekseerRenderer::SingleFrameMemoryPool* efkMemPool_;
	EffekseerRenderer::CommandList* efkCommandList_;
	std::unordered_map<std::string, Effekseer::Effect*> resource_;
	std::vector<PlayingEffect> playing_;
};

