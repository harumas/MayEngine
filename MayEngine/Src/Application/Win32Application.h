#pragma once
#include <windows.h>
#include <tchar.h>
#include "RenderPipeline.h"

class Win32Application
{
public:
	static void Run(const std::shared_ptr<RenderPipeline>& dxApp, HINSTANCE hInstance);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};