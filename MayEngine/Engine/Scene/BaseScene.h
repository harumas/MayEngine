#pragma once
#include <string>

class BaseScene
{
public:
	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	virtual std::string_view GetSceneName()
	{
		return "Unknown";
	}
};
