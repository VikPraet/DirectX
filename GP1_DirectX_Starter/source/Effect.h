#pragma once
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* devicePtr, const std::wstring& path);
	~Effect();

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11EffectMatrixVariable* GetWorldViewProjMatrix() const { return m_WorldViewProjMatrixPtr; }
	ID3DX11EffectMatrixVariable* GetWorldMatrix() const { return m_WorldMatrixPtr; }
	ID3DX11EffectVectorVariable* GetCameraPos() const { return m_CameraPosPtr; }

	void SetDiffuseMap(const Texture* diffuseTexturePtr) const;
	void SetNormalMap(const Texture* normalTexturePtr) const;
	void SetSpecularMap(const Texture* specularTexturePtr) const;
	void SetGlossinessMap(const Texture* glossinessTexturePtr) const;

	void SetUseNormalMap(bool useNormalMap) const;

	void SetSamplerState(ID3D11Device* devicePtr, int state) const;

private:
	ID3DX11Effect* m_EffectPtr{};
	ID3DX11EffectTechnique* m_TechniquePtr{};

	ID3DX11EffectMatrixVariable* m_WorldViewProjMatrixPtr{};
	ID3DX11EffectMatrixVariable* m_WorldMatrixPtr{};

	ID3DX11EffectVectorVariable* m_CameraPosPtr{};

	ID3DX11EffectShaderResourceVariable* m_DiffuseMapVariablePtr{};
	ID3DX11EffectShaderResourceVariable* m_NormalMapVariablePtr{};
	ID3DX11EffectShaderResourceVariable* m_SpecularMapVariablePtr{};
	ID3DX11EffectShaderResourceVariable* m_GlossinessMapVariablePtr{};

	ID3DX11EffectSamplerVariable* m_SamplerStateVariablePtr{};

	ID3DX11EffectVariable* m_UseNormalMapVariablePtr{};
};

