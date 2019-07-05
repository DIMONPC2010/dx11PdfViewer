#pragma once
#include "stdafx.h"
#include "PageBuilder.h"
#include "InputListener.h"
#include "Window.h"
#include "SearchEngine.h"

namespace MWRL = Microsoft::WRL;

struct AnimationCounter
{
	float cTR;
	float cW;
	float cH;
};

class Render : public InputListener
{
private:
	typedef std::shared_ptr<PageBuilder> PageBuilder_t;

public:
	bool KeyPressed(const KeyEvent &arg);
	
	Render();
	~Render();

	bool Init(HWND hwnd);
	bool CreateDevice(HWND hwnd);
	void BeginFrame(DescWindow s);
	void EndFrame();
	bool Draw();
	void RenderDocument(PageBuilder_t pages);
	void SetSearchPages(PageBuilder_t pages);
	PageBuilder_t GetDocument();

	void* operator new(size_t i)
	{
		return _aligned_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

private:
	HRESULT compileshaderfromfile(const WCHAR* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut);
	HRESULT createOnePage(bool push_back, bool push_front);
	HRESULT createStartPages(int i);
	std::wstring getModuleShaderName();
	bool animationRight();
	bool animationLeft();
	void animCountersSetDefault();
	void setNextPosValues();
	void setStartPosValues();

	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;
	MWRL::ComPtr<ID3D11Device> m_pd3dDevice;
	MWRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;
	MWRL::ComPtr<IDXGISwapChain> m_pSwapChain;
	MWRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;

	MWRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	MWRL::ComPtr<ID3D11InputLayout> m_pVertexLayout;
	MWRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	MWRL::ComPtr<ID3D11PixelShader> m_pPixelShader;

	DirectX::XMMATRIX m_World[PAGE_NUM];
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Projection;

	MWRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	MWRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;

	MWRL::ComPtr<ID3D11Texture2D> m_pDepthStencil;
	MWRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	MWRL::ComPtr<ID3D11ShaderResourceView> m_pTextureRV[PAGE_NUM];
	MWRL::ComPtr<ID3D11SamplerState> m_pSamplerLinear;

	DescWindow m_winsize;

	PageBuilder_t m_Doc;

	int m_pagesNum;
	int m_nowPage;

	bool m_left;
	bool m_right;
	bool m_day;
	bool m_night;

	AnimationCounter m_acounters[PAGE_NUM];
	AnimationCounter m_startpos[PAGE_NUM];
	AnimationCounter m_nextpos[PAGE_NUM];
	int m_div[PAGE_NUM];
	DirectX::XMMATRIX Translation[PAGE_NUM];
	DirectX::XMMATRIX Scale[PAGE_NUM];

};