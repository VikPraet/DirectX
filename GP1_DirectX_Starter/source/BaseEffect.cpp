#include "pch.h"
#include "BaseEffect.h"

BaseEffect::BaseEffect(ID3D11Device* devicePtr, const std::wstring& path)
{
	m_EffectPtr = LoadEffect(devicePtr, path);

	m_TechniquePtr = m_EffectPtr->GetTechniqueByName("DefaultTechnique");
	if (!m_TechniquePtr->IsValid()) std::wcout << L"Technique not valid\n";

	m_WorldViewProjMatrixPtr = m_EffectPtr->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_WorldViewProjMatrixPtr->IsValid())
	{
		std::wcout << L"WorldViewProjMatrixVariable not valid!\n";
	}

	m_WorldMatrixPtr = m_EffectPtr->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_WorldMatrixPtr->IsValid())
	{
		std::wcout << L"WorldMatrixVariable not valid!\n";
	}

	m_CameraPosPtr = m_EffectPtr->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_CameraPosPtr->IsValid())
	{
		std::wcout << L"CameraPos not valid!\n";
	}

	m_SamplerStateVariablePtr = m_EffectPtr->GetVariableByName("gSamplerState")->AsSampler();
	if (!m_SamplerStateVariablePtr->IsValid())
	{
		std::wcout << L"SamplerStateVariablePtr not valid!\n";
	}
}

BaseEffect::~BaseEffect()
{
	if (m_EffectPtr)
	{
		m_EffectPtr->Release();
	}
}

ID3DX11Effect* BaseEffect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* errorBlobPtr{ nullptr };
	ID3DX11Effect* effectPtr{ nullptr };

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&effectPtr,
		&errorBlobPtr);

	if (FAILED(result))
	{
		if (errorBlobPtr != nullptr)
		{
			const char* pErrors = static_cast<char*>(errorBlobPtr->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < errorBlobPtr->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			errorBlobPtr->Release();
			errorBlobPtr = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return effectPtr;
}

void BaseEffect::SetSamplerState(ID3D11Device* devicePtr, int state) const
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	// set console textColor to ...
	const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 0x0c);

	switch (state)
	{
	case 0:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		std::wcout << L"SamplerState Set: POINT\n";
		break;
	case 1:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		std::wcout << L"SamplerState Set: LINEAR\n";
		break;
	case 2:
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.MaxAnisotropy = 16;
		std::wcout << L"SamplerState Set: ANISOTROPIC\n";
		break;
	default:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		std::wcout << L"Default SamplerState Set: PointFiler\n";
		break;
	}

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create sampler state
	ID3D11SamplerState* samplerState{ nullptr };
	if (const HRESULT hr = devicePtr->CreateSamplerState(&samplerDesc, &samplerState); FAILED(hr))
	{
		std::wcout << L"CreateSamplerSate failed\n";
		return;
	}

	m_SamplerStateVariablePtr->SetSampler(0, samplerState);

	if (samplerState) samplerState->Release();

	// set console textColor to white
	SetConsoleTextAttribute(hConsole, 0x07);
}
