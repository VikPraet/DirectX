#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Utils.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_WindowPtr{ pWindow }
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);


		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		// initialize camera
		m_CameraPtr = new Camera({ 0,0,-50 }, 45.f, static_cast<float>(m_Width) / static_cast<float>(m_Height), m_VehiclePos);

		// initialize vehicle object
		std::vector<Mesh::Vertex> verticesVehicle{ };
		std::vector<uint32_t> indicesVehicle{ };

		if(Utils::ParseOBJ("Resources/vehicle.obj", verticesVehicle, indicesVehicle))
		{
			Mesh* vehicleMeshPtr = new Mesh(m_DevicePtr, verticesVehicle, indicesVehicle);
			m_MeshesPtr.push_back(vehicleMeshPtr);
		}

		const Matrix TMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, m_VehiclePos };
		m_MeshesPtr[0]->GetWorldMatrix() *= TMatrix;
	}

	Renderer::~Renderer()
	{
		delete m_CameraPtr;
		m_CameraPtr = nullptr;

		for(int i{}; m_MeshesPtr.size() > i; ++i)
		{
			delete m_MeshesPtr[i];
			m_MeshesPtr[i] = nullptr;
		}

		if (m_DevicePtr)
		{
			m_DevicePtr->Release();
		}

		if(m_DeviceContextPtr)
		{
			m_DeviceContextPtr->ClearState();
			m_DeviceContextPtr->Flush();
			m_DeviceContextPtr->Release();
		}

		if (m_SwapChainPtr)
		{
			m_SwapChainPtr->Release();
		}

		if (m_DepthStencilBufferPtr)
		{
			m_DepthStencilBufferPtr->Release();
		}

		if (m_DepthStencilViewPtr)
		{
			m_DepthStencilViewPtr->Release();
		}

		if (m_RenderTargetBufferPtr)
		{
			m_RenderTargetBufferPtr->Release();
		}

		if (m_RenderTargetViewPtr)
		{
			m_RenderTargetViewPtr->Release();
		}
	}

	void Renderer::Update(const Timer* pTimer) const
	{
		m_CameraPtr->Update(pTimer);

		for (int i{}; m_MeshesPtr.size() > i; ++i)
		{
			m_MeshesPtr[i]->GetEffectPtr()->GetCameraPos()->SetFloatVector(reinterpret_cast<float*>(&m_CameraPtr->GetOrigin()));
		}

		if (m_CanRotate)
		{
			// rotate mesh 0 (vehicle)

			constexpr float rotationSpeed = 1.0f; // in radians per second
			const float rotationAngle = pTimer->GetElapsed() * rotationSpeed;

			// translation to world origin
			const Matrix translationToOrigin = Matrix::CreateTranslation(-m_VehiclePos.x, -m_VehiclePos.y, -m_VehiclePos.z);

			// Rotation around the y-axis
			const Matrix rotationMatrix = Matrix::CreateRotation(0, rotationAngle, 0);

			// translation back to objectPos
			const Matrix translationBack = Matrix::CreateTranslation(m_VehiclePos.x, m_VehiclePos.y, m_VehiclePos.z);

			// Combine the matrices
			m_MeshesPtr[0]->GetWorldMatrix() *= translationToOrigin * rotationMatrix * translationBack;
		}
	}

	void Renderer::Render() const
	{
		if (!m_IsInitialized) return;

		// clear RTV & DSV
		constexpr float color[4] = {0.39f, 0.59f, 0.93f, 1.f};
		m_DeviceContextPtr->ClearRenderTargetView(m_RenderTargetViewPtr, color);
		m_DeviceContextPtr->ClearDepthStencilView(m_DepthStencilViewPtr, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		// set pipeline + invoke draw calls (= render)
		for (int i{}; m_MeshesPtr.size() > i; ++i)
		{
			Matrix worldViewProjectionMatrix{ m_MeshesPtr[i]->GetWorldMatrix() * m_CameraPtr->GetInvViewMatrix() * m_CameraPtr->GetProjectionMatrix()};
			m_MeshesPtr[i]->Render(m_DeviceContextPtr, reinterpret_cast<float*>(&worldViewProjectionMatrix));
		}

		// present back buffer (swap)
		m_SwapChainPtr->Present(0, 0);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		// create device & deviceContext
		constexpr D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel,
			1, D3D11_SDK_VERSION, &m_DevicePtr, nullptr, &m_DeviceContextPtr);
		if (FAILED(result)) return result;

		// create DGXIFactory
		IDXGIFactory1* dxgiFactoryPtr{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactoryPtr));

		if (FAILED(result)) return result;

		// Create a swap chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// get the handle from the SDL backBuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_WindowPtr, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		// Create swap chain
		result = dxgiFactoryPtr->CreateSwapChain(m_DevicePtr, &swapChainDesc, &m_SwapChainPtr);

		dxgiFactoryPtr->Release();
		if (FAILED(result)) return result;

		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_DevicePtr->CreateTexture2D(&depthStencilDesc, nullptr, &m_DepthStencilBufferPtr);
		if (FAILED(result)) return result;

		result = m_DevicePtr->CreateDepthStencilView(m_DepthStencilBufferPtr, &depthStencilViewDesc, &m_DepthStencilViewPtr);
		if (FAILED(result)) return result;

		// Create renderTarget and renderTargetView
		// resource
		result = m_SwapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_RenderTargetBufferPtr));
		if (FAILED(result)) return result;

		// view
		result = m_DevicePtr->CreateRenderTargetView(m_RenderTargetBufferPtr, nullptr, &m_RenderTargetViewPtr);
		if (FAILED(result)) return result;

		// bind renderTargetView and depthStencilView to output merger stage
		m_DeviceContextPtr->OMSetRenderTargets(1, &m_RenderTargetViewPtr, m_DepthStencilViewPtr);

		// set viewport
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;

		m_DeviceContextPtr->RSSetViewports(1, &viewport);

		return S_OK;
	}

	void Renderer::CycleSamplerState()
	{
		constexpr int nrOfStates{ 3 };
		++m_SamplerState;
		m_SamplerState %= nrOfStates;

		for (int i{}; m_MeshesPtr.size() > i; ++i)
		{
			m_MeshesPtr[i]->GetEffectPtr()->SetSamplerState(m_DevicePtr, m_SamplerState);
		}
	}

	void Renderer::ToggleRotation()
	{
		m_CanRotate = !m_CanRotate;

		const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 0x0c);
		std::cout << "Rotation ";

		if (m_CanRotate) SetConsoleTextAttribute(hConsole, 0x0a);
		else SetConsoleTextAttribute(hConsole, 0x04);
		std::cout << std::boolalpha << m_CanRotate << std::endl;

		SetConsoleTextAttribute(hConsole, 0x07);
	}

	void Renderer::ToggleNormalMap()
	{
		m_UseNormalMap = !m_UseNormalMap;

		const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 0x0c);
		std::cout << "Normal Map ";

		if (m_UseNormalMap) SetConsoleTextAttribute(hConsole, 0x0a);
		else SetConsoleTextAttribute(hConsole, 0x04);
		std::cout << std::boolalpha << m_UseNormalMap << std::endl;

		SetConsoleTextAttribute(hConsole, 0x07);

		m_MeshesPtr[0]->GetEffectPtr()->SetUseNormalMap(m_UseNormalMap);
	}
}
