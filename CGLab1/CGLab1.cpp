// CGLab1.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "DisplayWin32.h"
#include "Game.h"

int main()
{
    // Window initialization. Inputs registration.
    DisplayWin32 displayWindowInstance;
    InputDevice inputDevice = InputDevice(displayWindowInstance.getHWND());
    displayWindowInstance.CreateDisplay(&inputDevice);

    // Создаем Device со SwapChain

    D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_11_1};
   
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = displayWindowInstance.getScreenWidth();
    swapDesc.BufferDesc.Height = displayWindowInstance.getScreenHeight();
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = displayWindowInstance.getHWND();
    swapDesc.Windowed = true;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    IDXGISwapChain* swapChain;
    ID3D11DeviceContext* context;

    auto res = D3D11CreateDeviceAndSwapChain(
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
        
    }

    ID3D11Texture2D* backTexture;
    ID3D11RenderTargetView* rtv;
    res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTexture);
    res = device->CreateRenderTargetView(backTexture, nullptr, &rtv);

    ID3DBlob* vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    res = D3DCompileFromFile(
        L"../Shaders/FirstExampleShader.hlsl",
        nullptr /* macros */,
        nullptr /* include */,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vertexBC,
        &errorVertexCode
    );

    if (FAILED(res)) {
        // If the shader failed to compile it should have written somethig to the error message.
        if (errorVertexCode) {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
            std::cout << compileErrors << std::endl;
        } 
        // If there was nothing in the error message then it simply could not find the shader file itself
        else
        {
            MessageBox(displayWindowInstance.getHWND(), L"../Shaders/FirstExampleShader.hlsl", L"Missing Shader File", MB_OK);
        }

        return 0;
    }

    D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
    ID3DBlob* pixelBC;
    ID3DBlob* errorPixelCode;
    res = D3DCompileFromFile(L"../Shaders/FirstExampleShader.hlsl", Shader_Macros /* macros */, nullptr /* include */, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    device->CreateVertexShader(
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        nullptr, &vertexShader);

    device->CreatePixelShader(
        pixelBC->GetBufferPointer(),
        pixelBC->GetBufferSize(),
        nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}
    };

    ID3D11InputLayout* layout;
    device->CreateInputLayout(
        inputElements,
        2,
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        &layout);

    // Create set of points
    DirectX::XMFLOAT4 points[8] = {
        DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    };

    // Create Vertex and Index buffers
    D3D11_BUFFER_DESC vertexBuffDesc = {};
    vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBuffDesc.CPUAccessFlags = 0;
    vertexBuffDesc.MiscFlags = 0;
    vertexBuffDesc.StructureByteStride = 0;
    vertexBuffDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = points;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    ID3D11Buffer* vb;
    device->CreateBuffer(&vertexBuffDesc, &vertexData, &vb);

    int indeces[] = { 0,1,2, 1,0,3 };
    D3D11_BUFFER_DESC indexBuffDesc = {};
    indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBuffDesc.CPUAccessFlags = 0;
    indexBuffDesc.MiscFlags = 0;
    indexBuffDesc.StructureByteStride = 0;
    indexBuffDesc.ByteWidth = sizeof(int) * std::size(indeces);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indeces;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    ID3D11Buffer* ib;
    device->CreateBuffer(&indexBuffDesc, &indexData, &ib);

    // Setup the Input Assembeler (IA) stage
    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };

    // Setup Rasterizer Stage and Viewport
    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    ID3D11RasterizerState* rastState;
    res = device->CreateRasterizerState(&rastDesc, &rastState);

    context->RSSetState(rastState);

    std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
    float totalTime = 0;
    unsigned int frameCount = 0;


    MSG msg = {};
    bool isExitRequested = false;
    while (!isExitRequested) {
        // Handle the windows messages.
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If windows signals to end the application then exit out.
        if (msg.message == WM_QUIT) {
            isExitRequested = true;
        }

        context->ClearState();

        context->RSSetState(rastState);

        // Set BackBuffer for Output
        D3D11_VIEWPORT viewport = {};
        viewport.Width = static_cast<float>(displayWindowInstance.getScreenWidth());
        viewport.Height = static_cast<float>(displayWindowInstance.getScreenHeight());
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.MinDepth = 0;
        viewport.MaxDepth = 1.0f;

        context->RSSetViewports(1, &viewport);

        context->IASetInputLayout(layout);
        context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
        context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
        context->VSSetShader(vertexShader, nullptr, 0);
        context->PSSetShader(pixelShader, nullptr, 0);

        auto	curTime = std::chrono::steady_clock::now();
        float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
        PrevTime = curTime;

        totalTime += deltaTime;
        frameCount++;

        if (totalTime > 1.0f) {
            float fps = frameCount / totalTime;

            totalTime -= 1.0f;

            WCHAR text[256];
            swprintf_s(text, TEXT("FPS: %f"), fps);
            SetWindowText(displayWindowInstance.getHWND(), text);

            frameCount = 0;
        }

        context->OMSetRenderTargets(1, &rtv, nullptr);

        float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
        context->ClearRenderTargetView(rtv, color);

        context->DrawIndexed(6, 0, 0);

        //context->OMSetRenderTargets(0, nullptr, nullptr);

        // Present the result
        swapChain->Present(1, 0);
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
