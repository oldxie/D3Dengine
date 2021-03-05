#include "Camera.h"
#define deg(x) x/360.0f*3.1415f

Camera::Camera()
{
};

XMMATRIX Camera::GetTransMatrix() {
	const auto pos = XMVector3Transform(
		XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f)
	);
	return XMMatrixLookAtLH(
		pos, XMVectorZero(),
		XMVectorSet(0.0f,0.1f, 0.0f, 0.0f)
	) * XMMatrixRotationRollPitchYaw(
		pitch, -yaw, roll
	);
};
void Camera::CameraUpdate(char key) {

	switch (key)
	{
	case 'w':
		pitch = pitch + deg(1.0f);
		break;
	case 's':
		pitch = pitch - deg(1.0f);
		break;
	case 'd':
		yaw = yaw + deg(1.0f);
		break;
	case 'a':
		yaw = yaw - deg(1.0f);
		break;
	case 'e':
		roll = roll + deg(1.0f);
		break;
	case 'q':
		roll = roll - deg(1.0f);
		break;
	case 'i':
		r = r - 0.5f;
		break;
	case 'k':
		r = r + 0.5f;
		break;
	case 'j':
		theta = theta - deg(0.5f);
		break;
	case 'l':
		theta = theta + deg(0.5f);
		break;
	case 'u':
		phi = phi - deg(0.5f);
		break;
	case 'm':
		phi = phi + deg(0.5f);
		break;
	default:
		break;
	}
};

