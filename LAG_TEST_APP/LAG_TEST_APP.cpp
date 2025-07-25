#pragma once
#include <d3d11.h>
#include <stdexcept>
#include <cassert>
#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler
#include "grc/Buffer.h"
#include "grc/device.h"
#include "grc\image.h"
void Win32Common() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
class CRenderer {
public:
	int BufferTest[8] = { 1, 2, 4, 5, 6,7 ,8 , 9 };
	void Setup() {
		HRESULT hr;
		buffer = lag::grcBuffer();
		buffer.Init(8, sizeof(int), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, BufferTest);
		lag::grcBuffer buf2 = buffer; // this should call copy. 
		printf("Test");
		ID3D11Texture2D* frameBuf;
		hr = lag::grcDeviceD3D11::Implementation()->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&frameBuf);
		assert(SUCCEEDED(hr));
		m_ViewPtr = nullptr;
		hr = lag::grcDeviceD3D11::Implementation()->m_pDevice->CreateRenderTargetView(frameBuf, nullptr, &m_ViewPtr);
		frameBuf->Release();
		assert(SUCCEEDED(hr));
		assert(m_ViewPtr && "view is not setup");
		lag::grcDeviceD3D11::Implementation()->m_pContext->ClearRenderTargetView(m_ViewPtr, fColor);
		lag::grcDeviceD3D11::Implementation()->m_pContext->OMSetRenderTargets(1, &m_ViewPtr, nullptr); // no depth yet. 
	}
	void DoRender() {
		lag::grcDeviceD3D11::Implementation()->m_pContext->ClearRenderTargetView(m_ViewPtr, fColor);
		lag::grcDeviceD3D11::Implementation()->m_pContext->PSSetConstantBuffers(0, 1, buffer.GetBufferPtr()); // this will likely fail. 
		ID3D11ShaderResourceView* srv = nullptr;
		srv = this->myTexture.GetShaderResourceView();
		lag::grcDeviceD3D11::Implementation()->m_pContext->PSSetShaderResources(0, 1, &srv);
	}
	void Shutdown() {
		HRESULT hr;
		lag::grcDeviceD3D11::Implementation()->m_pContext->ClearState();
		lag::grcDeviceD3D11::Implementation()->m_pContext->Flush();
		m_ViewPtr->Release();
		buffer.Destroy();
		//buf2.Destroy();
		ID3D11Debug* m_Debugger = nullptr;
		hr = lag::grcDeviceD3D11::Implementation()->m_pDevice->QueryInterface(&m_Debugger);
		assert(SUCCEEDED(hr));
		if (m_Debugger) {
			m_Debugger->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
			m_Debugger->Release();
		}
		// Unbind render targets from pipeline before releasing the RTV.

		//m_ViewPtr->Release();
		lag::grcDeviceD3D11::Destroy();
	}
private:
	const FLOAT fColor[4] = { 0.f,0.f,0.f,1.f };
	ID3D11RenderTargetView* m_ViewPtr = nullptr;
	lag::grcBuffer buffer;
	lag::grcTexture2D myTexture = lag::grcTexture2D("W:\\GTAV Scripts\\LAG_TEST_APP\\LAG_TEST_APP\\sampleassets\\char_social_club.jpg");

};
int main(char** argc, int argv) {
	HRESULT hr = S_OK;
	lag::grcDeviceD3D11::Init();
	CRenderer renderer;
	renderer.Setup();
	while (!lag::grcWin32Setup::s_bClosed) {
		Win32Common();
		renderer.DoRender();
		
		lag::grcDeviceD3D11::Implementation()->m_pSwapChain->Present(1, 0);
		Sleep(1);
	}
	renderer.Shutdown();
}