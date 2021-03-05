#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "Keyboard.h"

class Camera
{
public:
	Camera();
	~Camera() {};
	XMMATRIX GetTransMatrix();
	void CameraUpdate(char key);
	void Reset() noexcept
	{
		r = 120.0f;
		theta = 120.0f;
		phi = 120.0f;
		pitch = 0.0f;
		yaw = 0.0f;
		roll = 0.0f;
	}
public:
	
private:
	float r = 120.0f;
	float theta = 120.0f;
	float phi = 120.0f;
	float pitch = 0.0f; // ratation x;
	float yaw = 0.0f; //rotation y;
	float roll = 0.0f; //rotation z;
	//XMMATRIX tranMatrix;
	//Keyboard keyboard;

//private:

};
