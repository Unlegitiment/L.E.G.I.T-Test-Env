#pragma once
#include "Buffer.h"
namespace lag {
	class grcVertexDeclaration { // sample for now. represents an I.A Stage. Should be used in place of things that need stride information or general info about vert buffers. 
	public:
		int GetStride() const { return m_Stride; }
	private:
		int m_Stride;
	};
	class grcVertexBuffer {
	public:
		grcVertexBuffer(unsigned int sizeOfBuffer, void* buffer, const grcVertexDeclaration& declaration) {
			m_Buffer.Init(sizeOfBuffer, declaration.GetStride(), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, buffer);
		}
		void Bind() {
			unsigned int StridePtr = 0;
			unsigned int OffsetsPtr = 0;
			grcDeviceD3D11::Implementation()->m_pContext->IASetVertexBuffers(0, 1, m_Buffer.GetBufferPtr(), &StridePtr, &OffsetsPtr);
		}
		~grcVertexBuffer() {
		}
	private:
		
		grcBuffer m_Buffer;
	};
}