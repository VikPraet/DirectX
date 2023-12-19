#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
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

		//Create some data for our mesh
		const std::vector<Mesh::Vertex> vertices
		{
			{{ 3,  3, 2 }, { 1.f ,0.f, 0.f }, { 1, 0} },
			{{ 3, -3, 2 }, { 0.f, 1.f, 0.f }, { 1, 1} },
			{{-3, -3, 2 }, { 0.f, 0.f, 1.f }, { 0, 1} },
			{{-3, 3,  2 }, { 0.f, 0.f, 1.f}, { 0, 0} }
		};
		const std::vector<uint32_t> indices{
			0,1,2,
			0,2,3
		};

		m_TrianglePtr = new Mesh( m_DevicePtr, vertices, indices );

		m_CameraPtr = new Camera({ 0,0,-10 }, 45.f, static_cast<float>(m_Width) / static_cast<float>(m_Height));
	}

	Renderer::~Renderer()
	{
		delete m_CameraPtr;
		m_CameraPtr = nullptr;

		delete m_TrianglePtr;
		m_TrianglePtr = nullptr;

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

	void Renderer::Update(const Timer* pTimer)
	{
		m_CameraPtr->Update(pTimer);
	}

	void Renderer::Render() const
	{
		if (!m_IsInitialized) return;

		// clear RTV & DSV
		constexpr float color[4] = {0.f, 0.f, 0.3f, 1.f};
		m_DeviceContextPtr->ClearRenderTargetView(m_RenderTargetViewPtr, color);
		m_DeviceContextPtr->ClearDepthStencilView(m_DepthStencilViewPtr, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		// set pipeline + invoke draw calls (= render)
		Matrix worldViewProjectionMatrix{ m_TrianglePtr->GetWorldMatrix() * m_CameraPtr->GetInvViewMatrix() * m_CameraPtr->GetProjectionMatrix() };
		m_TrianglePtr->Render(m_DeviceContextPtr, reinterpret_cast<float*>(&worldViewProjectionMatrix));

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
}
