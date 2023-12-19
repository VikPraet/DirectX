#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* devicePtr, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& texturePath)
	: m_Vertices{ vertices }
	, m_Indices{ indices }
	, m_EffectPtr{ new Effect(devicePtr, L"Resources/PosCol3D.fx") }
	, m_DiffuseMapPtr{ new Texture(texturePath, devicePtr) }
	, m_NormalMapPtr{ new Texture("Resources/vehicle_normal.png", devicePtr) }
	, m_SpecularMapPtr{ new Texture("Resources/vehicle_specular.png", devicePtr) }
	, m_GlossinessMapPtr{ new Texture("Resources/vehicle_gloss.png", devicePtr) }
{
	m_EffectPtr->SetDiffuseMap(m_DiffuseMapPtr);
	m_EffectPtr->SetNormalMap(m_NormalMapPtr);
	m_EffectPtr->SetSpecularMap(m_SpecularMapPtr);
	m_EffectPtr->SetGlossinessMap(m_GlossinessMapPtr);

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 36;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 48;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_EffectPtr->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = devicePtr->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_InputLayout);

	if (FAILED(result)) return;

	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(m_Vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_Vertices.data();

	result = devicePtr->CreateBuffer(&bd, &initData, &m_VertexBufferPtr);
	if (FAILED(result)) return;

	//Create index buffer
	m_NumIndices = static_cast<uint32_t>(m_Indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_Indices.data();
	result = devicePtr->CreateBuffer(&bd, &initData, &m_IndexBufferPtr);
	if (FAILED(result)) return;



}

Mesh::~Mesh()
{
	delete m_EffectPtr;
	m_EffectPtr = nullptr;

	delete m_DiffuseMapPtr;
	m_DiffuseMapPtr = nullptr;

	delete m_NormalMapPtr;
	m_NormalMapPtr = nullptr;

	delete m_SpecularMapPtr;
	m_SpecularMapPtr = nullptr;

	delete m_GlossinessMapPtr;
	m_GlossinessMapPtr = nullptr;

	if (m_InputLayout)
	{
		m_InputLayout->Release();
	}
	if (m_VertexBufferPtr)
	{
		m_VertexBufferPtr->Release();
	}
	if (m_IndexBufferPtr)
	{
		m_IndexBufferPtr->Release();
	}
}

void Mesh::Render(ID3D11DeviceContext* deviceContextPtr, const float* dataPtr)
{
	//1. Set Primitive Topology
	deviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set Input Layout
	deviceContextPtr->IASetInputLayout(m_InputLayout);

	m_EffectPtr->GetWorldViewProjMatrix()->SetMatrix(dataPtr);

	//3. Set VertexBuffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	deviceContextPtr->IASetVertexBuffers(0, 1, &m_VertexBufferPtr, &stride, &offset);

	//4. Set IndexBuffer
	deviceContextPtr->IASetIndexBuffer(m_IndexBufferPtr, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_EffectPtr->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_EffectPtr->GetTechnique()->GetPassByIndex(p)->Apply(0, deviceContextPtr);
		deviceContextPtr->DrawIndexed(m_NumIndices,  0, 0);
	}
}
