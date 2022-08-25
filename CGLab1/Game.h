#pragma once

#include "export.h"
#include "DisplayWin32.h"
#include "GameComponent.h"

class Game {

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		D3D11_VIEWPORT viewport;
		int numVP;
		IDXGISwapChain* swapChain;
		ID3D11RenderTargetView* rtv;
		ID3D11Texture2D* depthBuffer;
		ID3D11DepthStencilView* depthView;
		ID3DUserDefinedAnnotation* annotation;
		ID3D11Debug* debug;
	
		std::chrono::time_point<std::chrono::steady_clock> prevTime;
		float deltaTime;
		float totalTime = 0;
		unsigned int frameCount = 0;

		void Init();
		int PrepareResources();
		void DestroyResources();
		void PrepareFrame();
		void PrepareFrameViewport(int nVP);
		void EndFrame();
		void Draw();
		void ErrorsOutput(int ErrorCode);

	public:
		float* BGcolor;

		std::vector <GameComponent*> Components;

		DisplayWin32 display;
		InputDevice inputDevice;
		ID3D11DeviceContext* context;

		void Update();
		bool IsKeyDown(Keys key) { inputDevice.IsKeyDown(key); };

		Game();
		void Run();

		void SetBackgroundColor(float* color) { BGcolor = color; };
};