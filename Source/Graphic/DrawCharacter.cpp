#include "DrawCharacter.h"
#include "../D3D12/MDx12CommandQueue.h"
#include "../D3D12/MDx12Device.h"
#include "../D3D12/MDx12Fence.h"
#include "../Application.h"

#pragma comment(lib, "DirectXTK12.lib")
using namespace DirectX;
namespace
{
	XMFLOAT2 operator-(const XMFLOAT2& val1, const XMFLOAT2& val2)
	{
		return XMFLOAT2(val1.x - val2.x, val1.y - val2.y);
	}
}

DrawCharacter::DrawCharacter()
{
	Initialize();
}

DrawCharacter::~DrawCharacter()
{
	datas_.clear();
	for (auto& f : font_)
	{
		f.second->fontHeap_.Reset();
		delete f.second->sFont_;
		delete f.second->sBatch_;
		delete f.second->resBatch_;
	}
	delete gMemory_;
}

void DrawCharacter::LoadFont(const std::wstring& font, const std::string& key)
{
	if (font_.find(key) != font_.end())
	{
		return;
	}
	unsigned int width, height;
	Application::GetInstance().GetWindowSize(width, height);
	CD3DX12_VIEWPORT vp(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

	Font* fnt = new Font();

	fnt->resBatch_ = new DirectX::ResourceUploadBatch(DxDevice.Get());
	fnt->resBatch_->Begin();
	DirectX::RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
	DirectX::SpriteBatchPipelineStateDescription sbpsd(rtState);
	fnt->sBatch_ = new DirectX::SpriteBatch(DxDevice.Get(), *fnt->resBatch_, sbpsd, &vp);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DxDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(fnt->fontHeap_.GetAddressOf()));

	fnt->sFont_ = new DirectX::SpriteFont(DxDevice.Get(), *fnt->resBatch_, font.c_str(),
		fnt->fontHeap_->GetCPUDescriptorHandleForHeapStart(),
		fnt->fontHeap_->GetGPUDescriptorHandleForHeapStart());

	font_.emplace(key, fnt);

	auto ftr = fnt->resBatch_->End(RootCmQueue.Get());
	MDx12Fence::GetInstance().WaitSignal();
	ftr.wait();
}

void DrawCharacter::DrawString(const std::string& str, const std::string& font, 
	const DirectX::XMFLOAT2 coord, const DirectX::XMFLOAT4& color)
{
	if (font_.find(font) == font_.end())
	{
		return;
	}

	Data* dt = new Data();
	dt->str_ = str;
	dt->font_ = font;
	dt->coord_ = coord;
	dt->color_ = color;
	datas_.emplace_back(dt);
}

void DrawCharacter::DrawString(const std::wstring& str, const std::string& font, const DirectX::XMFLOAT2 coord, const DirectX::XMFLOAT4& color)
{
	if (font_.find(font) == font_.end())
	{
		return;
	}

	wData* dt = new wData();
	dt->str_ = str;
	dt->font_ = font;
	dt->coord_ = coord;
	dt->color_ = color;
	wdatas_.emplace_back(dt);
}

void DrawCharacter::Draw(ID3D12GraphicsCommandList* cList, const DirectX::XMFLOAT2& camera)
{

	Font* ft;
	if (!datas_.empty())
	{
		for (auto& d : datas_)
		{
			if (font_.find(d->font_) == font_.end()) { continue; }
			ft = font_.at(d->font_).get();
			ft->sBatch_->Begin(cList);
			cList->SetDescriptorHeaps(1, ft->fontHeap_.GetAddressOf());
			ft->sFont_->DrawString(ft->sBatch_, d->str_.c_str(), d->coord_ - camera, DirectX::XMLoadFloat4(&(d->color_)));
			ft->sBatch_->End();
		}
		datas_.clear();
	}
	if (!wdatas_.empty())
	{
		for (auto& d : wdatas_)
		{
			if (font_.find(d->font_) == font_.end()) { continue; }
			ft = font_.at(d->font_).get();
			ft->sBatch_->Begin(cList);
			cList->SetDescriptorHeaps(1, ft->fontHeap_.GetAddressOf());
			ft->sFont_->DrawString(ft->sBatch_, d->str_.c_str(), d->coord_ - camera, DirectX::XMLoadFloat4(&(d->color_)));
			ft->sBatch_->End();
		}
		wdatas_.clear();
	}
}

void DrawCharacter::Commit(void)
{
	gMemory_->Commit(RootCmQueue.Get());
}

void DrawCharacter::Initialize(void)
{
	gMemory_ = new DirectX::GraphicsMemory(DxDevice.Get());

	//---------------------------------------------------------------------------------------
	LoadFont(L"Resource/Font/MainMoreBig.spritefont", "main_bb");
	LoadFont(L"Resource/Font/MainBig.spritefont", "main_b");
	LoadFont(L"Resource/Font/MainSmall.spritefont", "main_s");
	LoadFont(L"Resource/Font/setting.spritefont", "setting_b");
	LoadFont(L"Resource/Font/setting_s.spritefont", "setting_s");
	//---------------------------------------------------------------------------------------
}
