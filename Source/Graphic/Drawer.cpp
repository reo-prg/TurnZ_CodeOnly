#include "Drawer.h"
#include <cassert>
#include <random>
#include <dxgi1_6.h>
#include "../Application.h"
#include "DrawCharacter.h"
#include "MEffekseer.h"
#include "../D3D12/MDx12CommandQueue.h"
#include "../D3D12/MDx12CommandList.h"
#include "../D3D12/MDx12DepthStencil.h"
#include "../D3D12/MDx12Device.h"
#include "../D3D12/MDx12Fence.h"
#include "../D3D12/MDxgiSwapChain.h"
#include "../Object/2D/Object2DDrawer.h"
#include "../Object/2D/Object2D.h"
#include "../Object/2D/Square2D.h"
#include "../Object/2D/Object2DContainer.h"
#include "../Object/3D/Object3DDrawer.h"
#include "../Object/3D/Object3D.h"
#include "../Graphic/ImageManager.h"
#include "../Graphic/TransformCBV.h"

using namespace DirectX;
namespace
{
	std::mt19937 mt_;
}

Drawer::Drawer()
{
	imagemng_.reset(new ImageManager());
	object2D_.reset(new Object2DDrawer());
	object3D_.reset(new Object3DDrawer());
	wvp_.reset(new TransformCBV());
	chr_.reset(new DrawCharacter());
	std::random_device rd;
	mt_ = std::mt19937(rd());
}

Drawer::~Drawer()
{
	chr_.reset();
	object2D_.reset();
	object3D_.reset();
	wvp_.reset();
	imagemng_.reset();
}

void Drawer::ChStart(void)
{
	std::uniform_real_distribution<float> rd(-3.0f, 3.0f);
	wvp_->SetCameraCoordinate({ rd(mt_), rd(mt_)});
	changeCoordX_ += speed_;
	player_->AddCoordinate(speed_, 0.0f);
	overwrap_->AddCoordinate(speed_, 0.0f);
	if (changeCoordX_ >= scrWidth_ * 2.0f)
	{
		wvp_->SetCameraCoordinate({ 0.0f, 0.0f });
		changeUpdater_ = &Drawer::ChIdle;
		state_ = ChangeState::Playing;
	}
}

void Drawer::ChIdle(void)
{
	if (change_)
	{
		if (state_ == ChangeState::Ready)
		{
			changeUpdater_ = &Drawer::ChStart;
			player_->SetRotation(XM_PIDIV2);
			drawChangeEfk_ = true;
		}
		if (state_ == ChangeState::Playing)
		{
			player_->SetRotation(-XM_PIDIV2);
			overwrap_->AddCoordinate((scrdiff_ - (plSize_ - scrdiff_)) * plScale_, 0.0f);
			changeUpdater_ = &Drawer::ChEnd;
		}
		change_ = false;
	}
}

void Drawer::ChEnd(void)
{
	std::uniform_real_distribution<float> rd(-3.0f, 3.0f);
	wvp_->SetCameraCoordinate({ rd(mt_), rd(mt_) });
	changeCoordX_ -= speed_;
	player_->AddCoordinate(-speed_, 0.0f);
	overwrap_->AddCoordinate(-speed_, 0.0f);
	if (changeCoordX_ <= 0)
	{
		wvp_->SetCameraCoordinate({ 0.0f, 0.0f });
		changeUpdater_ = &Drawer::ChIdle;
		state_ = ChangeState::Ready;
		drawChangeEfk_ = false;
		overwrap_->AddCoordinate(-(scrdiff_ - (plSize_ - scrdiff_)) * plScale_, 0.0f);
	}
}

Drawer& Drawer::GetInstance(void)
{
	static Drawer d;
	return d;
}

bool Drawer::Initialize(unsigned int width, unsigned int height)
{
	static bool isCalled = false;
	if (isCalled)
	{
		return false;
	}
	isCalled = true;
	if (!CreateSwapBuffer()) { return false; }
	object2D_->CreatePipelineState();
	object3D_->CreatePipelineState();
	wvp_->Initialize(width, height);
	CreateBasicObjects(width, height);
	state_ = ChangeState::Ready;
	changeUpdater_ = &Drawer::ChIdle;
	change_ = false;
	return true;
}

void Drawer::Update(void)
{
	(this->*changeUpdater_)();
}

void Drawer::Draw(void)
{
	HRESULT result;

	SetViewProjToEffekseer();

	D3D12_CPU_DESCRIPTOR_HANDLE baseBufferHandle = baseHeap_->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE depthstencilHandle = DxDSHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int currentBuffer = DxgiSwapChain->GetCurrentBackBufferIndex();
	baseBufferHandle.ptr += (currentBuffer * DxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	D3D12_CPU_DESCRIPTOR_HANDLE depthHandle = DxDSHeap->GetCPUDescriptorHandleForHeapStart();

	ID3D12CommandAllocator* rootCommandAllocator = RootCmAllocator.Get();
	ID3D12GraphicsCommandList* rootCommandList = RootCmList.Get();

	result = rootCommandAllocator->Reset();
	assert(SUCCEEDED(result));
	result = rootCommandList->Reset(rootCommandAllocator, nullptr);
	assert(SUCCEEDED(result));

	CD3DX12_RESOURCE_BARRIER rBar = CD3DX12_RESOURCE_BARRIER::Transition(baseBuffers_[currentBuffer].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	rootCommandList->ResourceBarrier(1, &rBar);

	rootCommandList->OMSetRenderTargets(1, &baseBufferHandle, false, nullptr);
	rootCommandList->ClearRenderTargetView(baseBufferHandle, clearColor, 0, nullptr);
	//rootCommandList->ClearDepthStencilView(DxDSHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	//	1.0f, 0, 0, nullptr);
	unsigned int width, height;
	Application::GetInstance().GetWindowSize(width, height);
	CD3DX12_VIEWPORT vp(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
	rootCommandList->RSSetViewports(1, &vp);
	CD3DX12_RECT rect(0, 0, width, height);
	rootCommandList->RSSetScissorRects(1, &rect);

	// background--------------------------------------------
	object2D_->SetPipelineState(rootCommandList);
	object2D_->DrawBG(rootCommandList, wvp_->Get2DDescriptorHeap().Get());
	// 3D ---------------------------------------------------
	//rootCommandList->OMSetRenderTargets(1, &baseBufferHandle, false, &depthstencilHandle);
	//rootCommandList->ClearDepthStencilView(DxDSHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH,
	//	1.0f, 0, 0, nullptr);
	//object3D_->SetPipelineState(rootCommandList);
	//object3D_->Draw(rootCommandList, wvp_->GetDescriptorHeap().Get());

	// 2D ---------------------------------------------------
	//rootCommandList->OMSetRenderTargets(1, &baseBufferHandle, false, nullptr);
	//object2D_->SetPipelineState(rootCommandList);
	object2D_->Draw(rootCommandList, wvp_->Get2DDescriptorHeap().Get());
	// •¶Žš ------------------------------------------------------
	//rootCommandList->ClearDepthStencilView(DxDSHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH,
	//	1.0f, 0, 0, nullptr);
	object3D_->SetPipelineState(rootCommandList);
	EfkIns.Draw();

	chr_->Draw(rootCommandList, wvp_->GetTrans2DMatrix()->camera_);

	//rootCommandList->ClearDepthStencilView(DxDSHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
	//	1.0f, 0, 0, nullptr);
	object2D_->SetPipelineState(rootCommandList);
	DrawChangeEffect_();


	rBar = CD3DX12_RESOURCE_BARRIER::Transition(baseBuffers_[currentBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	rootCommandList->ResourceBarrier(1, &rBar);

	rootCommandList->Close();
	ExecuteCommand();
	DxgiSwapChain->Present(0, 0);

	chr_->Commit();
}

const std::unique_ptr<ImageManager>& Drawer::GetImageManager(void)
{
	if (!imagemng_)
	{
		OutputDebugString(L"Drawer : ImageManager is removed");
		imagemng_.reset(new ImageManager());
	}
	return imagemng_;
}

const std::unique_ptr<Object3DDrawer>& Drawer::GetObjectDrawer(void)
{
	if (!object3D_)
	{
		OutputDebugString(L"Drawer : object3D is removed");
		object3D_.reset(new Object3DDrawer());
	}
	return object3D_;
}

const std::unique_ptr<Object2DDrawer>& Drawer::GetObject2DDrawer(void)
{
	if (!object2D_)
	{
		OutputDebugString(L"Drawer : object2D is removed");
		object2D_.reset(new Object2DDrawer());
	}
	return object2D_;
}

const std::unique_ptr<DrawCharacter>& Drawer::GetCharacterDrawer(void)
{
	if (!chr_)
	{
		OutputDebugString(L"Drawer : DrawCharacter is removed");
		chr_.reset(new DrawCharacter());
	}
	return chr_;
}

void Drawer::RegisterObject(const std::shared_ptr<Object2D>& object)
{
	object2D_->RegisterObject(object);
}

void Drawer::RegisterObject(const std::shared_ptr<Object3D>& object)
{
	object3D_->RegisterObject(object);
}

void Drawer::UnregisterObject(const std::shared_ptr<Object2D>& object)
{
	object2D_->UnregisterObject(object);
}

void Drawer::UnregisterObject(const std::shared_ptr<Object3D>& object)
{
	object3D_->UnregisterObject(object);
}

void Drawer::changeEffect(void)
{
	change_ = true;
}

ChangeState Drawer::GetChangeState(void)
{
	return state_;
}

void Drawer::SetViewProjToEffekseer(void)
{
	const TransMatrix* tr = wvp_->GetTransMatrix();
	EfkIns.SetCamera(tr->projection, tr->view);
}

void Drawer::CreateBasicObjects(unsigned int width, unsigned int height)
{
	float w, h;
	w = static_cast<float>(width);
	h = static_cast<float>(height);
	scrWidth_ = w;
	plScale_ = w / 128.0f;
	overwrap_ = new Square2D();
	overwrap_->CreateObject(-w - scrdiff_ * plScale_, h / 2.0f, w * 2.0f, h, { 0.5f, 0.5f, 1.0f, 1.0f });
	overwrap_->zBuffer_ = -10000;

	player_ = new Square2D();
	player_->CreateObject("Resource/Image/change.png", -w / 2.0f, h / 2.0f, w, w,
		XMMatrixRotationZ(-XM_PIDIV2));
	player_->zBuffer_ = -10001;

	changeCoordX_ = 0.0f;
	drawChangeEfk_ = false;
}

bool Drawer::CreateSwapBuffer(void)
{
	// Create swapchain's buffer
	HRESULT result;

	DXGI_SWAP_CHAIN_DESC1 swapDesc_ = {};
	DxgiSwapChain->GetDesc1(&swapDesc_);
	unsigned int bufferCount = swapDesc_.BufferCount;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = bufferCount;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	result = DxDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(baseHeap_.GetAddressOf()));
	assert(SUCCEEDED(result));
	if (FAILED(result)) { return false; }

	baseBuffers_.resize(bufferCount, nullptr);
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = baseHeap_->GetCPUDescriptorHandleForHeapStart();
	unsigned int increment = DxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (unsigned int i = 0; i < bufferCount; i++)
	{
		result = DxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(baseBuffers_[i].GetAddressOf()));
		assert(SUCCEEDED(result));
		if (FAILED(result)) { return false; }
		DxDevice->CreateRenderTargetView(baseBuffers_[i].Get(), &rtvDesc, handle);
		handle.ptr += increment;
	}

	return true;
}

void Drawer::ExecuteCommand(void)
{
	ID3D12CommandQueue* cQueue = RootCmQueue.Get();
	cQueue->ExecuteCommandLists(1, (ID3D12CommandList**)RootCmList.GetAddressOf());
	MDx12Fence::GetInstance().WaitSignal();
}

void Drawer::DrawChangeEffect_(void)
{
	if (!drawChangeEfk_) { return; }
	overwrap_->Draw(RootCmList.Get(), wvp_->Get2DDescriptorHeap().Get());
	player_->Draw(RootCmList.Get(), wvp_->Get2DDescriptorHeap().Get());
}
