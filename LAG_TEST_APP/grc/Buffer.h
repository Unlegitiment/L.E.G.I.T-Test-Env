#pragma once
#include "device.h"
#include <cassert>
#include <type_traits>
namespace lag {
	class grcBuffer {
	public:
		grcBuffer() = default;
		void Init(unsigned int amount, unsigned int stride, D3D11_USAGE usage, D3D11_BIND_FLAG bind, void* buffer) {
			D3D11_BUFFER_DESC pDsc = {};
			pDsc.Usage = usage; // check
			pDsc.ByteWidth = amount * stride;
			pDsc.BindFlags = bind;
			D3D11_SUBRESOURCE_DATA sub;
			sub.pSysMem = buffer;
			HRESULT hr = grcDeviceD3D11::Implementation()->m_pDevice->CreateBuffer(&pDsc, &sub, &m_pBuffer);
			assert(SUCCEEDED(hr));
		}
		void CloneItem(const grcBuffer& other) {
			this->m_pBuffer = other.m_pBuffer;
			if (this->m_pBuffer) {
				m_pBuffer->AddRef();
			}
		}
		void CloneItem(grcBuffer&& other) {
			m_pBuffer = other.m_pBuffer;
			other.m_pBuffer = nullptr;
		}
		grcBuffer(const grcBuffer& other) {
			CloneItem(other);
			printf("Copy construct");

		}
		grcBuffer& operator=(const grcBuffer& other) {
			if (this != &other) {
				if (m_pBuffer) m_pBuffer->Release();
				CloneItem(other);
			}
			printf("Copy assignment");
			return *this;
		}
		grcBuffer(grcBuffer&& other) noexcept {
			CloneItem(std::move(other));
		}
		grcBuffer& operator=(grcBuffer&& other) noexcept {
			if (this != &other) {
				if (m_pBuffer) m_pBuffer->Release();
				CloneItem(std::move(other));
			}
			return *this;
		}
		ID3D11Buffer* GetBuffer() { return this->m_pBuffer; }
		ID3D11Buffer** GetBufferPtr() { return &this->m_pBuffer; }
		void Destroy() {
			if (m_pBuffer) {
				m_pBuffer->Release();
				m_pBuffer = nullptr; // check.
			}
		}
		~grcBuffer() {
			Destroy();
		}
	private:
		ID3D11Buffer* m_pBuffer = nullptr;
	};
} // namespace lag 