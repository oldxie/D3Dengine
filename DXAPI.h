#pragma once
#include <stdio.h>
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "resource.h"


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 uv;
	XMFLOAT3 Normal;
};


struct ConstantBuffer
{
	//XMMATRIX mWorld;
	XMFLOAT4 hit_p[100];
	float t;
	float T;
	int x;
	int N;
};

struct ConstantBuffer_PROJECT
{
XMMATRIX mWorld;
XMMATRIX mView;
XMMATRIX mProjection;
};
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();


HRESULT CreateResource();
HRESULT CreateVB_IB(SimpleVertex * position, WORD *vertIndexs, int MeshSize, int IdxSize);
HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut);
HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void CleanupDevice();
void Render(SimpleVertex * position=NULL);
//methods
SimpleVertex *  CreateMesh(); 
void sethWnd(HWND hWnd);