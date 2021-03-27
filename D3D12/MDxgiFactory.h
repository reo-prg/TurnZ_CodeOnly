#pragma once
#include <d3dx12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

#define DxgiFactory MDxgiFactory::GetInstance().GetFactory()

class MDxgiFactory
{
public:
	static MDxgiFactory& GetInstance(void);
	void Initialize(void);

	const ComPtr<IDXGIFactory7>& GetFactory(void);
private:
	MDxgiFactory();
	~MDxgiFactory();	
	MDxgiFactory(const MDxgiFactory&) = delete;
	MDxgiFactory operator=(const MDxgiFactory&) = delete;

	ComPtr<IDXGIFactory7> factory_;
};

