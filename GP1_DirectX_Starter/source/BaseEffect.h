#pragma once
class BaseEffect
{
public:
	BaseEffect(ID3D11Device* devicePtr, const std::wstring& path);
	~BaseEffect();

	ID3DX11Effect* GetEffect() const { return m_EffectPtr; };
	ID3DX11EffectTechnique* GetTechnique() const { return m_TechniquePtr; }
	ID3DX11EffectVectorVariable* GetCameraPos() const { return m_CameraPosPtr; }

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11EffectMatrixVariable* GetWorldViewProjMatrix() const { return m_WorldViewProjMatrixPtr; }
	ID3DX11EffectMatrixVariable* GetWorldMatrix() const { return m_WorldMatrixPtr; }

	void SetSamplerState(ID3D11Device* devicePtr, int state) const;

protected:
	ID3DX11Effect* m_EffectPtr{};
	ID3DX11EffectTechnique* m_TechniquePtr{};
	ID3DX11EffectVectorVariable* m_CameraPosPtr{};

	ID3DX11EffectMatrixVariable* m_WorldViewProjMatrixPtr{};
	ID3DX11EffectMatrixVariable* m_WorldMatrixPtr{};

	ID3DX11EffectSamplerVariable* m_SamplerStateVariablePtr{};
};
