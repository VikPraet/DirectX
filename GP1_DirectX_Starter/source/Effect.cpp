#include "pch.h"
#include "Effect.h"

Effect::Effect(ID3D11Device* devicePtr, const std::wstring& path)
{
	m_EffectPtr = LoadEffect(devicePtr, path);

	m_TechniquePtr = m_EffectPtr->GetTechniqueByName("DefaultTechnique");
	if (!m_TechniquePtr->IsValid()) std::wcout << L"Technique not valid\n";

	m_WorldViewProjMatrixPtr = m_EffectPtr->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_WorldViewProjMatrixPtr->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";
	}
}

Effect::~Effect()
{
	if (m_EffectPtr)
	{
		m_EffectPtr->Release();
	}
	if (m_TechniquePtr)
	{
		m_TechniquePtr->Release();
	}
	if(m_WorldViewProjMatrixPtr)
	{
		m_WorldViewProjMatrixPtr->Release();
	}
}

ID3DX11Effect* Effect::GetEffect() const
{
	return m_EffectPtr;
}

ID3DX11EffectTechnique* Effect::GetTechnique() const
{
	return m_TechniquePtr;
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
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

void Effect::UpdateWorldViewProjectionMatrix(dae::Matrix& worldViewProjMatrix) const
{
	m_WorldViewProjMatrixPtr->SetMatrix(reinterpret_cast<float*>(&worldViewProjMatrix));
}
