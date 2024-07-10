#pragma once
#include "DXTexFix.h"
#include "RenderPipeline.h"

class GameApplication
{
public:
	GameApplication(unsigned int width, unsigned int height, const std::wstring& title) {  } 
	 
	void Init(HWND hwnd);
	void Update();
	void Render();
	void Destroy();
	 
	const WCHAR* GetTitle() const { return title_.c_str(); }
	unsigned int GetWindowWidth() const { return windowWidth_; }
	unsigned int GetWindowHeight() const { return windowHeight_; }
private:
	std::wstring title_;
	unsigned int windowWidth_;
	unsigned int windowHeight_;
 
	shared_ptr<RenderPipeline> renderPipeline;
};
