//--------------------------------------------------------------------------------------
// File: Tutorial06.cpp
//
// This application demonstrates simple lighting in the vertex shader
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "DXAPI.h"

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11Texture2D*        g_pDepthStencil = NULL;
ID3D11DepthStencilView* g_pDepthStencilView = NULL;
ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;
ID3D11ComputeShader*    g_pCS = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pComputeBuffer = NULL;
ID3D11Buffer*           g_pComputeBufferOut = NULL;
ID3D11Buffer*           g_pVertexBuffer = NULL;
ID3D11Buffer*           g_pIndexBuffer = NULL;
ID3D11Buffer*           g_pConstantBuffer = NULL;
ID3D11Buffer*           g_pConstantBuffer_project = NULL;
ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11ShaderResourceView*		g_pBuf0SRV = NULL;
//ID3D11ShaderResourceView*		g_pBuf1SRV = NULL;
ID3D11UnorderedAccessView*		g_pBufResultUAV = NULL;
ID3D11SamplerState*                 g_pSamplerLinear = NULL;
XMMATRIX                g_World;
XMMATRIX                g_View;
XMMATRIX                g_Projection;
int						g_idxsize;
float					g_t = 0.0f;
UINT NUM_ELEMENTS;

void sethWnd(HWND hWnd)
{
	g_hWnd = hWnd;
}
//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"Direct3D 11 Tutorial 6", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	const D3D_SHADER_MACRO defines[] =
	{
		"USE_STRUCTURED_BUFFERS", "1",
		NULL, NULL
	};
    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, defines, NULL, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		throw "create fail";

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	ID3D11RasterizerState* pRSState;
	D3D11_RASTERIZER_DESC rd = {};
	/*
	g_pd3dDevice->CreateRasterizerState(&rd, &pRSState);
	rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	g_pd3dDevice->CreateRasterizerState(&rd, &pRSState);
	g_pImmediateContext->RSSetState(pRSState);
	*/

}
HRESULT CreateResource()
{
	HRESULT hr = S_OK;
	// Compile the CS shader
	ID3DBlob* pCSBlob = NULL;
	hr = CompileShaderFromFile(L"BasicCompute11.hlsl", "CSMain", "cs_5_0", &pCSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}
	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"Tutorial06.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}
	// Create the compute shader
	hr = g_pd3dDevice->CreateComputeShader(pCSBlob->GetBufferPointer(), pCSBlob->GetBufferSize(), NULL, &g_pCS);
	if (FAILED(hr))
	{
		pCSBlob->Release();
		return hr;
	}
	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // 32+32+32/8 = 12 =
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"Tutorial06.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;
}
//--------------------------------------------------------------------------------------
// Create Structured Buffer
//--------------------------------------------------------------------------------------
HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, NULL, ppBufOut);
}
//--------------------------------------------------------------------------------------
// Create Shader Resource View for Structured or Raw Buffers
//--------------------------------------------------------------------------------------
HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}
//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured or Raw Buffers
//-------------------------------------------------------------------------------------- 
HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}
//creat VB IB
HRESULT CreateVB_IB(SimpleVertex * position , WORD *vertIndexs, int MeshSize, int IdxSize)
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	NUM_ELEMENTS = MeshSize* MeshSize;
	CreateStructuredBuffer(g_pd3dDevice, sizeof(SimpleVertex), NUM_ELEMENTS, position, &g_pComputeBuffer);
	//CreateStructuredBuffer(g_pd3dDevice, sizeof(SimpleVertex), NUM_ELEMENTS, position, &g_pBuf1);
	CreateStructuredBuffer(g_pd3dDevice, sizeof(SimpleVertex), NUM_ELEMENTS, NULL, &g_pComputeBufferOut);
	CreateBufferSRV(g_pd3dDevice, g_pComputeBuffer, &g_pBuf0SRV);
	//CreateBufferSRV(g_pDevice, g_pBuf1, &g_pBuf1SRV);
	CreateBufferUAV(g_pd3dDevice, g_pComputeBufferOut, &g_pBufResultUAV);

    // Create vertex buffer
	SimpleVertex *vertices = position;
    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * MeshSize *MeshSize;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    // Create index buffer
    // Create vertex buffer
	//WORD indices[] = {0,1,2};
	WORD *indices = vertIndexs;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * IdxSize;        // 96 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
	g_idxsize = IdxSize;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer_PROJECT);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pConstantBuffer_project );
    if( FAILED( hr ) )
        return hr;

	bd.ByteWidth = sizeof(ConstantBuffer);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;
    // Initialize the world matrices
	g_World = XMMatrixIdentity();

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( 96.0f, 48.0f, 96.0f, 0.0f );
	XMVECTOR At = XMVectorSet( -32.0f, -32.0f, -32.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	g_View = XMMatrixLookAtLH( Eye, At, Up );

    // Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, width / (FLOAT)height, 0.01f, 512.0f );

	ConstantBuffer_PROJECT cb1;
	cb1.mWorld = XMMatrixTranspose(g_World);
	cb1.mView = XMMatrixTranspose(g_View);
	cb1.mProjection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer_project, 0, NULL, &cb1, 0, 0);


	// Load the Texture
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"sky.jpg", NULL, NULL, &g_pTextureRV, NULL);
	if (FAILED(hr))
		return hr;
	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if (FAILED(hr))
		return hr;


    return S_OK;
}

//for cs debug

ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(pDevice->CreateBuffer(&desc, NULL, &debugbuf)))
	{
#if defined(DEBUG) || defined(PROFILE)
		debugbuf->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Debug") - 1, "Debug");
#endif

		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
}
//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	
	if( g_pImmediateContext ) g_pImmediateContext->ClearState();
	if (g_pSamplerLinear) g_pSamplerLinear->Release();
	if (g_pTextureRV) g_pTextureRV->Release();
    if( g_pConstantBuffer ) g_pConstantBuffer->Release();
	if (g_pConstantBuffer_project) g_pConstantBuffer_project->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pIndexBuffer ) g_pIndexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    //if( g_pPixelShaderSolid ) g_pPixelShaderSolid->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();

	
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void updatecb()
{
	static int total_hit = 20;
	static XMFLOAT4 hit_p[20];
	static bool first_init = true;
	float x = 0.0f;
	float z = 0.0f;
	float m_T = 5.0;
	static int creat_num = 0;
	g_t += 0.0001;

	if (g_t >= 4 * m_T)
	{
		g_t = 0;
	}
	
	float gap = m_T / total_hit;
	int timeer = (int)(g_t * 10000)% (int)(10000 * m_T / total_hit);
	static int creat_idx = 0;
	//create and destroy
	if (timeer == 0 )
	{ 
		x = rand() % 2000 / 1000.0f -1.0f;
		z = rand() % 2000 / 1000.0f -1.0f;
		hit_p[creat_idx] = XMFLOAT4(50.0*x, 0.0f, 50.0*z,0.0f);
		creat_idx++;

		//fisrt_init
		if (first_init == true)
		{
			creat_num = creat_idx;
		}
		else
		{
			creat_num = total_hit;
		}
		//destroy next one
		if (creat_idx == total_hit) { 
			creat_idx = 0; 
			first_init = false;
		}
		hit_p[creat_idx] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	//
	for (int i=0; i < total_hit; i++)
	{
		hit_p[i].w += 0.0001;
	}
	
    ConstantBuffer cb1;
	cb1.t = g_t;
	cb1.T = m_T;
	for (int i = 0; i < creat_num; i++)
	{
		if (i == creat_idx) continue;
		cb1.hit_p[i] = hit_p[i];
		//cb1.hit_p[i].w = 0.0001;
	}
	cb1.x = total_hit;
	cb1.N = NUM_ELEMENTS;
	//simulate
	

	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cb1, 0, 0 );
}

void Render(SimpleVertex *vertex)
{

	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//
    // Render the cube
    //
	updatecb();
	ID3D11ShaderResourceView* aRViews[1] = {g_pBuf0SRV};
	
	g_pImmediateContext->CSSetShader(g_pCS, NULL, 0);
	g_pImmediateContext->CSSetShaderResources(0, 1, aRViews);
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &g_pBufResultUAV, NULL);
	g_pImmediateContext->CSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->Dispatch(NUM_ELEMENTS, 1, 1);
	g_pImmediateContext->CopyResource(g_pVertexBuffer, g_pComputeBufferOut);
	/*
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(g_pd3dDevice, g_pImmediateContext, g_pComputeBufferOut);
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		SimpleVertex *p;
		g_pImmediateContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

		// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS
		// This is also a common trick to debug CS programs.
		p = (SimpleVertex*)MappedResource.pData;
		
		//g_pImmediateContext->UpdateSubresource(g_pVertexBuffer, 0, NULL, p, 0, 0);
	
		
		// Verify that if Compute Shader has done right
		//printf("Verifying against CPU result...");
		BOOL bSuccess = TRUE;
		float u, v;
		for (int i = 0; i < NUM_ELEMENTS; ++i)
		{ 
			 u = p[i].uv.x;
			 v = p[i].uv.y;
		}
	*/
	
	
	g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer_project);
	g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
	g_pImmediateContext->DrawIndexed(g_idxsize, 0, 0 );

    //
    // Render each light
    //
	/*
    for( int m = 0; m < 2; m++ )
    {
		XMMATRIX mLight = XMMatrixTranslationFromVector( 5.0f * XMLoadFloat4( &vLightDirs[m] ) );
		XMMATRIX mLightScale = XMMatrixScaling( 0.2f, 0.2f, 0.2f );
        mLight = mLightScale * mLight;

        // Update the world variable to reflect the current light
		cb1.mWorld = XMMatrixTranspose( mLight );
		cb1.vOutputColor = vLightColors[m];
		g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, NULL, &cb1, 0, 0 );

		g_pImmediateContext->PSSetShader( g_pPixelShaderSolid, NULL, 0 );
		g_pImmediateContext->DrawIndexed( 36, 0, 0 );
    }
	*/
    //
    // Present our back buffer to our front buffer
    //
    g_pSwapChain->Present( 0, 0 );
}


