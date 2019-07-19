#include "Render.h"


using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct ConstantBuffer
{
	XMMATRIX  WVP;
	float vInversion;
};

bool Render::KeyPressed(const KeyEvent & arg)
{
	if (arg.code == KEY_LEFT)
	{
		m_left = true;
		m_right = false;
	}
	if (arg.code == KEY_RIGHT)
	{
		m_right = true;
		m_left = false;
	}
	if (arg.code == KEY_D)
	{
		m_day = true;
		m_night = false;
	}
	if (arg.code == KEY_N)
	{
		m_night = true;
		m_day = false;
	}
	return false;
}


Render::Render() :
	m_pagesNum(PAGE_NUM)
	, m_left(false)
	, m_right(false)
	, m_day(true)
	, m_night(false)
	, m_bookmark_startpage(0)
{
	m_driverType = D3D_DRIVER_TYPE_NULL;
	m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	m_pd3dDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pSwapChain = nullptr;
	m_pRenderTargetView = nullptr;
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexLayout = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pConstantBuffer = nullptr;
	m_pDepthStencil = nullptr;
	m_pDepthStencilView = nullptr;
	m_pSamplerLinear = nullptr;
	m_pTextureWithBookmark[0] = nullptr;
	m_pTextureWithBookmark[1] = nullptr;

	for (int i = 0; i < m_pagesNum; i++)
		m_pTextureRV[i] = nullptr;

	animationInit();
}

HRESULT Render::compileshaderfromfile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	MWRL::ComPtr<ID3DBlob> pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
							dwShaderFlags, 0, ppBlobOut, pErrorBlob.GetAddressOf());
	
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
		}
		return hr;
	}

	return hr;
}

Render::~Render()
{
}

bool Render::Init(HWND hwnd)
{
	HRESULT hr = S_OK;
	MWRL::ComPtr<ID3DBlob> pVSBlob = nullptr;
	auto path = getModuleShaderName();
	hr = compileshaderfromfile(path.c_str(), "VS", "vs_4_0", pVSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Невозможно скомпилировать файл shader.fx. Пожалуйста, запустите данную программу из папки, содержащей этот файл", L"Error", MB_OK);
		return false;
	}

	hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = m_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_pVertexLayout.GetAddressOf());
	if (FAILED(hr))
		return false;

	m_pImmediateContext->IASetInputLayout(m_pVertexLayout.Get());

	MWRL::ComPtr<ID3DBlob> pPSBlob = NULL;

	hr = compileshaderfromfile(path.c_str(), "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Невозможно скомпилировать файл shader.fx. Пожалуйста, запустите данную программу из папки, содержащей этот файл", L"Error", MB_OK);
		return false;
	}

	hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf());
	if (FAILED(hr))
		return false;

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, m_pVertexBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	WORD indices[] =
	{
		0,1,2,
		2,1,3
	};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, m_pIndexBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = m_pd3dDevice->CreateBuffer(&bd, NULL, m_pConstantBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
	if (FAILED(hr))
		return false;

	for(int i = 0; i < m_pagesNum; i++)
		m_World[i] = XMMatrixIdentity();


	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1.5f, 1.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)1067 / 600, 0.1f, 1000.0f);

	return true;

}

bool Render::CreateDevice(HWND hwnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hwnd, &rc);
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
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd, m_pSwapChain.GetAddressOf(), m_pd3dDevice.GetAddressOf(), &m_featureLevel, m_pImmediateContext.GetAddressOf());
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return false;

	MWRL::ComPtr<ID3D11Texture2D> pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)pBackBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, &m_pRenderTargetView);
	if (FAILED(hr))
		return false;

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
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, nullptr, m_pDepthStencil.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil.Get(), &descDSV, m_pDepthStencilView.GetAddressOf());
	if (FAILED(hr))
		return false;

	m_pImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports(1, &vp);

	return Init(hwnd);
}

void Render::BeginFrame(DescWindow s)
{
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), ClearColor);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_winsize = s;
}

void Render::EndFrame()
{
	m_pSwapChain->Present(0, 0);
}

bool Render::Draw()
{
	m_Projection = XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)m_winsize.width / m_winsize.height, 0.1f, 1000.0f);


	Translation[0] = XMMatrixTranslation(m_acounters[0].cTR, 0.0f, 0.0f);
	Scale[0] = XMMatrixScaling(m_acounters[0].cW, m_acounters[0].cH, 0.0f);

	Translation[1] = XMMatrixTranslation(m_acounters[1].cTR, 0.0f, 0.0f);
	Scale[1] = XMMatrixScaling(m_acounters[1].cW, m_acounters[1].cH, 0.0f);

	Translation[2] = XMMatrixTranslation(m_acounters[2].cTR, 0.0f, 0.0f);
	Scale[2] = XMMatrixScaling(m_acounters[2].cW, m_acounters[2].cH, 0.0f);

	Translation[3] = XMMatrixTranslation(m_acounters[3].cTR, 0.0f, 0.0f);
	Scale[3] = XMMatrixScaling(m_acounters[3].cW, m_acounters[3].cH, 0.0f);

	Translation[4] = XMMatrixTranslation(m_acounters[4].cTR, 0.0f, 0.0f);
	Scale[4] = XMMatrixScaling(m_acounters[4].cW, m_acounters[4].cH, 0.0f);

	Translation[5] = XMMatrixTranslation(m_acounters[5].cTR, 0.0f, 0.0f);
	Scale[5] = XMMatrixScaling(m_acounters[5].cW, m_acounters[5].cH, 0.0f);

	Translation[6] = XMMatrixTranslation(m_acounters[6].cTR, 0.0f, 0.0f);
	Scale[6] = XMMatrixScaling(m_acounters[6].cW, m_acounters[6].cH, 0.0f);

	m_World[0] = Scale[0] * Translation[0];

	m_World[1] = Scale[1] * Translation[1];
	m_World[2] = Scale[2] * Translation[2];

	m_World[3] = Scale[3] * Translation[3];
	m_World[4] = Scale[4] * Translation[4];
	m_World[5] = Scale[5] * Translation[5];
	m_World[6] = Scale[6] * Translation[6];
	

	if (m_right == true && m_Doc->NowView() < m_Doc->size() - 1)
	{
		if (animationRight())
		{
			createOnePage(true, false);
			animCountersSetDefault();
			m_right = false;
		}
	}

	if (m_left == true && m_Doc->NowView() > 0)
	{
		if (animationLeft())
		{
			createOnePage(false, true);
			animCountersSetDefault();
			m_left = false;
		}
	}


	XMMATRIX WVP;
	ConstantBuffer cb;

	if(m_day)
		cb.vInversion = 0.0f;
	if(m_night)
		cb.vInversion = 1.0f;
	
	int worldC, end;

	setupRenderCounters(&worldC, &end);

	if (m_left)
	{
		int tmp = worldC;
		worldC = end - 1;
		end = tmp - 1;
		while (worldC > end)
		{
			m_pTextureWithBookmark[0] = m_pTextureRV[worldC];
			if (m_Doc->Bookmark(worldC))
				m_pTextureWithBookmark[1] = m_pTextureFullBM;
			else
				m_pTextureWithBookmark[1] = m_pTextureNullBM;
			WVP = m_World[worldC] * m_View * m_Projection;
			cb.WVP = XMMatrixTranspose(WVP);
			m_pImmediateContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
			m_pImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
			m_pImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
			m_pImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
			m_pImmediateContext->PSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
			m_pImmediateContext->PSSetShaderResources(0, 2, m_pTextureWithBookmark->GetAddressOf());
			m_pImmediateContext->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());
			m_pImmediateContext->DrawIndexed(6, 0, 0);
			worldC--;
		}
	}
	else
	{
		while (worldC < end)
		{
			m_pTextureWithBookmark[0] = m_pTextureRV[worldC];
			if (m_Doc->Bookmark(worldC))
				m_pTextureWithBookmark[1] = m_pTextureFullBM;
			else
				m_pTextureWithBookmark[1] = m_pTextureNullBM;
			WVP = m_World[worldC] * m_View * m_Projection;
			cb.WVP = XMMatrixTranspose(WVP);
			m_pImmediateContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
			m_pImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
			m_pImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
			m_pImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
			m_pImmediateContext->PSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
			m_pImmediateContext->PSSetShaderResources(0, 2, m_pTextureWithBookmark->GetAddressOf());
			m_pImmediateContext->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());
			m_pImmediateContext->DrawIndexed(6, 0, 0);
			worldC++;
		}
	}
	return true;
}


void Render::RenderDocument(PageBuilder_t pages)
{
	m_Doc = pages;
	m_left = false;
	m_right = false;
	animationInit();

	for (int i = 0; i < m_pagesNum; i++)
		createStartPages(i);

	HRESULT hr = CreateWICTextureFromFile(m_pd3dDevice.Get(), L"D:\\work\\Projects\\dx11pdfReader_v3\\dx11pdfReader_v3\\images\\bookmark.png", nullptr, m_pTextureFullBM.GetAddressOf());
	if (FAILED(hr))
		MessageBox(nullptr, L"Error", L"Не удалось создать текстуру", MB_OK);
	hr = CreateWICTextureFromFile(m_pd3dDevice.Get(), L"D:\\work\\Projects\\dx11pdfReader_v3\\dx11pdfReader_v3\\images\\nullbookmark.png", nullptr, m_pTextureNullBM.GetAddressOf());
	if (FAILED(hr))
		MessageBox(nullptr, L"Error", L"Не удалось создать текстуру", MB_OK);

	for (int i = 0; i < m_pagesNum; i++)
	{
		m_acounters[i].cH /= m_div[i];
		m_acounters[i].cW /= m_div[i];
		m_startpos[i].cH /= m_div[i];
		m_startpos[i].cW /= m_div[i];
	}

	setNextPosValues();
	m_nowPage = PAGE_NUM;
}

void Render::SetSearchPages(PageBuilder_t pages)
{
	m_Doc = pages;

	for (int i = 0; i < m_pagesNum; i++)
		createStartPages(i);
	
}

std::shared_ptr<PageBuilder> Render::GetDocument()
{
	return m_Doc;
}

void Render::ViewBookmark(int page_num)
{
	if (page_num > m_Doc->NowView())
	{
		while (m_Doc->NowView() != page_num)
		{
			createOnePage(true, false);
		}
		m_Doc->SetBookmark(true);
	}
	else if (page_num < m_Doc->NowView())
	{
		while (m_Doc->NowView() != page_num)
		{
			createOnePage(false, true);
		}
		m_Doc->SetBookmark(true);
	}
}

void Render::ReturnFromBookmark(int start_page)
{
	ViewBookmark(start_page);
	m_Doc->SetBookmark(false);
	//m_bookmark_startpage = 0;
}

HRESULT Render::createOnePage(bool push_back, bool push_front)
{
	HRESULT hr = S_OK;
	int bpp = 4;
	int nb_color = 4;

	int nowPage = 0;
	if (push_back)
	{
		m_Doc->GetNext();
		nowPage = m_nowPage - 1;
	}
	if (push_front)
	{
		m_Doc->GetPrevious();
		nowPage = m_nowPage - PAGE_NUM;
	}


	D3D11_SUBRESOURCE_DATA tbsd;

	tbsd.pSysMem = (void *)m_Doc->GetPage(nowPage);
	tbsd.SysMemPitch = m_Doc->width(nowPage) * bpp;
	tbsd.SysMemSlicePitch = m_Doc->width(nowPage) * m_Doc->height(nowPage) * bpp;

	D3D11_TEXTURE2D_DESC desc = {};

	desc.Width = m_Doc->width(nowPage);
	desc.Height = m_Doc->height(nowPage);
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	hr = m_pd3dDevice->CreateTexture2D(&desc, nullptr, tex.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = -1;

		if (push_back == true)
		{
			for (int i = 0; i < m_pagesNum - 1; i++)
				m_pTextureRV[i] = m_pTextureRV[i + 1];
			hr = m_pd3dDevice->CreateShaderResourceView(tex.Get(), &SRVDesc, m_pTextureRV[m_pagesNum - 1].GetAddressOf());
			m_nextpos[m_pagesNum - 1].cH = m_Doc->height(nowPage) * m_nextpos[m_pagesNum - 1].cW / m_Doc->width(nowPage);
			m_pImmediateContext->UpdateSubresource(tex.Get(), 0, 0, tbsd.pSysMem, tbsd.SysMemPitch, 0);
			m_pImmediateContext->GenerateMips(m_pTextureRV[m_pagesNum - 1].Get());
		}
		if (push_front == true)
		{
			for (int i = m_pagesNum - 1; i > 0; i--)
				m_pTextureRV[i] = m_pTextureRV[i - 1];
			hr = m_pd3dDevice->CreateShaderResourceView(tex.Get(), &SRVDesc, m_pTextureRV[0].GetAddressOf());
			m_startpos[0].cH = m_Doc->height(nowPage) * m_startpos[0].cW / m_Doc->width(nowPage);
			m_pImmediateContext->UpdateSubresource(tex.Get(), 0, 0, tbsd.pSysMem, tbsd.SysMemPitch, 0);
			m_pImmediateContext->GenerateMips(m_pTextureRV[0].Get());
		}
	}

	

	if (FAILED(hr))
		return hr;
	return hr;
}

HRESULT Render::createStartPages(int i)
{
	HRESULT hr = S_OK;
	int bpp = 4;
	int nb_color = 4;

	if (m_Doc->Bookmark(i))
		m_pTextureRV[PAGE_NUM] = m_pTextureWithBookmark[1];

	D3D11_SUBRESOURCE_DATA tbsd;

	tbsd.pSysMem = (void *)m_Doc->GetPage(i);
	tbsd.SysMemPitch = m_Doc->width(i) * bpp;
	tbsd.SysMemSlicePitch = m_Doc->width(i) * m_Doc->height(i) * bpp;

	D3D11_TEXTURE2D_DESC desc = {};

	desc.Width = m_Doc->width(i);
	desc.Height = m_Doc->height(i);
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;


	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	hr = m_pd3dDevice->CreateTexture2D(&desc, nullptr, tex.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = -1;

		hr = m_pd3dDevice->CreateShaderResourceView(tex.Get(), &SRVDesc, m_pTextureRV[i].GetAddressOf());
		m_pImmediateContext->UpdateSubresource(tex.Get(), 0, 0, tbsd.pSysMem, tbsd.SysMemPitch, 0);
		m_pImmediateContext->GenerateMips(m_pTextureRV[i].Get());
	}

	m_acounters[i].cH = m_Doc->height(i) * m_acounters[i].cW / m_Doc->width(i);
	m_startpos[i].cH = m_Doc->height(i) * m_startpos[i].cW / m_Doc->width(i);


	if (FAILED(hr))
		return hr;
	return hr;
}

std::wstring Render::getModuleShaderName()
{
	wchar_t szPath[MAX_PATH];
	DWORD code = GetModuleFileNameW(NULL, szPath, MAX_PATH);
	if (code == 0)
	{
		MessageBox(nullptr, L"Невозможно получить путь к файлу shader.fx", L"Error", MB_OK);
	}
	else
	{
		wchar_t shader[10] = L"shader.fx";

		int c = code;
		while (szPath[c] != '\\')
			c--;

		int j = 0;
		for (int i = c + 1; i < c + 11; i++)
		{
			szPath[i] = shader[j];
			j++;
		}


		szPath[c + 12] = '\0';
	}
	return szPath;
}

bool Render::animationRight()
{
		m_acounters[1].cTR -= 0.001f;
	if (m_acounters[1].cW > m_nextpos[0].cW)
		m_acounters[1].cW -= 0.001f;
	if (m_acounters[1].cH > m_nextpos[0].cH)
		m_acounters[1].cH -= 0.001f;

	if (m_acounters[2].cTR > m_nextpos[1].cTR)
		m_acounters[2].cTR -= 0.001f;
	if (m_acounters[2].cW > m_nextpos[1].cW)
		m_acounters[2].cW -= 0.001f;
	if (m_acounters[2].cH > m_nextpos[1].cH)
		m_acounters[2].cH -= 0.001f;

	if (m_acounters[3].cTR > m_nextpos[2].cTR)
		m_acounters[3].cTR -= 0.001f;
	if (m_acounters[3].cW > m_nextpos[2].cW)
		m_acounters[3].cW -= 0.001f;
	if (m_acounters[3].cH > m_nextpos[2].cH)
		m_acounters[3].cH -= 0.001f;

	if (m_acounters[4].cTR > m_nextpos[3].cTR)
		m_acounters[4].cTR -= 0.001f;
	if (m_acounters[4].cW < m_nextpos[3].cW)
		m_acounters[4].cW += 0.001f;
	if (m_acounters[4].cH < m_nextpos[3].cH)
		m_acounters[4].cH += 0.001f;

	if (m_acounters[5].cTR > m_nextpos[4].cTR)
		m_acounters[5].cTR -= 0.001f;
	if (m_acounters[5].cW < m_nextpos[4].cW)
		m_acounters[5].cW += 0.001f;
	if (m_acounters[5].cH < m_nextpos[4].cH)
		m_acounters[5].cH += 0.001f;

	if (m_acounters[6].cTR > m_nextpos[5].cTR)
		m_acounters[6].cTR -= 0.001f;
	if (m_acounters[6].cW < m_nextpos[5].cW)
		m_acounters[6].cW += 0.001f;
	if (m_acounters[6].cH < m_nextpos[5].cH)
		m_acounters[6].cH += 0.001f;

	if (m_acounters[4].cTR > m_nextpos[3].cTR)
		return false;
	else
		return true;
}

bool Render::animationLeft()
{
	if (m_acounters[0].cTR < m_startpos[1].cTR)
		m_acounters[0].cTR += 0.001f;
	if (m_acounters[0].cW < m_startpos[1].cW)
		m_acounters[0].cW += 0.001f;
	if (m_acounters[0].cH < m_startpos[1].cH)
		m_acounters[0].cH += 0.001f;

	if (m_acounters[1].cTR < m_startpos[2].cTR)
		m_acounters[1].cTR += 0.001f;
	if (m_acounters[1].cW < m_startpos[2].cW)
		m_acounters[1].cW += 0.001f;
	if (m_acounters[1].cH < m_startpos[2].cH)
		m_acounters[1].cH += 0.001f;

	if (m_acounters[2].cTR < m_startpos[3].cTR)
		m_acounters[2].cTR += 0.001f;
	if (m_acounters[2].cW < m_startpos[3].cW)
		m_acounters[2].cW += 0.001f;
	if (m_acounters[2].cH < m_startpos[3].cH)
		m_acounters[2].cH += 0.001f;

	if (m_acounters[3].cTR < m_startpos[4].cTR)
		m_acounters[3].cTR += 0.001f;
	if (m_acounters[3].cW > m_startpos[4].cW)
		m_acounters[3].cW -= 0.001f;
	if (m_acounters[3].cH > m_startpos[4].cH)
		m_acounters[3].cH -= 0.001f;

	if (m_acounters[4].cTR < m_startpos[5].cTR)
		m_acounters[4].cTR += 0.001f;
	if (m_acounters[4].cW > m_startpos[5].cW)
		m_acounters[4].cW -= 0.001f;
	if (m_acounters[4].cH > m_startpos[5].cH)
		m_acounters[4].cH -= 0.001f;

	//if (m_acounters[5].cTR < m_startpos[6].cTR)
		m_acounters[5].cTR += 0.001f;
	if (m_acounters[5].cW < m_startpos[6].cW)
		m_acounters[5].cW += 0.001f;
	if (m_acounters[5].cH < m_startpos[6].cH)
		m_acounters[5].cH += 0.001f;



	if (m_acounters[3].cTR < m_startpos[4].cTR)
		return false;
	else
		return true;
}

void Render::animCountersSetDefault()
{
	if (m_right)
	{
		for (int i = 0; i < m_pagesNum; i++)
			m_acounters[i] = m_nextpos[i];

		setStartPosValues();
		setNextPosValues();
	}

	if (m_left)
	{
		for (int i = 0; i < m_pagesNum; i++)
			m_acounters[i] = m_startpos[i];

		setStartPosValues();
		setNextPosValues();
	}
}

void Render::setNextPosValues()
{
	m_nextpos[0].cW = m_acounters[1].cW / 2;
	m_nextpos[1].cW = m_acounters[2].cW / 2;
	m_nextpos[2].cW = m_acounters[3].cW / 2;
	m_nextpos[3].cW = m_acounters[4].cW * 2;
	m_nextpos[4].cW = m_acounters[5].cW * 2;
	m_nextpos[5].cW = m_acounters[6].cW * 2;
	m_nextpos[6].cW = m_acounters[6].cW;

	m_nextpos[0].cH = m_acounters[1].cH / 2;
	m_nextpos[1].cH = m_acounters[2].cH / 2;
	m_nextpos[2].cH = m_acounters[3].cH / 2;
	m_nextpos[3].cH = m_acounters[4].cH * 2;
	m_nextpos[4].cH = m_acounters[5].cH * 2;
	m_nextpos[5].cH = m_acounters[6].cH * 2;
	m_nextpos[6].cH = m_acounters[6].cH;
}

void Render::setStartPosValues()
{
	m_startpos[0].cW = m_acounters[0].cW;
	m_startpos[1].cW = m_acounters[0].cW * 2;
	m_startpos[2].cW = m_acounters[1].cW * 2;
	m_startpos[3].cW = m_acounters[2].cW * 2;
	m_startpos[4].cW = m_acounters[3].cW / 2;
	m_startpos[5].cW = m_acounters[4].cW / 2;
	m_startpos[6].cW = m_acounters[5].cW / 2;

	m_startpos[0].cH = m_acounters[0].cH;
	m_startpos[1].cH = m_acounters[0].cH * 2;
	m_startpos[2].cH = m_acounters[1].cH * 2;
	m_startpos[3].cH = m_acounters[2].cH * 2;
	m_startpos[4].cH = m_acounters[3].cH / 2;
	m_startpos[5].cH = m_acounters[4].cH / 2;
	m_startpos[6].cH = m_acounters[5].cH / 2;
}

void Render::animationInit()
{
	m_acounters[0].cTR = -1.925f;
	m_acounters[1].cTR = -1.65f;
	m_acounters[2].cTR = -1.1f;
	m_acounters[3].cTR = 0.0f;
	m_acounters[4].cTR = 1.1f;
	m_acounters[5].cTR = 1.65f;
	m_acounters[6].cTR = 1.925f;

	for (int i = 0; i < m_pagesNum; i++)
		m_acounters[i].cW = 0.71f;

	m_div[0] = 8;
	m_div[1] = 4;
	m_div[2] = 2;
	m_div[3] = 1;
	m_div[4] = 2;
	m_div[5] = 4;
	m_div[6] = 8;

	for (int i = 0; i < m_pagesNum; i++)
	{
		m_startpos[i] = m_acounters[i];
		m_nextpos[i] = m_acounters[i];
	}
}

void Render::setupRenderCounters(int * worldC, int * end)
{
	*end = m_pagesNum - 1;
	if (m_right)
		*end = m_pagesNum;

	*worldC = 1;
	if (m_left)
		*worldC = 0;

	if (m_Doc->NowView() == 0)
		*worldC = 3;
	else if (m_Doc->NowView() == 1)
		*worldC = 2;
	else if (m_Doc->NowView() == 2)
		*worldC = 1;
	else if (m_Doc->NowView() == m_Doc->size() - 3)
		*end = m_pagesNum - 1;
	else if (m_Doc->NowView() == m_Doc->size() - 2)
		*end = m_pagesNum - 2;
	else if (m_Doc->NowView() == m_Doc->size() - 1)
		*end = m_pagesNum - 3;

	if (m_Doc->size() == 4 && m_Doc->NowView() == 0 && m_right == false)
		*end = m_Doc->size() + 2;
	else if (m_Doc->size() == 4 && m_Doc->NowView() == m_Doc->size() - 1)
		*end = m_Doc->size();

	else if (m_Doc->size() == 5 && m_Doc->NowView() == m_Doc->size() - 3 && m_right == true)
		*end = m_pagesNum - 1;


	else if (m_Doc->size() < PAGE_NUM - 2)
		*end = m_Doc->size() + *worldC;
}
