#pragma once
class CImageLoader {
public:
	struct sImageSpecs {
		int X;
		int Y;
		int COMP;
		int REQ_COMP;
	};
	struct sInput {
		const char* path;
		int requiredComp = 4;
	};
	CImageLoader() = default;
	CImageLoader(const CImageLoader&) = delete;
	CImageLoader& operator=(const CImageLoader&)= delete;
	CImageLoader(CImageLoader&&)noexcept = delete;
	CImageLoader& operator=(CImageLoader&&)noexcept = delete;
	virtual ~CImageLoader() = default;
	virtual void* GetImage() = 0;
	virtual sImageSpecs& GetSpecs() = 0;
};
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h" // move
class CSTBILoad : public CImageLoader {
public:
	CSTBILoad(const sInput& input) {
		int x=0, y=0, actual_comp=0;
		this->m_Image = stbi_load(input.path, &x, &y, &actual_comp, input.requiredComp);
		m_ImageSpecs =  sImageSpecs();
		m_ImageSpecs.X = x;
		m_ImageSpecs.Y = y;
		m_ImageSpecs.COMP = actual_comp;
		m_ImageSpecs.REQ_COMP = input.requiredComp;
		if (!m_Image) {
			auto mFailReason = stbi_failure_reason();
			printf("[LOADER][IMAGE_LOADER]: STBI_FAILED for reason: %s", mFailReason);
			__debugbreak();
		}
	}
	CSTBILoad(const CSTBILoad&) = delete;
	CSTBILoad& operator=(const CSTBILoad&) = delete;
	CSTBILoad(CSTBILoad&&) noexcept = delete;
	CSTBILoad& operator=(CSTBILoad&&)noexcept = delete;
	void* GetImage() override {
		return m_Image;	
	}
	sImageSpecs& GetSpecs() override{ return m_ImageSpecs;  }
	~CSTBILoad() {
		if (m_Image) stbi_image_free(m_Image); m_Image = nullptr;
	}
private:
	void* m_Image = nullptr;
	CSTBILoad::sImageSpecs m_ImageSpecs{};
};
class CTheLoaders {
public:
	using ImageLoader = CSTBILoad;
	static ImageLoader* CreateImage(const char* fileName) {
		return new ImageLoader({fileName});
	}
private:

};