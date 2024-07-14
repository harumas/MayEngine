#pragma once
#include <windows.h>
#include "GameApplication.h"

class Win32Application
{
public:
	static void Run(GameApplication& gameApp, HINSTANCE hInstance);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};