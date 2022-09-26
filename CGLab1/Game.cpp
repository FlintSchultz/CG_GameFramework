#include "export.h"
#include "Game.h"
#include "TriangleComponent.h"

#define PI 3.1415926535

Game::Game() {
	context = nullptr;
	swapChain = nullptr;
	rtv = nullptr;
	BGcolor = new float[4] { 0.2f, 0.1f, 0.4f, 1.0f };
}

void Game::Init() {
	inputDevice.Init(display.getHWND());

	display.CreateDisplay(&inputDevice);

	// CreateCircle();
}

void Game::Run() {
	Init();

	int errors = PrepareResources();
	MSG msg = {};
	bool isExitRequested = false;

	CreateTriangle();

	for (int i = 0; i < Components.size(); i++)
	{
		Components[i]->Init(device, display);
	}

	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// If windows signals to end the application then exit out.
			if (msg.message == WM_QUIT) {
				isExitRequested = true;
			}
		}

		PrepareFrame();
		Draw();

		swapChain->Present(1, 0);
	}

	DestroyResources();
}

int Game::PrepareResources() {
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };
	
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = display.getScreenWidth();
	swapDesc.BufferDesc.Height = display.getScreenHeight();
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = display.getHWND();
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	HRESULT res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context
	);
	
	if (FAILED(res))
	{
		std::cout << "Error while create device and swap chain" << std::endl;
	}

	ID3D11Texture2D* backTexture;
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTexture);	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backTexture, nullptr, &rtv);

	if (FAILED(res))
	{
		std::cout << "Error while create render target view" << std::endl;
	}

	return 0;
}

void Game::DestroyResources() {
	for (int i = 0; i < Components.size(); i++) {
		Components[i]->DestroyResourses();
	}
	
	if (context != nullptr) {
		context->ClearState();
		context->Release();
	}
	
	if (swapChain != nullptr) {
		swapChain->Release();
	}
	
	if (device != nullptr) {
		device->Release();
	}
}

void Game::PrepareFrame() {
	auto curTime = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime).count() / 1000000.0f;
	prevTime = curTime;
	totalTime += deltaTime;
	frameCount++;
	
	if (totalTime > 1.0f) {
		float fps = frameCount / totalTime;
		totalTime = 0.0f;
		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(display.getHWND(), text);
		frameCount = 0;
	}

	context->ClearState();
	
	context->OMSetRenderTargets(1, &rtv, nullptr);
	context->ClearRenderTargetView(rtv, BGcolor);

	viewport = {};
	viewport.Width = static_cast<float>(display.getScreenWidth());
	viewport.Height = static_cast<float>(display.getScreenHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

}

void Game::Update() {

}

void Game::Draw() {
	for (int i = 0; i < Components.size(); i++)
		Components[i]->Draw(context);
}

void Game::CreateTriangle() {
	TriangleComponentParameters rect;
	rect.numPoints = 10;
	rect.numIndeces = 9;
	rect.points = new DirectX::XMFLOAT4[rect.numPoints] {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.8f, 0.8f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	};

	auto localTriangleComponent = new TriangleComponent(rect);

	Components.push_back(localTriangleComponent);
}

void Game::CreateCircle() {
	float radius = 0.55f;

	TriangleComponentParameters circle;

	circle.numPoints = 72;
	circle.numIndeces = 36;

	int i = 0;
	float y = 0.0f;
	float temp = 0.0f;

	circle.points = new DirectX::XMFLOAT4[circle.numPoints * 2];
	circle.indeces = new int[circle.numIndeces * 2];

	for (int j = 0; j < circle.numIndeces; j++) {
		circle.indeces[j] = j;
	}

	for (float x = -radius; x < radius - 0.000001; x = x + (radius / 6)) {
		circle.points[i] = DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);
		i++;
	
		circle.points[i] = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		i++;
		
		circle.points[i] = DirectX::XMFLOAT4(x, y, 0.5f, 1.0f);
		i++;
		
		circle.points[i] = DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
		i++;
		
		temp = x + (radius / 6);
		y = sqrt(pow(radius, 2) - pow(temp, 2));
		
		circle.points[i] = DirectX::XMFLOAT4(temp, y, 0.5f, 1.0f);
		i++;
		
		circle.points[i] = DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
		i++;
	}
	
	circle.numPoints = circle.numPoints * 2;
	circle.numIndeces = circle.numIndeces * 2;
	
	y = 0.0f;
	temp = 0.0f;
	
	for (int j = 24; j < circle.numIndeces; j++) {
		circle.indeces[j] = j;
	}

	for (float x = -radius; x < radius - 0.000001; x = x + (radius / 6)) {
		circle.points[i] = DirectX::XMFLOAT4(0, 0, 0.5f, 1.0f);
		i++;

		circle.points[i] = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		i++;

		circle.points[i] = DirectX::XMFLOAT4(x + 0, -y + 0, 0.5f, 1.0f);
		i++;

		circle.points[i] = DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
		i++;
		
		temp = x + (radius / 6);
		y = sqrt(pow(radius, 2) - pow(temp, 2));
		
		circle.points[i] = DirectX::XMFLOAT4(temp + 0, -y + 0, 0.5f, 1.0f);
		i++;
		
		circle.points[i] = DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
		i++;
	}

	auto localCircleComponent = new TriangleComponent(circle);

	Components.push_back(localCircleComponent);
};