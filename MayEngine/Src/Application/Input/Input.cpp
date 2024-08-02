#include "Input.h"


LPDIRECTINPUT8 Input::g_pDI = nullptr;
LPDIRECTINPUTDEVICE8 Input::g_pKeyboard = nullptr;
LPDIRECTINPUTDEVICE8 Input::g_pMouse = nullptr;
BYTE Input::g_KeyboardState[256] = { 0 };
DIMOUSESTATE Input::g_MouseState = { 0 };

bool Input::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	ThrowIfFailed(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&g_pDI), NULL));

	ThrowIfFailed(g_pDI->CreateDevice(GUID_SysKeyboard, &g_pKeyboard, NULL));

	ThrowIfFailed(g_pKeyboard->SetDataFormat(&c_dfDIKeyboard));

	ThrowIfFailed(g_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));

	ThrowIfFailed(g_pKeyboard->Acquire());

	ThrowIfFailed(g_pDI->CreateDevice(GUID_SysMouse, &g_pMouse, NULL));

	ThrowIfFailed(g_pMouse->SetDataFormat(&c_dfDIMouse));

	ThrowIfFailed(g_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));

	ThrowIfFailed(g_pMouse->Acquire());

	return true;
}

void Input::Shutdown()
{
	if (g_pMouse)
	{
		g_pMouse->Unacquire();
		g_pMouse->Release();
		g_pMouse = nullptr;
	}

	if (g_pKeyboard)
	{
		g_pKeyboard->Unacquire();
		g_pKeyboard->Release();
		g_pKeyboard = nullptr;
	}

	if (g_pDI)
	{
		g_pDI->Release();
		g_pDI = nullptr;
	}
}

void Input::Update()
{
	HRESULT hr;

	hr = g_pKeyboard->GetDeviceState(sizeof(g_KeyboardState), (LPVOID)&g_KeyboardState);
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			g_pKeyboard->Acquire();
		}
	}

	hr = g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &g_MouseState);
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			g_pMouse->Acquire();
		}
	}
}

bool Input::IsKeyPressed(Key key)
{
	return (g_KeyboardState[static_cast<BYTE>(key)] & 0x80) != 0;
}

bool Input::IsMouseButtonPressed(int button)
{
	return (g_MouseState.rgbButtons[button] & 0x80) != 0;
}

std::pair<float, float> Input::GetMouseDelta()
{
	return std::make_pair(g_MouseState.lX, g_MouseState.lY);
}
