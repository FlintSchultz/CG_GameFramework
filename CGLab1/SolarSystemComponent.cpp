#include "SolarSystemComponent.h"
#include <random>

SolarSystemComponent::SolarSystemComponent() 
{
	Init();
	SetBackgroundColor(new float[4] { 0.2f, 0.2f, 0.5f, 1.0f });
}

void SolarSystemComponent::Init()
{
	SunComponent = new TriangleComponent(CreateSphere(
		1,
		DirectX::SimpleMath::Vector4(1.0f, 0.8f, 0.2f, 1.0f),
		DirectX::SimpleMath::Vector4(1.0f, 0.5f, 0.0f, 1.0f)
	));
	Components.push_back(SunComponent);


	Venus = new TriangleComponent(CreateSphere(
		2,
		DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.0f, 0.2f, 0.4f, 1.0f)
	));
	//Venus->parent = SunComponent;
	Venus->localPosition = { 20.0, 0.0, 0.0, 0.0 };
	Components.push_back(Venus);

	Mercury = new TriangleComponent(CreateSphere(
		0.2, 
		DirectX::SimpleMath::Vector4(1.0f, 0.1f, 0.1f, 1.0f), 
		DirectX::SimpleMath::Vector4(1.0f, 0.2f, 0.4f, 1.0f)
	));
	//Mercury->parent = SunComponent;
	Mercury->localPosition = { 10.0, 0.0, 0.0, 0.0 };
	Components.push_back(Mercury);

	Mars = new TriangleComponent(CreateSphere(
		2,
		DirectX::SimpleMath::Vector4(0.25f, 0.0f, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.4f, 0.0f, 0.0f, 1.0f)
	));
	Mars->parent = SunComponent;
	Mars->localPosition = { 40.0, 0.0, 0.0, 0.0 };
	Components.push_back(Mars);

	Phobos = new TriangleComponent(CreateSphere(
		0.2,
		DirectX::SimpleMath::Vector4(1.0f, 0.1f, 0.1f, 1.0f),
		DirectX::SimpleMath::Vector4(1.0f, 0.2f, 0.4f, 1.0f)
	));
	Phobos->parent = Mars;
	Phobos->localPosition = { 5.0, 0.0, 0.0, 0.0 };
	Components.push_back(Phobos);

	Earth = new TriangleComponent(CreateCube());
	Earth->parent = SunComponent;
	Earth->localPosition = { 30.0, 0.0, 0.0, 0.0 };
	Components.push_back((Earth));
}

void SolarSystemComponent::Run() 
{
	Game::Run();
}

void SolarSystemComponent::Update() 
{
	Game::camera.at(0)->Update(
		Game::deltaTime, 
		Game::display.getScreenWidth(), 
		Game::display.getScreenHeight()
	);

	angleValue += 0.2;
	
	SunComponent->localEuler = { 0, angleValue, 0 };
	SunComponent->Update(Game::context, Game::camera.at(0));

	Mercury->localEuler = { 0, angleValue, 0 };
	Mercury->localPosition = { 10 * static_cast<float>(cos(angleValue * 2 * 3.14 / 180)), 0, 10 * static_cast<float>(sin(angleValue * 2 * 3.14 / 180)) };

	std::cout << cos(angleValue * 2 * 3.14 / 180) << std::endl;

	float newScale = std::abs(cos(angleValue * 2 * 3.14 / 180) * 2);

	Mercury->localScale = { newScale, newScale, newScale };

	
	Mercury->Update(Game::context, Game::camera.at(0));

	//Mars->localEuler = { 0, angleValue, 0 };
	//Mars->localPosition = { 40 * static_cast<float>(cos(angleValue * 0.5 * 3.14 / 180)), 0, 40 * static_cast<float>(sin(angleValue * 0.5 * 3.14 / 180)) };
	//Mars->Update(Game::context, Game::camera.at(0));

	//Venus->localPosition = { 20 * static_cast<float>(cos(angleValue * 1.5 * 3.14 / 180)), 0, 20 * static_cast<float>(sin(angleValue * 1.5 * 3.14 / 180)) };
	//Venus->localEuler = { 0, -angleValue * 2, 0 };
	//Venus->Update(Game::context, Game::camera.at(0));

	//Phobos->localEuler = { angleValue, angleValue, 0 };
	//Phobos->localPosition = { 5 * static_cast<float>(cos(angleValue * 4 * 3.14 / 180)), 0, 5 * static_cast<float>(sin(angleValue * 4 * 3.14 / 180)) };
	//Phobos->Update(Game::context, Game::camera.at(0));

	//Earth->localEuler = { angleValue, angleValue, 0 };
	//Earth->Update(Game::context, Game::camera.at(0));
}

TriangleComponent SolarSystemComponent::CreateCube() {
	TriangleComponentParameters cube;
	cube.numPoints = 48;
	cube.numIndeces = 36;
	cube.points = new DirectX::XMFLOAT4[48] {
		DirectX::XMFLOAT4(-0.5f, 0.0f, 0.5f, 1.0f), 
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 0.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 1.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 1.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 1.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 1.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 1.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 0.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 1.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 1.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 1.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 0.0f, -0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 1.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f) 
	};
	
	cube.indeces = new int[36] {
		0, 1, 2,
		2, 3, 0,
		6, 5, 4,
		4, 7, 6,
		8, 9, 10,
		10, 11, 8,
		14, 13, 12,
		12, 15, 14,
		18, 17, 16,
		16, 19, 18,
		20, 21, 22,
		22, 23, 20 
	};

	TriangleComponent Cube1(cube);

	return cube;
}


TriangleComponent SolarSystemComponent::CreateSphere(
	float r, 
	DirectX::SimpleMath::Vector4 mainColor, 
	DirectX::SimpleMath::Vector4 color
) {
	TriangleComponentParameters sphere;
	int parallels = 100;
	int meridians = 100;
	
	sphere.numPoints = parallels * meridians * 4 - 1 * 2 * meridians;
	DirectX::SimpleMath::Vector4* positions = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	
	sphere.numIndeces = parallels * meridians * 6 - 3 * 2 * meridians;
	sphere.numPoints *= 2;
	sphere.points = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	sphere.indeces = new int[sphere.numIndeces];
	
	int tempPos = 0;
	int tempInd = 0;
	
	DirectX::SimpleMath::Vector4 vertex1, vertex2, vertex3, vertex4;
	
	for (int i = 0; i < parallels; i++)
	{
		float teta1 = ((float)(i) / parallels) * PI;
		float teta2 = ((float)(i + 1) / parallels) * PI;
		
		for (int j = 0; j < meridians; j++)
		{
			float fi1 = ((float)(j) / meridians) * 2 * PI;
			float fi2 = ((float)(j + 1) / meridians) * 2 * PI;

			float x;
			float y;
			float z;
			
			x = r * sin(teta1) * cos(fi1);
			y = r * sin(teta1) * sin(fi1);
			z = r * cos(teta1);
			vertex1 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = r * sin(teta1) * cos(fi2);
			y = r * sin(teta1) * sin(fi2);
			z = r * cos(teta1);
			vertex2 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = r * sin(teta2) * cos(fi2);
			y = r * sin(teta2) * sin(fi2);
			z = r * cos(teta2);
			vertex3 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = r * sin(teta2) * cos(fi1);
			y = r * sin(teta2) * sin(fi1);
			z = r * cos(teta2);
			vertex4 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);

			if (i == 0)
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else if (i == parallels - 1)
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				tempPos++;

				sphere.indeces[tempInd] = tempPos - 1;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 4;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 3;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 3;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 2;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 1;
				tempInd++;
			}
		}
	}
	
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, 100);

	float colorPoints = uni(rng);
	int temp = 0;

	for (int i = 0; i < sphere.numPoints; i += 2) {
		colorPoints = uni(rng);
		sphere.points[i] = positions[temp];
		sphere.points[i + 1] = mainColor;
		
		if (colorPoints <= 5) {
			sphere.points[i + 1] = color;
		}

		temp++;
	}
	
	TriangleComponent Sphere1(sphere);
	
	return Sphere1;

}