#pragma once

#include "export.h"
#include "GameComponent.h"

struct TriangleComponentParameters {
	DirectX::XMFLOAT4* points;

	int* indeces;
	int numPoints;
	int numIndeces;

	DirectX::SimpleMath::Vector3 compPosition;
};

struct ConstData
{
	DirectX::SimpleMath::Matrix worldViewProj;
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix invertedWorldTransform;
};

struct LightData {
	DirectX::SimpleMath::Vector4 direction;
	DirectX::SimpleMath::Vector4 color;
	DirectX::SimpleMath::Vector4 viewerPosition;
};

class TriangleComponent : public GameComponent {
private:
	DirectX::SimpleMath::Matrix GetModelMatrix();
	
	float radius;

	TriangleComponentParameters parameters;
	ID3DBlob* vertexBC;
	ID3DBlob* pixelBC;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	UINT strides[4];
	UINT offsets[4];
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

public:
	TriangleComponent* parent;

	ConstData constData;
	LightData lightData;
	ID3D11Buffer* constBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* vBuffers[4];

	DirectX::SimpleMath::Vector3 localEuler;
	DirectX::SimpleMath::Vector3 localScale;
	DirectX::SimpleMath::Vector3 localPosition;
	DirectX::SimpleMath::Vector3 offset;
	DirectX::SimpleMath::Vector4* normals;

	ID3D11RasterizerState* rastState;
	ID3D11InputLayout* layout;

	DirectX::SimpleMath::Vector3 compPosition;

	TriangleComponent();
	TriangleComponent(TriangleComponentParameters param);

	int Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin display, HRESULT result);
	void DestroyResources();
	void Draw(ID3D11DeviceContext* context);
	//void Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, float* BGcolor);
	//void Update(ID3D11DeviceContext* context);
	void Update(ID3D11DeviceContext* context, Camera* camera) override;
	void NormalsCalc();
};