#pragma once
#include "BaseEffect.h"
#include "Texture.h"

class VehicleEffect : public BaseEffect
{
public:
	VehicleEffect(ID3D11Device* devicePtr);
	~VehicleEffect();

	void SetDiffuseMap(const Texture* diffuseTexturePtr) const;
	void SetNormalMap(const Texture* normalTexturePtr) const;
	void SetSpecularMap(const Texture* specularTexturePtr) const;
	void SetGlossinessMap(const Texture* glossinessTexturePtr) const;

	void SetUseNormalMap(bool useNormalMap) const;

private:
	ID3DX11EffectShaderResourceVariable* m_DiffuseMapVariablePtr{};
	ID3DX11EffectShaderResourceVariable* m_NormalMapVariablePtr{};
	ID3DX11EffectShaderResourceVariable* m_SpecularMapVariablePtr{};
	ID3DX11EffectShaderResourceVariable* m_GlossinessMapVariablePtr{};

	ID3DX11EffectVariable* m_UseNormalMapVariablePtr{};
};

