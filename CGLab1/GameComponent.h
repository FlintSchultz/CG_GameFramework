#pragma once

#include "export.h"
#include "DisplayWin.h"
#include "Camera.h"

using namespace DirectX;

class GameComponent {
	public:
		virtual int Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin display, HRESULT res) { return 0; };
	
		virtual void DestroyResourses() {};

		virtual void Draw(ID3D11DeviceContext* context) {};

		// virtual void Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, float* BGcolor) {};

		// virtual void Update(ID3D11DeviceContext* context) {};

		virtual void Update(ID3D11DeviceContext* context, Camera* camera) = 0;
};