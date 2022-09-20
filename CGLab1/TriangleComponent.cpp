#include "export.h"
#include "TriangleComponent.h"
#include "Camera.h"

TriangleComponent::TriangleComponent() {
	parameters.points = nullptr;
	parameters.indeces = nullptr;
	parameters.numPoints = 0;
	parameters.numIndeces = 0;

	compPosition = DirectX::SimpleMath::Vector3(0, 0, 0);

	localEuler = { 0.0f, 0.0f, 0.0f };
	localScale = { 1.0f, 1.0f, 1.0f };
	localPosition = { 0.0f, 0.0f, 0.0f };
	offset = { 0.0f, 0.0f, 0.0f };
	radius = 1.0f;

	parent = nullptr;
	vertexBC = nullptr;
	pixelBC = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;

	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;
	lightBuffer = nullptr;
	rastState = nullptr;
	normals = nullptr;

	for (int i = 0; i < 4; i++) {
		strides[i] = 0;
	}
	
	for (int i = 0; i < 4; i++) {
		offsets[i] = 0;
	}
}

TriangleComponent::TriangleComponent(TriangleComponentParameters param) {
	parameters.points = param.points;
	parameters.indeces = param.indeces;
	parameters.numPoints = param.numPoints;
	parameters.numIndeces = param.numIndeces;

	compPosition = param.compPosition;

	localEuler = { 0.0f, 0.0f, 0.0f };
	localScale = { 1.0f, 1.0f, 1.0f };
	localPosition = { 0.0f, 0.0f, 0.0f };
	offset = { 0.0f, 0.0f, 0.0f };
	radius = 1.0f;

	parent = nullptr;
	vertexBC = nullptr;
	pixelBC = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;
	
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;
	lightBuffer = nullptr;

	rastState = nullptr;

	normals = new DirectX::SimpleMath::Vector4[parameters.numPoints / 2];
	NormalsCalc();

	for (int i = 0; i < 4; i++) {
		strides[i] = 32;
	}
	
	for (int i = 0; i < 4; i++) {
		offsets[i] = 0;
	}
}

int TriangleComponent::Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin display, HRESULT result) {
	ID3DBlob* errorVertexCode = nullptr;
	result = D3DCompileFromFile(
		L"../Shaders/ThirdExampleShader.hlsl",
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,
		&errorVertexCode
	);

	if (FAILED(result)) {
		if (errorVertexCode)
		{
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
		{
			MessageBox(display.getHWND(), L"../Shaders/ThirdExampleShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return 0;
	}

	ConstData constData; // ???
	
	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(constData);
	result = device->CreateBuffer(&constBufDesc, nullptr, &constBuffer);

	if (FAILED(result)) {
		std::cout << "Error while const buffer creating...";
	}

	D3D_SHADER_MACRO Shader_Macros[] = {
		"TEST",
		"1",
		"TCOLOR",
		"float4(0.0f, 1.0f, 0.0f, 1.0f)",
		nullptr,
		nullptr
	};

	D3D11_BUFFER_DESC lightBufDesc = {};
	lightBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufDesc.MiscFlags = 0;
	lightBufDesc.StructureByteStride = 0;
	lightBufDesc.ByteWidth = sizeof(LightData);
	result = device->CreateBuffer(&lightBufDesc, nullptr, &lightBuffer);

	if (FAILED(result)) {
		std::cout << "Error while light buffer creating...";
	}

	ID3DBlob* errorPixelCode;
	result = D3DCompileFromFile(
		L"../Shaders/ThirdExampleShader.hlsl",
		Shader_Macros /*macros*/,
		nullptr /*include*/,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelBC,
		&errorPixelCode
	);

	device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr,
		&vertexShader
	);

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr,
		&pixelShader
	);

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
	   D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
	   D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		D3D11_INPUT_ELEMENT_DESC {
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	device->CreateInputLayout(
		inputElements,
		3,
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout
	);

	int size = parameters.numPoints / 2 * 3;

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * (size);

	DirectX::XMFLOAT4* pointsNormals = new DirectX::XMFLOAT4[size];
	int temp = 0;
	int temp1 = 0;
	for (int i = 0; i < size; i += 3) {
		pointsNormals[i] = parameters.points[temp1];
		pointsNormals[i + 1] = parameters.points[temp1 + 1];
		temp1 += 2;
		pointsNormals[i + 2] = normals[temp];
		temp++;
	}

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = pointsNormals;
	// vertexData.pSysMem = parameters.points;
	vertexData.SysMemPitch = 0;	
	vertexData.SysMemSlicePitch = 0;
	device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * parameters.numIndeces;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = parameters.indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

	strides[0] = 48;
	strides[1] = 48;
	strides[2] = 48;
	strides[3] = 48;
	offsets[0] = 0;
	offsets[1] = 0;
	offsets[2] = 0;
	offsets[3] = 0;

	// RASTERIZER state
	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	result = device->CreateRasterizerState(&rastDesc, &rastState);

	return 0;
}

void TriangleComponent::DestroyResources() {
	if (vertexShader != nullptr) {
		vertexShader->Release();
	}

	if (pixelShader != nullptr) {
		pixelShader->Release();
	}

	if (layout != nullptr) {
		layout->Release();
	}

	if (vertexBuffer != nullptr) {
		vertexBuffer->Release();
	}

	if (indexBuffer != nullptr) {
		indexBuffer->Release();
	}

	if (rastState != nullptr) {
		rastState->Release();
	}

	if (constBuffer != nullptr) {
		constBuffer->Release();
	}

	if (lightBuffer != nullptr) {
		lightBuffer->Release();
	}
}

//void TriangleComponent::Update(ID3D11DeviceContext* context) {
//	D3D11_MAPPED_SUBRESOURCE res = {};
//	context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
//
//	auto dataP = reinterpret_cast<float*>(res.pData);
//	memcpy(dataP, &constData, sizeof(ConstData));
//
//	context->Unmap(constBuffer, 0);
//
//}

void TriangleComponent::Update(ID3D11DeviceContext* context, Camera* camera) {
	// ConstData data;
	constData.worldViewProj = GetModelMatrix() * camera->viewMatrix * camera->projectionMatrix;
	constData.worldViewProj = constData.worldViewProj.Transpose();
	//constData.world = DirectX::SimpleMath::Matrix::CreateTranslation(parameters.compPosition);
	constData.world = GetModelMatrix().Transpose();
	constData.invertedWorldTransform = GetModelMatrix().Transpose().Invert().Transpose();
	
	D3D11_MAPPED_SUBRESOURCE subresourse = {};
	context->Map(
		constBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subresourse
	);
	
	memcpy(
		reinterpret_cast<float*>(subresourse.pData),
		&constData,
		sizeof(ConstData)
	);

	context->Unmap(constBuffer, 0);

	lightData.viewerPosition = DirectX::SimpleMath::Vector4(
		camera->position.x,
		camera->position.y,
		camera->position.z, 
		1.0f
	);
	lightData.direction = DirectX::SimpleMath::Vector4(0.0f, 10.0f, 6.0f, 1.0f);
	lightData.color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	D3D11_MAPPED_SUBRESOURCE subresourse2 = {};
	context->Map(
		lightBuffer,
		0,  
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subresourse2
	);
	
	memcpy(
		reinterpret_cast<float*>(subresourse2.pData),
		&lightData,
		sizeof(LightData)
	);
	context->Unmap(lightBuffer, 0);

}

//void TriangleComponent::Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, float* BGcolor) {
//	if (parameters.numIndeces != 0)
//	{
//		context->IASetInputLayout(layout);
//		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//		context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
//		context->VSSetShader(vertexShader, nullptr, 0);
//		context->PSSetShader(pixelShader, nullptr, 0);
//		context->VSSetConstantBuffers(0, 1, &constBuffer);
//		context->VSSetConstantBuffers(1, 1, &lightBuffer);
//		context->RSSetState(rastState);
//
//		context->DrawIndexed(
//			parameters.numIndeces,
//			0,
//			0
//		);
//	}
//}

void TriangleComponent::Draw(ID3D11DeviceContext* context) {
	if (parameters.numIndeces != 0)
	{
		context->IASetInputLayout(layout);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);
		context->VSSetConstantBuffers(0, 1, &constBuffer);
		context->PSSetConstantBuffers(1, 1, &lightBuffer);
		context->RSSetState(rastState);

		context->DrawIndexed(
			parameters.numIndeces,
			0,
			0
		);
	}
}

void TriangleComponent::NormalsCalc() {
	int ind_a, ind_b, ind_c;
	
	DirectX::SimpleMath::Vector4 a, b, c, p, q, norm;
	DirectX::XMFLOAT4* position = new DirectX::XMFLOAT4[parameters.numPoints / 2];
	
	int temp = 0;
	for (int i = 0; i < parameters.numPoints / 2; i += 1) {
		position[i] = parameters.points[temp];
		temp += 2;
	}

	for (int i = 0; i < parameters.numIndeces; i += 3) {
		ind_a = parameters.indeces[i];
		ind_b = parameters.indeces[i + 1];
		ind_c = parameters.indeces[i + 2];

		a = position[ind_a];
		b = position[ind_b];
		c = position[ind_c];

		p = a - b;
		q = c - b;

		norm.x = p.y * q.z - p.z * q.y;
		norm.y = p.z * q.x - p.x * q.z;
		norm.z = p.x * q.y - p.y * q.x;
		norm.w = 1.0f;

		normals[ind_a] = norm;
		normals[ind_b] = norm;
		normals[ind_c] = norm;
	};
}

DirectX::SimpleMath::Matrix TriangleComponent::GetModelMatrix()
{
	using namespace DirectX::SimpleMath;
	Matrix model = Matrix::Identity;
	model *= Matrix::CreateScale(localScale);
	model *= Matrix::CreateFromYawPitchRoll(localEuler * DirectX::XM_PI / 180);
	model *= Matrix::CreateTranslation(localPosition);

	if (parent != nullptr)
	{
		model *= parent->GetModelMatrix();
	}

	return model;
}