#pragma once
#include "Effect.h"

class Mesh
{
public:
	struct Vertex
	{
		dae::Vector3 Position;
		dae::Vector3 Color;
		dae::Vector2 UV;
	};
	Mesh(ID3D11Device* devicePtr, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	~Mesh();

	void Render(ID3D11DeviceContext* deviceContextPtr, const float* dataPtr);

	dae::Matrix& GetWorldMatrix() { return m_WorldMatrix; }
	Effect* GetEffectPtr() const { return m_EffectPtr; }
private:
	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

	Effect* m_EffectPtr{};
	ID3D11Buffer* m_VertexBufferPtr{};
	ID3D11Buffer* m_IndexBufferPtr{};
	ID3D11InputLayout* m_InputLayout{};
	int m_NumIndices{};

	Texture* m_TexturePtr{};

	dae::Matrix m_WorldMatrix{
		{1.0f,	0.0f,	0.0f},
		{0.0f,	1.0f,	0.0f},
		{0.0f,	0.0f,	1.0f},
		   {0.0f,	0.0f,	0.0f}
	};
};