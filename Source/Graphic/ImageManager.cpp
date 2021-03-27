#include "ImageManager.h"
#include <DirectXTex.h>
#include "../D3D12/MDx12CommandList.h"
#include "../D3D12/MDx12CommandQueue.h"
#include "../D3D12/MDx12Device.h"
#include "../D3D12/MDx12Fence.h"
#include "../Utility/utility.h"
#include "../Window/DisplayException.h"

using namespace DirectX;

ImageManager::ImageManager()
{
}

ImageManager::~ImageManager()
{
	for (auto& t : texture_)
	{
		t.second->Release();
	}
}

bool ImageManager::LoadImageData(const std::string& filename, ComPtr<ID3D12Resource>& buffer)
{
	if (texture_.find(filename) != texture_.end())
	{
		buffer.Attach(texture_.at(filename).Get());
		texture_.at(filename).Get()->AddRef();
		return true;
	}
	HRESULT result = E_INVALIDARG;
	TexMetadata meta = {};
	ScratchImage scrimage = {};

	std::string ext = GetExtension(filename);
	if (ext == "png" || ext == "jpg" || ext == "bmp")
	{
		result = LoadFromWICFile(WstringToString(filename).c_str(), WIC_FLAGS_NONE, &meta, scrimage);
	}
	else
	{
		std::wstring exc = L"Oops! image resource \"" + WstringToString(filename) + L"\"is not found";
			DisplayException::DisplayError(exc.c_str());
		return false;
	}
	if (FAILED(result)) { buffer = nullptr;  }

	const Image* image = scrimage.GetImage(0, 0, 0);
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(meta.format, meta.width, static_cast<UINT>(meta.height),
		1, static_cast<UINT16>(meta.mipLevels));

	result = DxDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE,
		&resDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(buffer.GetAddressOf()));
	if (FAILED(result)) { buffer = nullptr; return false; };

	result = buffer->WriteToSubresource(0, nullptr, image->pixels, static_cast<UINT>(image->rowPitch), static_cast<UINT>(image->slicePitch));
	if (FAILED(result)) { buffer = nullptr; return false; };

	texture_.emplace(filename, buffer);
	buffer.Get()->AddRef();
	return true;
}

void ImageManager::CreateColorTexture(unsigned char r, unsigned char g, unsigned char b, ComPtr<ID3D12Resource>& buffer)
{
	std::string key;
	key.push_back(r);
	key.push_back(g);
	key.push_back(b);
	if (texture_.find(key) != texture_.end())
	{
		buffer.Attach(texture_.at(key).Get());
		texture_.at(key).Get()->AddRef();
		return;
	}
	HRESULT result;

	// “]‘—æ
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 4, 4, 1, 1);
	result = DxDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE,
		&resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(buffer.GetAddressOf()));
	if (FAILED(result)) { buffer = nullptr; return; };

	// “]‘—Œ³
	ID3D12Resource* interRes = nullptr;

	CD3DX12_HEAP_PROPERTIES origHeapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC origResDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(buffer.Get(), 0, 1));
	result = DxDevice->CreateCommittedResource(&origHeapProp, D3D12_HEAP_FLAG_NONE,
		&origResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&interRes));
	if (FAILED(result)) { buffer = nullptr; return; };

	std::vector<unsigned char> data(4 * 4 * 4);
	for (int i = 0; i < data.size(); i = i + 4)
	{
		data[i] = r;
		data[static_cast<size_t>(i) + static_cast<size_t>(1)] = g;
		data[static_cast<size_t>(i) + static_cast<size_t>(2)] = b;
		data[static_cast<size_t>(i) + static_cast<size_t>(3)] = 0xff;
	}

	unsigned char* interdata = nullptr;
	interRes->Map(0, nullptr, (void**)&interdata);
	std::copy(data.begin(), data.end(), interdata);
	interRes->Unmap(0, nullptr);

	CD3DX12_TEXTURE_COPY_LOCATION tcl(buffer.Get());

	D3D12_SUBRESOURCE_DATA sd = {};
	sd.pData = data.data();
	sd.RowPitch = 4 * 4;
	sd.SlicePitch = 4 * 4 * 4;

	ID3D12CommandAllocator* cAllocator = RootCmAllocator.Get();
	ID3D12GraphicsCommandList* cList = RootCmList.Get();

	cAllocator->Reset();
	cList->Reset(cAllocator, nullptr);

	UpdateSubresources(cList, buffer.Get(), interRes, 0, 0, 1, &sd);

	D3D12_RESOURCE_BARRIER rbar = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cList->ResourceBarrier(1, &rbar);
	result = cList->Close();
	assert(SUCCEEDED(result));
	ID3D12CommandQueue* cQueue = RootCmQueue.Get();
	cQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&cList);
	MDx12Fence::GetInstance().WaitSignal();

	texture_.emplace(key, buffer);
	buffer.Get()->AddRef();
}

void ImageManager::DeleteTextureData(const std::string& filename)
{
	if (texture_.find(filename) == texture_.end()) { return; }
	texture_.at(filename)->Release();
	texture_.erase(filename);
}

void ImageManager::DeleteTextureDataAll(void)
{
	for (auto& t : texture_)
	{
		t.second->Release();
	}
	texture_.clear();
}
