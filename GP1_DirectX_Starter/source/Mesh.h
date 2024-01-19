#pragma once
#include "VehicleEffect.h"

class Mesh
{
public:
	struct Vertex
	{
		dae::Vector3 position;
		dae::Vector3 color;
		dae::Vector2 uv;
		dae::Vector3 normal;
		dae::Vector3 tangent;
	};
	Mesh(ID3D11Device* devicePtr, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, BaseEffect* effect);
	~Mesh();

	void Render(ID3D11DeviceContext* deviceContextPtr, const float* dataPtr);

	dae::Matrix& GetWorldMatrix() { return m_WorldMatrix; }
	BaseEffect* GetEffectPtr() const { return m_EffectPtr; }
private:
	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

	BaseEffect* m_EffectPtr;

	ID3D11Buffer* m_VertexBufferPtr{};
	ID3D11Buffer* m_IndexBufferPtr{};
	ID3D11InputLayout* m_InputLayout{};
	int m_NumIndices{};

	dae::Matrix m_WorldMatrix{
		{1.0f,	0.0f,	0.0f},
		{0.0f,	1.0f,	0.0f},
		{0.0f,	0.0f,	1.0f},
		{0.0f,	0.0f,	0.0f}
	};
};