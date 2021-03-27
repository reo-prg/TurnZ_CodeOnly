#pragma once
#include <string>
#include <unordered_map>
#include <d3dx12.h>

using namespace Microsoft::WRL;
class ImageManager
{
public:
	ImageManager();
	~ImageManager();

	bool LoadImageData(const std::string& filename, ComPtr<ID3D12Resource>& buffer);
	void CreateColorTexture(unsigned char r, unsigned char g, unsigned char b, ComPtr<ID3D12Resource>& buffer);
	void DeleteTextureData(const std::string& filename);
	void DeleteTextureDataAll(void);
private:
	std::unordered_map<std::string, ComPtr<ID3D12Resource>> texture_;
};

