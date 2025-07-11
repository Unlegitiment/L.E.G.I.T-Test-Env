#pragma once
#include <wtypes.h>
#include <d3d11.h>
namespace lag {
	class grcWin32Setup {
	public:
		static constexpr const wchar_t grcWindow[] = L"grcWindow";
		static inline HINSTANCE g_hInstance = nullptr;
		static inline wchar_t gWindowTitle[64] = L"Project Legit";
		static inline bool s_bClosed = false;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			switch (msg) {
			case WM_CLOSE:
				s_bClosed = true;
				return 0;
			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		void Init() {
			if (!g_hInstance)
				g_hInstance = GetModuleHandle(NULL);
			WNDCLASS wc = {};
			wc.lpfnWndProc = WindowProc;
			wc.hInstance = g_hInstance;
			wc.lpszClassName = grcWindow;
			m_WndClass = ::RegisterClass(&wc);
			if (m_WndClass == 0) {
				DWORD code = GetLastError();
				__debugbreak();
			}
			HWND result = ::CreateWindowEx(0, grcWindow, gWindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, NULL, NULL, g_hInstance, NULL);
			if (!result) {
				DWORD code = GetLastError();
				__debugbreak();
			}
			::ShowWindow(result, SW_NORMAL);
			::UpdateWindow(result);
			::SetFocus(result);
			this->m_pWindow = result;

			m_bisInited = true;
		}
		bool IsInitialized() {
			return m_bisInited;
		}
		void* GetWindow() const { return m_pWindow; }
		HWND GetWindow() { return (HWND)m_pWindow; }
	private:
		unsigned int m_WndClass;
		void* m_pWindow;
		bool m_bisInited;
	};
	struct LEGITD3D11 {
		typedef ID3D11Device grcDeviceHandle;
		typedef ID3D11DeviceContext grcDeviceContextHandle;
		typedef IDXGISwapChain grcDeviceSwapchain;
		grcDeviceContextHandle* m_pContext = nullptr;
		grcDeviceHandle* m_pDevice = nullptr;
		grcDeviceSwapchain* m_pSwapChain = nullptr;
	};
#define GRCDEF static inline
	class grcDeviceD3D11 {
		typedef LEGITD3D11 grcHandle;
	public:
		static void Init() {
			sm_pSetup = new grcWin32Setup();
			sm_pSetup->Init();
			sm_pD3D11Vars = D3D11Init();
		}
		static void Destroy() {
			sm_pD3D11Vars->m_pContext->Release();
			sm_pD3D11Vars->m_pContext = nullptr;
			sm_pD3D11Vars->m_pDevice->Release();
			sm_pD3D11Vars->m_pDevice = nullptr;
			sm_pD3D11Vars->m_pSwapChain->Release();
			sm_pD3D11Vars->m_pSwapChain = nullptr;
			delete sm_pD3D11Vars;
			sm_pD3D11Vars = nullptr;
		}
		static grcHandle* Implementation() { return sm_pD3D11Vars; }
	private:
		static LEGITD3D11* D3D11Init() {
			LEGITD3D11* ret = new LEGITD3D11();
			D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };
			DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
			swapchaindesc.BufferDesc.Width = 0; // use window width
			swapchaindesc.BufferDesc.Height = 0; // use window height
			swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // can't specify SRGB framebuffer directly when using FLIP model swap effect. see lines 49, 66
			swapchaindesc.SampleDesc.Count = 1;
			swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchaindesc.BufferCount = 2;
			swapchaindesc.OutputWindow = sm_pSetup->GetWindow();
			swapchaindesc.Windowed = TRUE;
			swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &ret->m_pSwapChain, &ret->m_pDevice, nullptr, &ret->m_pContext); // D3D11_CREATE_DEVICE_DEBUG is optional, but provides useful d3d11 debug output
			if (hr != S_OK) {
				__debugbreak();

			}
			return ret;
		}
		GRCDEF grcWin32Setup* sm_pSetup = nullptr;
		GRCDEF LEGITD3D11* sm_pD3D11Vars = nullptr;
	};
}