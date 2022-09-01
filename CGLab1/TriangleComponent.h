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
	DirectX::SimpleMath::Matrix WorldViewProj;
	DirectX::SimpleMath::Matrix World;
	DirectX::SimpleMath::Matrix InvertedWorldTransform;
};

struct LightData {
	DirectX::SimpleMath::Vector4 Direction;
	DirectX::SimpleMath::Vector4 Color;
	DirectX::SimpleMath::Vector4 ViewerPosition;
};

class TriangleComponent : public GameComponent {
private:
	TriangleComponent* parent;

	ConstData constData;
	LightData lightData;

	bool isCatch;
	float radius;
	float a = 0;
	
	DirectX::SimpleMath::Vector3 localEuler;
	DirectX::SimpleMath::Vector3 localScale;
	DirectX::SimpleMath::Vector3 localPosition;
	DirectX::SimpleMath::Vector3 offset;

	DirectX::SimpleMath::Matrix GetModelMatrix();

	TriangleComponentParameters parameters;
	ID3DBlob* vertexBC;
	ID3DBlob* pixelBC;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	UINT strides[1];
	UINT offsets[1];
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* vBuffers[4];
	ID3D11BlendState* blend;
	ID3D11SamplerState* sampler;

public:
	ID3D11RasterizerState* rastState;
	ID3D11InputLayout* layout;

	DirectX::SimpleMath::Vector3 compPosition;

	TriangleComponent();
	TriangleComponent(TriangleComponentParameters param);

	int Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin display, HRESULT result);
	void DestroyResources();
	void Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, float* BGcolor);
};
