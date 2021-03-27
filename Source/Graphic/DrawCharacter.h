#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

class DrawCharacter
{
public:
	DrawCharacter();
	~DrawCharacter();

	void LoadFont(const std::wstring& font, const std::string& key);

	void DrawString(const std::string& str, const std::string& font, 
		const DirectX::XMFLOAT2 coord, const DirectX::XMFLOAT4& color);
	void DrawString(const std::wstring& str, const std::string& font,
		const DirectX::XMFLOAT2 coord, const DirectX::XMFLOAT4& color);
	void Draw(ID3D12GraphicsCommandList* cList, const DirectX::XMFLOAT2& camera);
	void Commit(void);
private:
	void Initialize(void);

	DirectX::GraphicsMemory* gMemory_;

	struct Data
	{
		std::string str_;
		std::string font_;
		DirectX::XMFLOAT2 coord_;
		DirectX::XMFLOAT4 color_;
	};
	std::vector<std::unique_ptr<Data>> datas_;

	struct wData
	{
		std::wstring str_;
		std::string font_;
		DirectX::XMFLOAT2 coord_;
		DirectX::XMFLOAT4 color_;
	};
	std::vector<std::unique_ptr<wData>> wdatas_;

	struct Font
	{
		DirectX::SpriteBatch* sBatch_;
		DirectX::ResourceUploadBatch* resBatch_;
		DirectX::SpriteFont* sFont_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> fontHeap_;
	};
	std::unordered_map<std::string, std::shared_ptr<Font>> font_;
};

