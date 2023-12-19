#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer) const;
		void Render() const;

		void CycleSamplerState();
		void ToggleRotation();

	private:
		SDL_Window*				m_WindowPtr{};
		ID3D11Device*			m_DevicePtr{};
		ID3D11DeviceContext*	m_DeviceContextPtr{};
		IDXGISwapChain*			m_SwapChainPtr{};

		ID3D11Texture2D*		m_DepthStencilBufferPtr{};
		ID3D11DepthStencilView* m_DepthStencilViewPtr{};

		ID3D11Resource*			m_RenderTargetBufferPtr{};
		ID3D11RenderTargetView* m_RenderTargetViewPtr{};

		Camera* m_CameraPtr{};
		Mesh* m_TrianglePtr{};

		std::vector<Mesh*> m_MeshesPtr{};
		const Vector3 m_VehiclePos{ 0, 0, 0 };

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		int m_SamplerState{};
		bool m_CanRotate{ false };

		//DIRECTX
		HRESULT InitializeDirectX();
		//...
	};
}
