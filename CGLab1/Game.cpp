#include "export.h"
#include "Game.h"

Game::Game() {

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	D3D11_VIEWPORT viewport;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3D11Debug* debug;

	std::chrono::time_point<std::chrono::steady_clock> prevTime;

	float deltaTime;
	float totalTime = 0;
	unsigned int frameCount = 0;

	ID3D11Texture2D* depthBuffer;
	ID3D11DepthStencilView* depthView;

	void Init();
	int PrepareResources();
	void DestroyResources();
	void PrepareFrame();
	void Draw();

	float* BGcolor;

	std::vector <GameComponent*> Components;

	ID3D11DeviceContext* context;
	DisplayWin32 display;
	InputDevice inputDevice;


	context = nullptr;
	swapChain = nullptr;
	rtv = nullptr;
	debug = nullptr;
	BGcolor = new float[4] { 0.0f, 0.0f, 0.0f, 0.0f };
	depthBuffer = nullptr;
	depthView = nullptr;
}

void Game::Init() {
	inputDevice = InputDevice(display.getHWND());
}

void Game::Run() {
	Init();

	display.CreateDisplay(&inputDevice);

	int errors = PrepareResources();
	MSG msg = {};
	bool isExitRequested = false;


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
	
		//swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
		PrepareFrame();
		context->RSSetViewports(1, &viewport);
		context->OMSetRenderTargets(1, &rtv, depthView);
		
		//context->OMSetRenderTargets(1, &rtv, nullptr);
		
		Update();
		Draw();
		
		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
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

	viewport = {};
	viewport.Width = static_cast<float>(display.getScreenWidth());
	viewport.Height = static_cast<float>(display.getScreenHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

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

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.ArraySize = 1;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.Width = display.getScreenWidth();
	depthTexDesc.Height = display.getScreenHeight();
	depthTexDesc.SampleDesc = { 1, 0 }; 
	res = device->CreateTexture2D(&depthTexDesc, nullptr, &depthBuffer);


	D3D11_DEPTH_STENCIL_VIEW_DESC depthStenDesc = {};
	depthStenDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStenDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStenDesc.Flags = 0;
	res = device->CreateDepthStencilView(depthBuffer, &depthStenDesc, &depthView);


	ID3D11Texture2D* backTex;
	rtv;
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);

	for (int i = 0; i < Components.size(); i++)
	{
		Components[i]->Init(device, display, res);
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
	
	if (debug != nullptr) {
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	
	if (depthBuffer) {
		depthBuffer->Release();
	}
	
	if (depthView != nullptr) {
		depthView->Release();
	}
}

void Game::PrepareFrame() {
	//std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	//float totalTime = 0;
	//unsigned int frameCount = 0;
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
	context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Game::Update() {

}

void Game::Draw() {
	//TC.Draw(context);
	context->ClearRenderTargetView(rtv, BGcolor);
	for (int i = 0; i < Components.size(); i++)
		Components[i]->Draw(context);
}