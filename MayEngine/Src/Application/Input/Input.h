#pragma once
#define DIRECTINPUT_VERSION 0x0800 

#include <Windows.h>
#include <dinput.h>
#include <utility>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib") 

#include "../Helper.h"

enum class Key
{
	ESCAPE = DIK_ESCAPE,
	KEY_1 = DIK_1,
	KEY_2 = DIK_2,
	KEY_3 = DIK_3,
	KEY_4 = DIK_4,
	KEY_5 = DIK_5,
	KEY_6 = DIK_6,
	KEY_7 = DIK_7,
	KEY_8 = DIK_8,
	KEY_9 = DIK_9,
	KEY_0 = DIK_0,
	A = DIK_A,
	B = DIK_B,
	C = DIK_C,
	D = DIK_D,
	E = DIK_E,
	F = DIK_F,
	G = DIK_G,
	H = DIK_H,
	I = DIK_I,
	J = DIK_J,
	K = DIK_K,
	L = DIK_L,
	M = DIK_M,
	N = DIK_N,
	O = DIK_O,
	P = DIK_P,
	Q = DIK_Q,
	R = DIK_R,
	S = DIK_S,
	T = DIK_T,
	U = DIK_U,
	V = DIK_V,
	W = DIK_W,
	X = DIK_X,
	Y = DIK_Y,
	Z = DIK_Z,
	F1 = DIK_F1,
	F2 = DIK_F2,
	F3 = DIK_F3,
	F4 = DIK_F4,
	F5 = DIK_F5,
	F6 = DIK_F6,
	F7 = DIK_F7,
	F8 = DIK_F8,
	F9 = DIK_F9,
	F10 = DIK_F10,
	F11 = DIK_F11,
	F12 = DIK_F12,
	UP = DIK_UP,
	DOWN = DIK_DOWN,
	LEFT = DIK_LEFT,
	RIGHT = DIK_RIGHT,
	LSHIFT = DIK_LSHIFT,
	RSHIFT = DIK_RSHIFT,
	LCONTROL = DIK_LCONTROL,
	RCONTROL = DIK_RCONTROL,
	LALT = DIK_LALT,
	RALT = DIK_RALT,
	TAB = DIK_TAB,
	RETURN = DIK_RETURN,
	SPACE = DIK_SPACE,
	BACK = DIK_BACK,
	KEY_DELETE = DIK_DELETE,
	INSERT = DIK_INSERT,
	HOME = DIK_HOME,
	END = DIK_END,
	PGUP = DIK_PGUP,
	PGDN = DIK_PGDN,
	CAPSLOCK = DIK_CAPSLOCK,
	NUMLOCK = DIK_NUMLOCK,
	SCROLL = DIK_SCROLL,
	NUMPAD0 = DIK_NUMPAD0,
	NUMPAD1 = DIK_NUMPAD1,
	NUMPAD2 = DIK_NUMPAD2,
	NUMPAD3 = DIK_NUMPAD3,
	NUMPAD4 = DIK_NUMPAD4,
	NUMPAD5 = DIK_NUMPAD5,
	NUMPAD6 = DIK_NUMPAD6,
	NUMPAD7 = DIK_NUMPAD7,
	NUMPAD8 = DIK_NUMPAD8,
	NUMPAD9 = DIK_NUMPAD9,
	NUMPADENTER = DIK_NUMPADENTER,
	NUMPADPLUS = DIK_NUMPADPLUS,
	NUMPADMINUS = DIK_NUMPADMINUS,
	NUMPADSTAR = DIK_NUMPADSTAR,
	NUMPADSLASH = DIK_NUMPADSLASH
};


class Input
{
public:

	static bool Initialize(HINSTANCE hInstance, HWND hWnd);
	static void Shutdown();
	static void Update();
	static bool IsKeyPressed(Key key);
	static bool IsMouseButtonPressed(int button);
	static std::pair<float, float> GetMouseDelta();

private:
	static LPDIRECTINPUT8 g_pDI;
	static LPDIRECTINPUTDEVICE8 g_pKeyboard;
	static LPDIRECTINPUTDEVICE8 g_pMouse;
	static BYTE g_KeyboardState[256];
	static DIMOUSESTATE g_MouseState;

	Input() = delete;
	~Input() = delete;
};
