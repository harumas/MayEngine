#pragma once
#include <dxgi1_6.h>
#include <stdexcept>
#include <d3d12.h>
#include <vector>


// D3D12Deviceの生成
inline void CreateD3D12Device(IDXGIFactory6* dxgiFactory, ID3D12Device** d3d12device)
{
	ID3D12Device* tmpDevice = nullptr;

	// グラフィックスボードの選択
	std::vector <IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0; SUCCEEDED(dxgiFactory->EnumAdapters(i, &tmpAdapter)); ++i)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adapter : adapters)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		adapter->GetDesc(&adapterDesc);
		// AMDを含むアダプターオブジェクトを探して格納（見つからなければnullptrでデフォルト）
		// 製品版の場合は、オプション画面から選択させて設定する必要がある
		std::wstring strAdapter = adapterDesc.Description;
		if (strAdapter.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adapter;
			break;
		}
	}

	// Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	for (auto level : levels)
	{
		if (D3D12CreateDevice(tmpAdapter, level, IID_PPV_ARGS(&tmpDevice)) == S_OK)
		{
			break; // 生成可能なバージョンが見つかったらループを打ち切り
		}
	}
	*d3d12device = tmpDevice;
}

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		// hrのエラー内容をthrowする
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		std::string errMessage = std::string(s_str);
		throw std::runtime_error(errMessage);
	}
}

inline void ThrowMessage(std::string message)
{
	throw std::runtime_error(message);
}
