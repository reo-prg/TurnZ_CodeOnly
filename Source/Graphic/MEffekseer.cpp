#include "MEffekseer.h"

namespace
{
	constexpr int efkInstance = 8000;
}

MEffekseer::MEffekseer():efkCommandList_(nullptr), efkManager_(nullptr),
		efkMemPool_(nullptr), efkRenderer_(nullptr)
{
}

MEffekseer::~MEffekseer()
{
	for (auto& p : playing_)
	{
		StopEffect(p.handle_);
	}
	playing_.clear();
	for (auto& r : resource_)
	{
		r.second->Release();
	}
	resource_.clear();
	efkCommandList_->Release();
	efkMemPool_->Release();
	efkRenderer_->Destroy();
	efkManager_->Destroy();
}

MEffekseer& MEffekseer::GetInstance(void)
{
	static MEffekseer e;
	return e;
}

void MEffekseer::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
	const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& cQue)
{
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer_ = ::EffekseerRendererDX12::Create(device.Get(), cQue.Get(),
		2, &format, 1, DXGI_FORMAT_UNKNOWN, false, efkInstance);

	efkMemPool_ = EffekseerRendererDX12::CreateSingleFrameMemoryPool(efkRenderer_);
	efkCommandList_ = EffekseerRendererDX12::CreateCommandList(efkRenderer_, efkMemPool_);

	efkManager_ = Effekseer::Manager::Create(efkInstance);

	efkManager_->SetSpriteRenderer(efkRenderer_->CreateSpriteRenderer());
	efkManager_->SetRibbonRenderer(efkRenderer_->CreateRibbonRenderer());
	efkManager_->SetRingRenderer(efkRenderer_->CreateRingRenderer());
	efkManager_->SetTrackRenderer(efkRenderer_->CreateTrackRenderer());
	efkManager_->SetModelRenderer(efkRenderer_->CreateModelRenderer());

	efkManager_->SetTextureLoader(efkRenderer_->CreateTextureLoader());
	efkManager_->SetModelLoader(efkRenderer_->CreateModelLoader());
	efkManager_->SetMaterialLoader(efkRenderer_->CreateMaterialLoader());
}

void MEffekseer::SetCamera(const DirectX::XMMATRIX& projection, const DirectX::XMMATRIX& view)
{
	Effekseer::Matrix44 projMat, cameraMat;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			projMat.Values[i][j] = projection.r[i].m128_f32[j];
			cameraMat.Values[i][j] = view.r[i].m128_f32[j];
		}
	}
	efkRenderer_->SetProjectionMatrix(projMat);
	efkRenderer_->SetCameraMatrix(cameraMat);
}

void MEffekseer::LoadEffect(const std::u16string& filename, const std::string& key, float scale)
{
	if (resource_.find(key) != resource_.end())
	{
		return;
	}
	Effekseer::Effect* efk = Effekseer::Effect::Create(efkManager_, filename.c_str(), scale);
	if (efk == nullptr) 
	{
		OutputDebugString(L"\nMEffekseer::LoadEffect is failed\n\n");
		return;
	}
	resource_.emplace(key, efk);
}

int MEffekseer::PlayEffect(const std::string& key, bool loop)
{
	if (resource_.find(key) == resource_.end())
	{
		return -1;
	}
	Effekseer::Handle efk = efkManager_->Play(resource_.at(key), 0.0f, 0.0f, 0.0f);
	PlayingEffect p;
	p.handle_ = efk;
	p.effect_ = resource_.at(key);
	p.loop_ = loop;
	playing_.push_back(p);
	return efk;
}

void MEffekseer::StopEffect(int handle)
{
	if (!efkManager_->Exists(handle))
	{
		return;
	}
	efkManager_->StopEffect(handle);
	for (auto efk = playing_.begin(); efk != playing_.end();)
	{
		if (efk->handle_ == handle)
		{
			efk = playing_.erase(efk);
			break;
		}
		efk++;
	}
}

void MEffekseer::SetRotation(int handle, float x, float y, float z)
{
	if (!efkManager_->Exists(handle))
	{
		return;
	}
	efkManager_->SetRotation(handle, x, y, z);
	for (auto& efk : playing_)
	{
		if (efk.handle_ == handle)
		{
			efk.rx = x;
			efk.ry = y;
			efk.rz = z;
			break;
		}
	}
}

void MEffekseer::SetCoordinate(int handle, float x, float y, float z)
{
	if (!efkManager_->Exists(handle))
	{
		return;
	}
	efkManager_->SetLocation(handle, x, y, z);
	for (auto& efk : playing_)
	{
		if (efk.handle_ == handle)
		{
			efk.cx = x;
			efk.cy = y;
			efk.cz = z;
			break;
		}
	}
}

void MEffekseer::SetScale(int handle, float x, float y, float z)
{
	if (!efkManager_->Exists(handle))
	{
		return;
	}
	efkManager_->SetScale(handle, x, y, z);
	for (auto& efk : playing_)
	{
		if (efk.handle_ == handle)
		{
			efk.sx = x;
			efk.sy = y;
			efk.sz = z;
			break;
		}
	}
}

void MEffekseer::Update(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cList, int delta)
{
	for (auto efk = playing_.begin(); efk != playing_.end();)
	{
		if (!efkManager_->Exists(efk->handle_))
		{
			if (efk->loop_)
			{
				Effekseer::Handle e;
				e = efkManager_->Play(efk->effect_, efk->cx, efk->cy, efk->cz);
				efkManager_->SetLocation(e, efk->cx, efk->cy, efk->cz);
				efkManager_->SetRotation(e, efk->rx, efk->ry, efk->rz);
				efkManager_->SetScale(e, efk->sx, efk->sy, efk->sz);
			}
			else
			{
				efk = playing_.erase(efk);
			}
		}
		else
		{
			efk++;
		}
	}
	efkMemPool_->NewFrame();

	EffekseerRendererDX12::BeginCommandList(efkCommandList_, cList.Get());
	efkRenderer_->SetCommandList(efkCommandList_);

	efkManager_->Update(static_cast<float>(delta));
}

void MEffekseer::Draw(void)
{
	efkRenderer_->BeginRendering();
	efkManager_->Draw();
	efkRenderer_->EndRendering();

	efkRenderer_->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(efkCommandList_);
}
