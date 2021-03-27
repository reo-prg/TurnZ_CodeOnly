#include "MDxgiFactory.h"

MDxgiFactory& MDxgiFactory::GetInstance(void)
{
	static MDxgiFactory f;
	return f;
}

const ComPtr<IDXGIFactory7>& MDxgiFactory::GetFactory(void)
{
	if (factory_ == nullptr)
	{
		Initialize();
	}
	return factory_;
}

void MDxgiFactory::Initialize(void)
{
	try
	{ 
		if (factory_ != nullptr)
		{
			OutputDebugString(L"DxgiFactory has initialize multiply\n");
			return;
		}

		HRESULT result;
	#if _DEBUG
		result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory_.GetAddressOf()));
	#else
		result = CreateDXGIFactory2(0, IID_PPV_ARGS(factory_.GetAddressOf()));
	#endif
		if (FAILED(result))
		{
			throw L"can not create dxgi factory";
		}
	}
	catch (LPCWSTR desc)
	{
		OutputDebugString(desc);
		terminate();
	}
}

MDxgiFactory::MDxgiFactory():factory_(nullptr)
{
}

MDxgiFactory::~MDxgiFactory()
{
}
