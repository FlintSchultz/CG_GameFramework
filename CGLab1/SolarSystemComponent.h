#pragma once

#include "export.h"
#include "Game.h"

class SolarSystemComponent : public Game
{
private: 
	void Init();
	
	TriangleComponentParameters PrepareRect(float xOff, float yOff);
	TriangleComponentParameters PrepareCircle(float xOff, float yOff);

public:
	float angleValue = 0.0f;

	SolarSystemComponent();
	
	void Run();
	void Update() override;

	TriangleComponent* SunComponent;
	TriangleComponent* Mercury;
	TriangleComponent* Venus;
	TriangleComponent* Mars;
	TriangleComponent* Phobos;
	TriangleComponent* Earth;

	TriangleComponent CreateSphere(
		float radius,
		DirectX::SimpleMath::Vector4 mainColor,
		DirectX::SimpleMath::Vector4 color
	);
	TriangleComponent CreateCube();

	std::vector <TriangleComponent*> PlanetComponents;
};