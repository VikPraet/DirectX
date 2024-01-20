#include "pch.h"
#include "FireFXEffect.h"

FireFXEffect::FireFXEffect(ID3D11Device* devicePtr) :
	BaseEffect(devicePtr, L"Resources/PartialCoverage.fx")
{
	m_DiffuseMapVariablePtr = m_EffectPtr->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_DiffuseMapVariablePtr->IsValid())
	{
		std::wcout << L"DiffuseMapVariable not valid!\n";
	}

	SetDiffuseMap(new Texture("Resources/fireFX_diffuse.png", devicePtr));
}

FireFXEffect::~FireFXEffect()
{
}

void FireFXEffect::SetDiffuseMap(const Texture* diffuseTexturePtr) const
{
	if (m_DiffuseMapVariablePtr) m_DiffuseMapVariablePtr->SetResource(diffuseTexturePtr->GetResourceView());
	delete diffuseTexturePtr;
}