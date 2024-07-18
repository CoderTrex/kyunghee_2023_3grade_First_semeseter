#include "Game.h"

HWND g_hWnd = nullptr;
HINSTANCE g_hInstance = nullptr;
LPCWSTR g_pszWindowName = L"GCP 2021103751 JeongEunSeong";
LPCWSTR g_pszWindowClassName = L"GGPWindowClass";

D3D_DRIVER_TYPE g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11Device1* g_pd3dDevice1 = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
ID3D11DeviceContext1* g_pImmediateContext1 = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
IDXGISwapChain1* g_pSwapChain1 = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_pIndexBuffer = nullptr;
ID3D11InputLayout* g_pVertexLayout = nullptr;

//ID3D11Buffer* g_pConstantBuffer = nullptr; 이제 사용하지 않음
XMMATRIX g_worldMatrix1;
XMMATRIX g_viewMatrix1;
XMMATRIX g_projectionMatrix1;

ID3D11Texture2D* g_pDepthStencil = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;

XMVECTOR at;
XMVECTOR eye;
XMVECTOR up;

ID3D11ShaderResourceView* g_pTextureRV = nullptr;
ID3D11SamplerState* g_pSamplerLinear = nullptr;

ID3D11Buffer *g_pCBView = nullptr;
ID3D11Buffer *g_pCBProjection = nullptr;
ID3D11Buffer *g_pCBWorld = nullptr;


LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	if (!hWnd)
	{
		DWORD dwError = GetLastError();
		MessageBox(
			nullptr,
			L"Call to CreateWindow failed!",
			L"Game Graphics Programming",
			NULL
		);
		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
		{
			return HRESULT_FROM_WIN32(dwError);
		}
		return E_FAIL;
	}

	switch (uMsg)
	{
	case WM_CLOSE:
		if (MessageBox(hWnd,
			L"진짜 종료하시겠습니까?",
			L"Game Graphics Programming",
			MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hWnd);
		}
		return 0;
	case WM_SIZE: // window resizing
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
{
	WNDCLASSEX wcex =
	{
	.cbSize = sizeof(WNDCLASSEX),
	.style = CS_HREDRAW | CS_VREDRAW,
	.lpfnWndProc = WindowProc,
	.cbClsExtra = 0,
	.cbWndExtra = 0,
	.hInstance = hInstance,
	.hIcon = LoadIcon(hInstance,IDI_WINLOGO),
	.hCursor = LoadCursor(nullptr, IDC_ARROW),
	.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
	.lpszMenuName = nullptr,
	.lpszClassName = g_pszWindowClassName,
	.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION),
	};

	if (!RegisterClassEx(&wcex))
	{
		DWORD dwError = GetLastError();
		MessageBox(
			nullptr,
			L"Call to RegisterClassEx failed!",
			L"Game Graphics Programming",
			NULL
		);
		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
		{
			return HRESULT_FROM_WIN32(dwError);
		}
		return E_FAIL;
	}

	g_hInstance = hInstance;
	RECT rc = { 0, 0, 1280, 720 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(
		g_pszWindowClassName,
		g_pszWindowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	ShowWindow(g_hWnd, nCmdShow);
	return S_OK;
}

HRESULT InitDevice()
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
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
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels,
			numFeatureLevels, D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1],
				numFeatureLevels - 1, D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel,
				&g_pImmediateContext);
		}
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	IDXGIFactory1* dxgiFactory = nullptr;
	IDXGIDevice* dxgiDevice = nullptr;
	hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	if (SUCCEEDED(hr))
	{
		IDXGIAdapter* adapter = nullptr;
		hr = dxgiDevice->GetAdapter(&adapter);
		if (SUCCEEDED(hr))
		{
			hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
			adapter->Release();
		}
		dxgiDevice->Release();
	}
	if (FAILED(hr))
		return hr;

	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2)
	{
		hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
		if (SUCCEEDED(hr))
		{
			(void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1),
				reinterpret_cast<void**>(&g_pImmediateContext1));
		}
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
		if (SUCCEEDED(hr))
		{
			hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain),
				reinterpret_cast<void**>(&g_pSwapChain));
		}
		dxgiFactory2->Release();
	}
	else {
		DXGI_SWAP_CHAIN_DESC sd = {};
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
		hr = dxgiFactory-> CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
	}
	dxgiFactory-> Release();
	if(FAILED(hr))
		return hr;

	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;
	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);


	// 1. vertex shader 컴파일
	ID3DBlob* pVertexShaderBlob = nullptr;
	hr = CompileShaderFromFile(L"../Library/Game/shader.fx", "VS", "vs_5_0", &pVertexShaderBlob);
	if (FAILED(hr))
	{
		pVertexShaderBlob->Release();
		return hr;
	}

	// 2. vertex shader 생성
	hr = g_pd3dDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVertexShaderBlob->Release();
		return hr;
	}

	// 3. input layout object 생성
	//D3D11_INPUT_ELEMENT_DESC layouts[]
	//{
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};

	D3D11_INPUT_ELEMENT_DESC layouts[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0,DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT uNumElements = ARRAYSIZE(layouts);
	hr = g_pd3dDevice->CreateInputLayout(
		layouts, uNumElements,
		pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(),
		&g_pVertexLayout);

	pVertexShaderBlob->Release();

	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	// 4. input layout binding
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	// 5. pixel shader 컴파일

	ID3DBlob* pPixelShaderBlob = nullptr;
	hr = CompileShaderFromFile(L"../Library/Game/shader.fx", "PS", "ps_5_0", &pPixelShaderBlob);
	if (FAILED(hr))
	{
		pPixelShaderBlob->Release();
		return hr;
	}

	// 6. pixel shader 생성

	hr = g_pd3dDevice->CreatePixelShader(
		pPixelShaderBlob->GetBufferPointer(),
		pPixelShaderBlob->GetBufferSize(),
		nullptr,
		&g_pPixelShader
	);

	if (FAILED(hr))
	{
		pPixelShaderBlob->Release();
		return hr;
	}

	// 7. vertex buffer 생성
	// 8. vertex buffer binding

	//SimpleVertex sVertices[] =
	//{
	//	{.Pos = XMFLOAT3(-1.0f, 1.0f, -1.0f),
	//		.Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
	//	{.Pos = XMFLOAT3(1.0f, 1.0f, -1.0f),
	//		.Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
	//	{.Pos = XMFLOAT3(1.0f, 1.0f, 1.0f),
	//		.Color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
	//	{.Pos = XMFLOAT3(-1.0f, 1.0f, 1.0f),
	//		.Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{.Pos = XMFLOAT3(-1.0f,-1.0f, -1.0f),
	//		.Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
	//	{.Pos = XMFLOAT3(1.0f, -1.0f, -1.0f),
	//		.Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
	//	{.Pos = XMFLOAT3(1.0f, -1.0f, 1.0f),
	//		.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
	//	{.Pos = XMFLOAT3(-1.0f, -1.0f, 1.0f),
	//		.Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	//};

	SimpleVertex sVertices[] = 
	{ 
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, 
		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, 
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }, 
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }, 
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, 
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, 
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, 
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }, 
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }, 
		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, 
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, 
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) }, 
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }, 
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }, 
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) }, 
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }, 
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }, 
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }, 
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, 
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, 
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, 
		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = sVertices;

	if (FAILED(hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer)))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	UINT uStride = sizeof(SimpleVertex);
	UINT uOffset = 0;

	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &uStride, &uOffset);

	//9. Index Butter
	ID3D11Buffer* g_pIndexBuffer = nullptr;
	//WORD sIndices[] = {
	//	3,1,0,
	//	2,1,3,
	//	0,5,4,
	//	1,5,0,
	//	3,4,7,
	//	0,4,3,
	//	1,6,5,
	//	2,6,1,
	//	2,7,6,
	//	3,7,2,
	//	6,4,5,
	//	7,4,6,
	//};

	WORD sIndices[] = {
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,
		
		11,9,8,
		10,9,11,
		
		14,12,13,
		15,12,14,
		
		19,17,16,
		18,17,19,
		
		22,20,21,
		23,20,22
	};

	
	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	initData = {};
	initData.pSysMem = sIndices;
	hr = g_pd3dDevice->CreateBuffer(
		&bd,
		&initData,
		&g_pIndexBuffer
	);
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	bd = {};
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;


	hr = g_pd3dDevice-> CreateBuffer(&bd, nullptr, &g_pCBView);
	if(FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pCBWorld);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pCBProjection);
	if (FAILED(hr))
		return hr;

	g_worldMatrix1 = XMMatrixIdentity();

	XMVECTOR eye1 = XMVectorSet(0.0f, 3.0f,-5.0f, 0.0f);
	XMVECTOR at1 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR up1 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	g_viewMatrix1 = XMMatrixLookAtLH(eye1, at1, up1);
	g_projectionMatrix1 = XMMatrixPerspectiveFovLH (XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

	//10. Depth Buffer
	D3D11_TEXTURE2D_DESC descDepth = {};
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
	hr = g_pd3dDevice-> CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
	if(FAILED(hr))
		return hr;

	
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	hr = CreateDDSTextureFromFile(g_pd3dDevice, L"../Library/Game/seafloor.dds", nullptr, &g_pTextureRV);
	if (FAILED(hr))
		return hr;
	
	D3D11_SAMPLER_DESC sampDesc = { };
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice-> CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if(FAILED(hr))
		return hr;
}

void CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain1) g_pSwapChain1->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext1) g_pImmediateContext1->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice1) g_pd3dDevice1->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	//if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pTextureRV) g_pTextureRV->Release();
	if (g_pSamplerLinear) g_pSamplerLinear-> Release();
	if (g_pCBView) g_pCBView-> Release();
	if(g_pCBProjection) g_pCBProjection-> Release();
	if(g_pCBWorld) g_pCBWorld-> Release();
}

HRESULT CompileShaderFromFile(_In_ PCWSTR pszFileName,_In_ PCSTR pszEntryPoint,	_In_ PCSTR pszShaderModel,_Outptr_ ID3DBlob** ppBlobOut) {
	HRESULT hr = S_OK;
	
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined (DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile
	(
		pszFileName, // FileName
		nullptr, // shader macros
		nullptr, // include files`
		pszEntryPoint, // Entry point
		pszShaderModel, // shader target
		dwShaderFlags, // flag1
		0, // flag2
		ppBlobOut, // ID3DBlob out 
		&pErrorBlob // error blob out
	);

	if(FAILED(hr))
	{
		if(pErrorBlob) {
			OutputDebugStringA(reinterpret_cast<PCSTR>(pErrorBlob-> GetBufferPointer()));
			pErrorBlob-> Release();
		}
		return hr;
	}
	if(pErrorBlob)
		pErrorBlob-> Release();
	return S_OK;
}

void Render()
{
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE) {
		t += (float)XM_PI * 0.0125f;
	}
	else {
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = (timeCur - timeStart) / 1000.0f;
	}

	g_worldMatrix1 = XMMatrixRotationY(t * 5);
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	XMMATRIX w = XMMatrixTranspose(g_worldMatrix1);
	XMMATRIX v = XMMatrixTranspose(g_viewMatrix1);
	XMMATRIX p = XMMatrixTranspose(g_projectionMatrix1);

	g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

	g_pImmediateContext->UpdateSubresource(g_pCBWorld, 0, nullptr, &w, 0, 0);
	g_pImmediateContext->UpdateSubresource(g_pCBView, 0, nullptr, &v, 0, 0);
	g_pImmediateContext->UpdateSubresource(g_pCBProjection, 0, nullptr, &p, 0, 0);

	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBView);
	g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBProjection);
	g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBWorld);

	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);

	g_pImmediateContext->DrawIndexed(36, 0, 0);
	g_pSwapChain->Present(0, 0);
}