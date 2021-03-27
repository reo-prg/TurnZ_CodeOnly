#include "Object2DDrawer.h"
#include <d3dcompiler.h>
#include <algorithm>
#include <cassert>
#include "../../D3D12/MDx12Device.h"
#include "Object2D.h"

void Object2DDrawer::CreatePipelineState(void)
{
	HRESULT result;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};

	ID3DBlob* vertexShader = nullptr;
	ID3DBlob* pixelShader = nullptr;
	ID3DBlob* errorBlob = nullptr;

	D3D12_INPUT_ELEMENT_DESC ieDesc[] = {
	{
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}
	};

	gpsDesc.InputLayout.NumElements = _countof(ieDesc);
	gpsDesc.InputLayout.pInputElementDescs = ieDesc;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	result = D3DCompileFromFile(L"Shader/Object2D/Default2D_vs.hlsl", nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader, &errorBlob);
	assert(SUCCEEDED(result));

	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);

	result = D3DCompileFromFile(L"Shader/Object2D/Default2D_ps.hlsl", nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader, &errorBlob);
	assert(SUCCEEDED(result));

	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);

	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	D3D12_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = false;
	dsDesc.StencilEnable = false;
	gpsDesc.DepthStencilState = dsDesc;

	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	gpsDesc.BlendState.AlphaToCoverageEnable = false;
	gpsDesc.BlendState.IndependentBlendEnable = false;
	gpsDesc.BlendState.RenderTarget[0].BlendEnable = true;
	gpsDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	gpsDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	gpsDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gpsDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	gpsDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	gpsDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	gpsDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	gpsDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	gpsDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;

	gpsDesc.NodeMask = 0;


	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};
	ID3DBlob* rootSigBrob = nullptr;


	CD3DX12_DESCRIPTOR_RANGE dRange[3] = {};
	dRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	dRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	dRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rParam[3] = {};
	rParam[0].InitAsDescriptorTable(1, &dRange[0]);
	rParam[1].InitAsDescriptorTable(1, &dRange[1]);
	rParam[2].InitAsDescriptorTable(1, &dRange[2]);

	CD3DX12_STATIC_SAMPLER_DESC ssDesc[1] = {};
	ssDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	ssDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	ssDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	ssDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	ssDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

	rsDesc.Init(_countof(rParam), rParam, _countof(ssDesc), ssDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	result = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBrob, &errorBlob);
	assert(SUCCEEDED(result));

	result = DxDevice->CreateRootSignature(0, rootSigBrob->GetBufferPointer(),
		rootSigBrob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
	assert(SUCCEEDED(result));

	gpsDesc.pRootSignature = rootSignature_.Get();

	result = DxDevice->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(pipelineState_.GetAddressOf()));
	assert(SUCCEEDED(result));
}

void Object2DDrawer::SetPipelineState(ID3D12GraphicsCommandList* cList)
{
	cList->SetPipelineState(pipelineState_.Get());
	cList->SetGraphicsRootSignature(rootSignature_.Get());
}

void Object2DDrawer::DrawBG(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans)
{
	auto it = std::remove_if(objects_.begin(), objects_.end(), [](std::weak_ptr<Object2D> obj) { return obj.expired(); });
	if (it != objects_.end())
	{
		objects_.erase(it, objects_.end());
	}
	std::sort(objects_.begin(), objects_.end(),
		[](const std::weak_ptr<Object2D>& obj1, const std::weak_ptr<Object2D>& obj2)
		{ return obj1.lock()->zBuffer_ > obj2.lock()->zBuffer_; });
	for (auto& obj : objects_)
	{
		if (!obj.lock()->isBackGround_) { continue; }
		obj.lock()->Draw(cList, trans);
	}
}

void Object2DDrawer::Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* trans)
{
	auto it = std::remove_if(objects_.begin(), objects_.end(), [](const std::weak_ptr<Object2D>& obj) { return obj.expired(); });
	if (it != objects_.end())
	{
		objects_.erase(it, objects_.end());
	}
	std::sort(objects_.begin(), objects_.end(),
		[](const std::weak_ptr<Object2D>& obj1, const std::weak_ptr<Object2D>& obj2)
		{ return obj1.lock()->zBuffer_ > obj2.lock()->zBuffer_; });
	for (auto& obj : objects_)
	{
		if (obj.lock()->isBackGround_) { continue; }
		obj.lock()->Draw(cList, trans);
	}
}

void Object2DDrawer::RegisterObject(const std::shared_ptr<Object2D>& object)
{
	objects_.push_back(object);
}

void Object2DDrawer::UnregisterObject(const std::shared_ptr<Object2D>& object)
{
	auto it = std::remove_if(objects_.begin(), objects_.end(),
		[&object](const std::weak_ptr<Object2D>& obj) { return obj.lock() == object; });
	if (it == objects_.end()) { return; }
	objects_.erase(it, objects_.end());
}
