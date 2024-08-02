#pragma once
#include <string>

class AppInfo
{
public:
	static 	const wchar_t* GetTitle() { return title.c_str(); }
	static unsigned int GetWindowWidth() { return windowWidth; }
	static unsigned int GetWindowHeight() { return windowHeight; }
	static float GetWindowAspectRatio() { return static_cast<float>(windowWidth) / static_cast<float>(windowHeight); }

	static void SetWindowTitle(const std::wstring& title)
	{
		AppInfo::title = title;
	}

	static void SetWindowSize(unsigned int width, unsigned int height)
	{
		windowWidth = width;
		windowHeight = height;
	}

private:
	static std::wstring title;
	static unsigned int windowWidth;
	static unsigned int windowHeight;
};
