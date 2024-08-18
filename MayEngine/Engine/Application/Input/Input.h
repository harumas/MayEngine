#pragma once
#define DIRECTINPUT_VERSION 0x0800 

#include <utility>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib") 

#include <cstdint>
#include <windows.h>

// ���z�L�[�R�[�h�̗񋓑�
enum class Key : uint16_t
{
	LBUTTON = 0x01,      // ���}�E�X�{�^��
	RBUTTON = 0x02,      // �E�}�E�X�{�^��
	CANCEL = 0x03,       // ���f
	MBUTTON = 0x04,      // �����}�E�X�{�^��
	BUTTON1 = 0x05,     // �T�C�h�{�^�� 1
	BUTTON2 = 0x06,     // �T�C�h�{�^�� 2
	BACK = 0x08,         // �o�b�N�X�y�[�X
	TAB = 0x09,          // �^�u
	CLEAR = 0x0C,        // CLEAR
	RETURN = 0x0D,       // ENTER
	SHIFT = 0x10,        // SHIFT
	CONTROL = 0x11,      // CTRL
	MENU = 0x12,         // ALT
	PAUSE = 0x13,        // PAUSE
	CAPITAL = 0x14,      // CAPS LOCK
	ESCAPE = 0x1B,       // ESC
	SPACE = 0x20,        // �X�y�[�X
	PRIOR = 0x21,        // PAGE UP
	NEXT = 0x22,         // PAGE DOWN
	END = 0x23,          // END
	HOME = 0x24,         // HOME
	LEFT = 0x25,         // ��
	UP = 0x26,           // ��
	RIGHT = 0x27,        // �E
	DOWN = 0x28,         // ��
	SELECT = 0x29,       // SELECT
	PRINT = 0x2A,        // PRINT
	EXECUTE = 0x2B,      // EXECUTE
	SNAPSHOT = 0x2C,     // PRINT SCREEN
	INSERT = 0x2D,       // INSERT
	DEL = 0x2E,       // DELETE
	HELP = 0x2F,         // HELP
	K0 = 0x30,            // 0
	K1 = 0x31,            // 1
	K2 = 0x32,            // 2
	K3 = 0x33,            // 3
	K4 = 0x34,            // 4
	K5 = 0x35,            // 5
	K6 = 0x36,            // 6
	K7 = 0x37,            // 7
	K8 = 0x38,            // 8
	K9 = 0x39,            // 9
	A = 0x41,            // A
	B = 0x42,            // B
	C = 0x43,            // C
	D = 0x44,            // D
	E = 0x45,            // E
	F = 0x46,            // F
	G = 0x47,            // G
	H = 0x48,            // H
	I = 0x49,            // I
	J = 0x4A,            // J
	K = 0x4B,            // K
	L = 0x4C,            // L
	M = 0x4D,            // M
	N = 0x4E,            // N
	O = 0x4F,            // O
	P = 0x50,            // P
	Q = 0x51,            // Q
	R = 0x52,            // R
	S = 0x53,            // S
	T = 0x54,            // T
	U = 0x55,            // U
	V = 0x56,            // V
	W = 0x57,            // W
	X = 0x58,            // X
	Y = 0x59,            // Y
	Z = 0x5A,            // Z
	LWIN = 0x5B,         // ���E�B���h�E�L�[
	RWIN = 0x5C,         // �E�E�B���h�E�L�[
	APPS = 0x5D,         // �A�v���P�[�V�������j���[�L�[
	SLEEP = 0x5F,        // �X���[�v
	NUMPAD0 = 0x60,      // �����L�[ 0
	NUMPAD1 = 0x61,      // �����L�[ 1
	NUMPAD2 = 0x62,      // �����L�[ 2
	NUMPAD3 = 0x63,      // �����L�[ 3
	NUMPAD4 = 0x64,      // �����L�[ 4
	NUMPAD5 = 0x65,      // �����L�[ 5
	NUMPAD6 = 0x66,      // �����L�[ 6
	NUMPAD7 = 0x67,      // �����L�[ 7
	NUMPAD8 = 0x68,      // �����L�[ 8
	NUMPAD9 = 0x69,      // �����L�[ 9
	MULTIPLY = 0x6A,     // *
	ADD = 0x6B,          // +
	SEPARATOR = 0x6C,    // |
	SUBTRACT = 0x6D,     // -
	DECIMAL = 0x6E,      // .
	DIVIDE = 0x6F,       // /
	F1 = 0x70,           // F1
	F2 = 0x71,           // F2
	F3 = 0x72,           // F3
	F4 = 0x73,           // F4
	F5 = 0x74,           // F5
	F6 = 0x75,           // F6
	F7 = 0x76,           // F7
	F8 = 0x77,           // F8
	F9 = 0x78,           // F9
	F10 = 0x79,          // F10
	F11 = 0x7A,          // F11
	F12 = 0x7B,          // F12
	F13 = 0x7C,          // F13
	F14 = 0x7D,          // F14
	F15 = 0x7E,          // F15
	F16 = 0x7F,          // F16
	F17 = 0x80,          // F17
	F18 = 0x81,          // F18
	F19 = 0x82,          // F19
	F20 = 0x83,          // F20
	F21 = 0x84,          // F21
	F22 = 0x85,          // F22
	F23 = 0x86,          // F23
	F24 = 0x87,          // F24
	NUMLOCK = 0x90,      // NUM LOCK
	SCROLL = 0x91,       // SCROLL LOCK
	LSHIFT = 0xA0,       // �� SHIFT
	RSHIFT = 0xA1,       // �E SHIFT
	LCONTROL = 0xA2,     // �� CTRL
	RCONTROL = 0xA3,     // �E CTRL
	LMENU = 0xA4,        // �� ALT
	RMENU = 0xA5,        // �E ALT
	BROWSER_BACK = 0xA6, // �u���E�U �o�b�N
	BROWSER_FORWARD = 0xA7, // �u���E�U �t�H���[�h
	BROWSER_REFRESH = 0xA8, // �u���E�U �X�V
	BROWSER_STOP = 0xA9, // �u���E�U ��~
	BROWSER_SEARCH = 0xAA, // �u���E�U ����
	BROWSER_FAVORITES = 0xAB, // �u���E�U ���C�ɓ���
	BROWSER_HOME = 0xAC, // �u���E�U �z�[��
	VOLUME_MUTE = 0xAD, // ���� �~���[�g
	VOLUME_DOWN = 0xAE, // ���� �_�E��
	VOLUME_UP = 0xAF, // ���� �A�b�v
	MEDIA_NEXT_TRACK = 0xB0, // ���f�B�A ���̃g���b�N
	MEDIA_PREV_TRACK = 0xB1, // ���f�B�A �O�̃g���b�N
	MEDIA_STOP = 0xB2, // ���f�B�A ��~
	MEDIA_PLAY_PAUSE = 0xB3, // ���f�B�A �Đ�/�ꎞ��~
	LAUNCH_MAIL = 0xB4, // ���[���N��
	LAUNCH_MEDIA_SELECT = 0xB5, // ���f�B�A�I��
	LAUNCH_APP1 = 0xB6, // �A�v���P�[�V����1�N��
	LAUNCH_APP2 = 0xB7, // �A�v���P�[�V����2�N��
	OEM_1 = 0xBA, // ; : (�p��L�[�{�[�h�̏ꍇ)
	OEM_PLUS = 0xBB, // + (�p��L�[�{�[�h�̏ꍇ)
	OEM_COMMA = 0xBC, // , < (�p��L�[�{�[�h�̏ꍇ)
	OEM_MINUS = 0xBD, // - _ (�p��L�[�{�[�h�̏ꍇ)
	OEM_PERIOD = 0xBE, // . > (�p��L�[�{�[�h�̏ꍇ)
	OEM_2 = 0xBF, // / ? (�p��L�[�{�[�h�̏ꍇ)
	OEM_3 = 0xC0, // ` ~ (�p��L�[�{�[�h�̏ꍇ)
	OEM_4 = 0xDB, // [ { (�p��L�[�{�[�h�̏ꍇ)
	OEM_5 = 0xDC, // \ | (�p��L�[�{�[�h�̏ꍇ)
	OEM_6 = 0xDD, // ] } (�p��L�[�{�[�h�̏ꍇ)
	OEM_7 = 0xDE, // ' " (�p��L�[�{�[�h�̏ꍇ)
	OEM_8 = 0xDF, // OEM 8 (�p��L�[�{�[�h�̏ꍇ)
	OEM_102 = 0xE2, // < > (�p��L�[�{�[�h�̏ꍇ)
	PROCESSKEY = 0xE5, // PROCESSKEY
	PACKET = 0xE7, // PACKET
	ATTN = 0xF6, // ATTN
	CRSEL = 0xF7, // CRSEL
	EXSEL = 0xF8, // EXSEL
	EREOF = 0xF9, // EREOF
	PLAY = 0xFA, // PLAY
	ZOOM = 0xFB, // ZOOM
	NONAME = 0xFC, // NONAME
	PA1 = 0xFD, // PA1
	OEM_CLEAR = 0xFE // OEM CLEAR
};

class Input
{
public:
	static bool Initialize();
	static void Shutdown();
	static void Update();
	static void PostUpdate();
	static bool IsKeyPressed(Key key);
	static bool IsMouseButtonPressed(int button);
	static std::pair<float, float> GetMouseDelta();

private:
	static std::pair<float, float> mouseDelta;
	static bool keyState[256];

	Input() = delete;
	~Input() = delete;

	static LRESULT CALLBACK InputProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};
