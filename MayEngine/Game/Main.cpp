#include <windows.h>

#include "MainScene.h"
#include "../Engine/Application/Application.h"
#include "../Engine/Scene/SceneManager.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	Application gameApp(hInstance, 1920, 1080, L"MayEngine");

	SceneManager& sceneManager = gameApp.GetSceneManager();
	sceneManager.Register<MainScene>();

	gameApp.Run(0);

	return 0;
}
