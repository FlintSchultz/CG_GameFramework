#pragma once

#include "export.h"

#include "GameComponent.h"


struct LineComponentParameters {
	DirectX::SimpleMath::Vector4* positions;
	DirectX::SimpleMath::Vector4* colors;
	int numPoints;
	DirectX::SimpleMath::Vector3 compPosition;
};

class LineComponent : public GameComponent {
private:
	LineComponentParameters parameters;

	ID3DBlob* vertexBC;
	ID3DBlob* pixelBC;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* vBuffers[2];

	UINT strides[2]; 
	UINT offsets[2];

	ID3D11Buffer* indBuf;
	ID3D11BlendState* blend;
	ID3D11Buffer* constBuf;

	float blendFactor[4];

	UINT sampleMask;

public:
	DirectX::SimpleMath::Vector3 compPosition;

	LineComponent();
	LineComponent(LineComponentParameters param);

	int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device);
	int LoadTextureFromFile(
		Microsoft::WRL::ComPtr<ID3D11Device> device, 
		ID3D11DeviceContext* context, 
		bool generateMips, 
		bool useSrgb, 
		UINT frameIndex
	) { return 0; };

	void DestroyResourses();
	void Draw(ID3D11DeviceContext* context);
};
