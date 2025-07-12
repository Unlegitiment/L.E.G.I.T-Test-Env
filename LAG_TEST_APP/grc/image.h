#pragma once
#include "device.h"
#include "../loaders.h"
#include <cassert>
namespace lag {
	class grcTexture2D {
	public:
		grcTexture2D(const char* fileName, D3D11_BIND_FLAG bind = D3D11_BIND_SHADER_RESOURCE) {
			auto* loader = CTheLoaders::CreateImage(fileName);
			auto& specs = loader->GetSpecs();
			Init(D3D11_USAGE_DEFAULT, bind, DXGI_FORMAT_R8G8B8A8_UNORM, specs.REQ_COMP, specs.X, specs.Y, loader->GetImage());
			delete loader;
		}
		grcTexture2D(D3D11_USAGE usage, D3D11_BIND_FLAG bind, DXGI_FORMAT format, int byteStride, unsigned int x, unsigned int y, void* data) {
			Init(usage, bind, format, byteStride, x, y, data);
		}
		void Init(D3D11_USAGE usage, D3D11_BIND_FLAG bind, DXGI_FORMAT format, int byteStride, unsigned int x, unsigned int y, void* data) {
			D3D11_TEXTURE2D_DESC desc{};
			desc.Usage = usage;
			desc.BindFlags = bind; // take as param.
			desc.Format = format;
			desc.Width = x;
			desc.Height = y;
			desc.MipLevels = 1; // check.  ? 
			desc.ArraySize = 1; // check 
			desc.SampleDesc.Count = 1;
			D3D11_SUBRESOURCE_DATA srd = {};
			srd.pSysMem = data;
			srd.SysMemPitch = x * byteStride; // bytestride == 4 typically what does it look like : 0xff'ff'ff'ff
			HRESULT hr = grcDeviceD3D11::Implementation()->m_pDevice->CreateTexture2D(&desc, &srd, &m_pRawTexture);
			assert(SUCCEEDED(hr));
			D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
			srvdesc.Format = format;
			srvdesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D; // check.
			srvdesc.Texture2D.MipLevels = 1;
			hr = grcDeviceD3D11::Implementation()->m_pDevice->CreateShaderResourceView(m_pRawTexture, &srvdesc, &m_SRV);
			assert(SUCCEEDED(hr));
			return;
		}
		grcTexture2D(const grcTexture2D& other) {
			this->m_Format = other.m_Format;
			if (other.m_pRawTexture) {
				other.m_pRawTexture->AddRef();
				this->m_pRawTexture = other.m_pRawTexture; // still kinda weird. 
			}
			if (other.m_SRV) {
				other.m_SRV->AddRef();
				this->m_SRV = other.m_SRV;
			}
		}
		grcTexture2D(grcTexture2D&& other) noexcept {
			this->m_Format = other.m_Format;

			this->m_pRawTexture = other.m_pRawTexture; // erm sigma. 
			other.m_pRawTexture = nullptr;

			this->m_SRV = other.m_SRV;
			other.m_SRV = nullptr;
		}
		grcTexture2D& operator=(grcTexture2D&& other) noexcept{
			if (this != &other) {
				if (m_pRawTexture)
					m_pRawTexture->Release();
				if (m_SRV)
					m_SRV->Release();

				m_Format = other.m_Format;
				m_pRawTexture = other.m_pRawTexture;
				m_SRV = other.m_SRV;

				other.m_pRawTexture = nullptr;
				other.m_SRV = nullptr;
			}
			return *this;
		}
		grcTexture2D& operator=(const grcTexture2D& other) {
			if (this != &other) {
				if (m_pRawTexture)
					m_pRawTexture->Release();
				if (m_SRV)
					m_SRV->Release();

				m_Format = other.m_Format;
				m_pRawTexture = other.m_pRawTexture;
				m_SRV = other.m_SRV;

				if (m_pRawTexture)
					m_pRawTexture->AddRef();
				if (m_SRV)
					m_SRV->AddRef();
			}
			return *this;
		}
		ID3D11ShaderResourceView* GetShaderResourceView() {
			return this->m_SRV;
		}
		ID3D11Texture2D* GetTexture() const { return m_pRawTexture; } 
		~grcTexture2D() {
			if (m_pRawTexture) {
				m_pRawTexture->Release();
				m_pRawTexture = nullptr;
			}
			if (m_SRV) {
				m_SRV->Release();
				m_pRawTexture = nullptr;
			}
		}
	private:
		ID3D11Texture2D* m_pRawTexture = nullptr;
		ID3D11ShaderResourceView* m_SRV = nullptr;
		DXGI_FORMAT m_Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM; // check 
	};
} // namespace lag;
