#pragma once

#include "export.h"
#include "DisplayWin.h"

using namespace DirectX;

class GameComponent {
	public:
		virtual int Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin display, HRESULT res) { return 0; };
	
		// virtual int LoadTextureFromFile(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3D11DeviceContext* context, bool generateMips, bool useSrgb, UINT frameIndex) { return 0; };
	
		virtual void DestroyResourses() {};

		virtual void Draw(ID3D11DeviceContext* context) {};
};