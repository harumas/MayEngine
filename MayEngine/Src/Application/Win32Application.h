#pragma once
#include <windows.h>
#include <tchar.h>
#include "Application.h"

class Win32Application
{
public:
	static void Run(Application* dxApp, HINSTANCE hInstance);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};