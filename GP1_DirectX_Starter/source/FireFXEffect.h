#pragma once
#include "BaseEffect.h"
#include "Texture.h"

class FireFXEffect : public BaseEffect
{
public:
	FireFXEffect(ID3D11Device* devicePtr);
	~FireFXEffect();

	void SetDiffuseMap(const Texture* diffuseTexturePtr) const;

private:
	ID3DX11EffectShaderResourceVariable* m_DiffuseMapVariablePtr{};
};

