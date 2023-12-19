#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>
#include <iostream>

Texture::Texture(const std::string& path, ID3D11Device* devicePtr)
{
	LoadFromFile(path, devicePtr);
}

Texture::~Texture()
{
	if(m_ResourcePtr)
	{
		m_ResourcePtr->Release();
	}
	if(m_ResourceViewPtr)
	{
		m_ResourceViewPtr->Release();
	}
}

void Texture::LoadFromFile(const std::string& path, ID3D11Device* devicePtr)
{
	SDL_Surface* surfacePtr = IMG_Load(path.c_str());

	if (!surfacePtr) return;

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = surfacePtr->w;
	desc.Height = surfacePtr->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = surfacePtr->pixels;
	initData.SysMemPitch = static_cast<UINT>(surfacePtr->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(surfacePtr->h * surfacePtr->pitch);

	HRESULT hr = devicePtr->CreateTexture2D(&desc, &initData, &m_ResourcePtr);
	if(FAILED(hr)) return;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = devicePtr->CreateShaderResourceView(m_ResourcePtr, &SRVDesc, &m_ResourceViewPtr);
	if(FAILED(hr)) return;


	if (m_pSurface)
	{
		SDL_FreeSurface(m_pSurface);
		m_pSurface = nullptr;
	}
}
