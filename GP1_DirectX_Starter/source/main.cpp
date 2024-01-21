#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void PrintControls()
{
	const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 0x0e);

	std::cout << std::endl;

	SetConsoleTextAttribute(hConsole, 0xe0);
	std::cout << "RENDERING";
	SetConsoleTextAttribute(hConsole, 0x07);
	std::cout << std::endl;

	SetConsoleTextAttribute(hConsole, 0x0e);
	std::cout << "'F4' \t cycle sampling state" << std::endl;
	std::cout << "'F5' \t toggle rotation" << std::endl;
	std::cout << "'F6' \t toggle normal map" << std::endl;
	std::cout << "'F7' \t toggle fire fx" << std::endl;

	std::cout << std::endl;

	SetConsoleTextAttribute(hConsole, 0xf0);
	std::cout << "KEYBOARD";
	SetConsoleTextAttribute(hConsole, 0x07);
	std::cout << std::endl;

	SetConsoleTextAttribute(hConsole, 0x0f);
	std::cout << "'W / UP'        move camera forward" << std::endl;
	std::cout << "'S / DOWN'      move camera backward" << std::endl;
	std::cout << "'A / LEFT'      move camera left" << std::endl;
	std::cout << "'D / RIGHT'     move camera fight" << std::endl;
	std::cout << "'LSHIFT'        camera speed boost" << std::endl;
	std::cout << "'Q'             move camera down" << std::endl;
	std::cout << "'E'             move camera up" << std::endl;
	std::cout << "'F'             recenter camera" << std::endl;

	std::cout << std::endl;

	SetConsoleTextAttribute(hConsole, 0x90);
	std::cout << "MOUSE";
	SetConsoleTextAttribute(hConsole, 0x07);
	std::cout << std::endl;

	SetConsoleTextAttribute(hConsole, 0x09);
	std::cout << "'RMB'           look around" << std::endl;
	std::cout << "'LMB'           move camera forward / backwards" << std::endl;
	std::cout << "'LMB + RMB'     move camera up / down" << std::endl;
	std::cout << "'LALT' + 'LMB'  orbit around center point" << std::endl;
	std::cout << "'LALT' + 'RMB'  move camera forwards / backwards" << std::endl;
	std::cout << "'SCROLL'        zoom in / out" << std::endl;

	std::cout << std::endl;

	SetConsoleTextAttribute(hConsole, 0x07);
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - ***Vik Praet/2DAE10***",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	// clear console
	system("CLS");

	// show info text
	const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 0xc0);
	std::cout << "press 'i' for key-bindings" << std::endl;
	SetConsoleTextAttribute(hConsole, 0x07);

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);
	pRenderer->CycleSamplerState();

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				//Test for a key
				//if (e.key.keysym.scancode == SDL_SCANCODE_X)
				if (e.key.keysym.scancode == SDL_SCANCODE_I)
				{
					PrintControls();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					pRenderer->CycleSamplerState();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					pRenderer->ToggleRotation();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F6)
				{
					pRenderer->ToggleNormalMap();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F7)
				{
					pRenderer->ToggleFireFX();
				}
				break;
			case SDL_MOUSEWHEEL:
				{
					constexpr float fovIncrement{ 3 };
					if (e.wheel.y > 0)
					{
						pRenderer->GetCamera().UpdateFOV(-fovIncrement);
					}
					if (e.wheel.y < 0)
					{
						pRenderer->GetCamera().UpdateFOV(fovIncrement);
					}
				}
			break;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}