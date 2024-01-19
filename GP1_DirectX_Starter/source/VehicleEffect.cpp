#include "pch.h"
#include "VehicleEffect.h"

VehicleEffect::VehicleEffect(ID3D11Device* devicePtr):
	BaseEffect(devicePtr, L"Resources/PosCol3D.fx")
{
	m_DiffuseMapVariablePtr = m_EffectPtr->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_DiffuseMapVariablePtr->IsValid())
	{
		std::wcout << L"DiffuseMapVariable not valid!\n";
	}

	m_NormalMapVariablePtr = m_EffectPtr->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_NormalMapVariablePtr->IsValid())
	{
		std::wcout << L"DiffuseMapVariable not valid!\n";
	}

	m_SpecularMapVariablePtr = m_EffectPtr->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_SpecularMapVariablePtr->IsValid())
	{
		std::wcout << L"DiffuseMapVariable not valid!\n";
	}

	m_GlossinessMapVariablePtr = m_EffectPtr->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_GlossinessMapVariablePtr->IsValid())
	{
		std::wcout << L"DiffuseMapVariable not valid!\n";
	}

	m_UseNormalMapVariablePtr = m_EffectPtr->GetVariableByName("gUseNormalMap")->AsScalar();
	if (!m_UseNormalMapVariablePtr->IsValid())
	{
		std::wcout << L"UseNormalMapVariable not valid!\n";
	}

	SetDiffuseMap( new Texture("Resources/vehicle_diffuse.png", devicePtr) );
	SetNormalMap( new Texture("Resources/vehicle_normal.png", devicePtr) );
	SetSpecularMap( new Texture("Resources/vehicle_specular.png", devicePtr) );
	SetGlossinessMap( new Texture("Resources/vehicle_gloss.png", devicePtr) );
}

VehicleEffect::~VehicleEffect()
{

}

void VehicleEffect::SetDiffuseMap(const Texture* diffuseTexturePtr) const
{
	if (m_DiffuseMapVariablePtr) m_DiffuseMapVariablePtr->SetResource(diffuseTexturePtr->GetResourceView());
	delete diffuseTexturePtr;
}

void VehicleEffect::SetNormalMap(const Texture* normalTexturePtr) const
{
	if (m_NormalMapVariablePtr) m_NormalMapVariablePtr->SetResource(normalTexturePtr->GetResourceView());
	delete normalTexturePtr;
}

void VehicleEffect::SetSpecularMap(const Texture* specularTexturePtr) const
{
	if (m_SpecularMapVariablePtr) m_SpecularMapVariablePtr->SetResource(specularTexturePtr->GetResourceView());
	delete specularTexturePtr;
}

void VehicleEffect::SetGlossinessMap(const Texture* glossinessTexturePtr) const
{
	if (m_GlossinessMapVariablePtr) m_GlossinessMapVariablePtr->SetResource(glossinessTexturePtr->GetResourceView());
	delete glossinessTexturePtr;
}

void VehicleEffect::SetUseNormalMap(bool useNormalMap) const
{
	if (m_UseNormalMapVariablePtr) m_UseNormalMapVariablePtr->AsScalar()->SetBool(useNormalMap);
}
