#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"


class Texture
{
public:
	Texture(const std::string& path, ID3D11Device* devicePtr);
	~Texture();

	void LoadFromFile(const std::string& path, ID3D11Device* devicePtr);
	ID3D11ShaderResourceView* GetResourceView() const { return m_ResourceViewPtr; }

private:

	SDL_Surface* m_pSurface{ nullptr };
	uint32_t* m_pSurfacePixels{ nullptr };

	ID3D11Texture2D* m_ResourcePtr{ nullptr };
	ID3D11ShaderResourceView* m_ResourceViewPtr{ nullptr };
};
