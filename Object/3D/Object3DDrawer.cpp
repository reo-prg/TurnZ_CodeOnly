#include "Object3DDrawer.h"
#include <d3dcompiler.h>
#include <cassert>
#include "../../D3D12/MDx12Device.h"
#include "Object3D.h"



void Object3DDrawer::CreatePipelineState(void)
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
		"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}
	};

	gpsDesc.InputLayout.NumElements = _countof(ieDesc);
	gpsDesc.InputLayout.pInputElementDescs = ieDesc;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// 頂点シェーダのコンパイル

	//result = D3DCompileFromFile(L"Shader/Object3D/Default3D_vs.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_1",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader, &errorBlob);
	result = D3DReadFileToBlob(L"Resource/Shader/Default3D_vs.cso", &vertexShader);
	assert(SUCCEEDED(result));

	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);

	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	// ピクセルシェーダのコンパイル
	//result = D3DCompileFromFile(L"Shader/Object3D/Default3D_ps.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_1",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader, &errorBlob);
	result = D3DReadFileToBlob(L"Resource/Shader/Default3D_ps.cso", &pixelShader);
	assert(SUCCEEDED(result));

	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);

	gpsDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	gpsDesc.NodeMask = 0;

	// ルートシグネチャ
	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};
	ID3DBlob* rootSigBrob = nullptr;

	CD3DX12_DESCRIPTOR_RANGE dRange[2] = {};
	dRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	dRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	CD3DX12_ROOT_PARAMETER rParam[2] = {};
	rParam[0].InitAsDescriptorTable(1, &dRange[0]);
	rParam[1].InitAsDescriptorTable(1, &dRange[1]);

	CD3DX12_STATIC_SAMPLER_DESC ssDesc[1] = {};
	ssDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	ssDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

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

void Object3DDrawer::SetPipelineState(ID3D12GraphicsCommandList* cList)
{
	cList->SetPipelineState(pipelineState_.Get());
	cList->SetGraphicsRootSignature(rootSignature_.Get());
}

void Object3DDrawer::Draw(ID3D12GraphicsCommandList* cList, ID3D12DescriptorHeap* wvp)
{
	auto it = std::remove_if(objects_.begin(), objects_.end(), [](std::weak_ptr<Object3D> obj) { return obj.expired(); });
	objects_.erase(it, objects_.end());
	for (auto& obj : objects_)
	{
		obj.lock()->Draw(cList, wvp);
	}
}

void Object3DDrawer::RegisterObject(const std::shared_ptr<Object3D>& object)
{
	objects_.push_back(object);
}

void Object3DDrawer::UnregisterObject(const std::shared_ptr<Object3D>& object)
{
	auto it = std::remove_if(objects_.begin(), objects_.end(),
		[&object](const std::weak_ptr<Object3D>& obj) { return obj.lock() == object; });
	objects_.erase(it, objects_.end());
}
