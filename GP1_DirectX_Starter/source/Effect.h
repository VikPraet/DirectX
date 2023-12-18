#pragma once
class Effect
{
public:
	Effect(ID3D11Device* devicePtr, const std::wstring& path);
	~Effect();

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11EffectMatrixVariable* GetWorldViewProjMatrix() const { return m_WorldViewProjMatrixPtr; }

	void UpdateWorldViewProjectionMatrix(dae::Matrix& worldViewProjMatrix) const;
private:
	ID3DX11Effect* m_EffectPtr{};
	ID3DX11EffectTechnique* m_TechniquePtr{};

	ID3DX11EffectMatrixVariable* m_WorldViewProjMatrixPtr{};
};

