#pragma once
#include <d3d12.h>
#include <d3dx12.h>

using namespace Microsoft::WRL;

#define DxDevice MDx12Device::GetInstance().GetDevice()

class MDx12Device
{
public:
	static MDx12Device& GetInstance(void);
	void Initialize(void);

	const ComPtr<ID3D12Device>& GetDevice(void);
private:
	MDx12Device();
	~MDx12Device();
	MDx12Device(const MDx12Device&) = delete;
	MDx12Device operator=(const MDx12Device&) = delete;

	ComPtr<ID3D12Device> device_;
};

