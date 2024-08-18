#pragma once
#include <string>

#include "Win32Application.h"
#include "../Scene/SceneManager.h"

class Application
{
public:
	Application(HINSTANCE hInstance, unsigned int width, unsigned int height, const std::wstring& title);
	void Run(int bootSceneIndex);

	SceneManager& GetSceneManager()
	{
		return sceneManager;
	}

private:
	Win32Application winApplication;
	SceneManager sceneManager;

	void SetUp(HWND hwnd, HINSTANCE hInstance);
	void Update();
	void Dispose();
};
