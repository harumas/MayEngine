#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "d3dx12.h"

#include "FbxLoader.h"
#include "Helper.h"

#include <stdexcept>
#include <vector>

#include <basetsd.h>
#include <dxgi1_5.h>
#include <windows.h>
#include <wrl/client.h>
#include <memory>
#include <string>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

using Microsoft::WRL::ComPtr;

class RenderPipeline
{
public:
	static constexpr unsigned int kFrameCount = 2;

	// パイプラインオブジェクト
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;              // レンダーターゲットヒープ
	ComPtr<ID3D12DescriptorHeap> dsvHeap;              // 深度バッファーヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap;              // シェーダーリソースヒープ
	ComPtr<ID3D12Resource> renderTargets[kFrameCount]; // バックバッファー
	ComPtr<ID3D12Resource> depthBuffer;                // 深度バッファー
	ComPtr<ID3D12RootSignature> rootSignature;         // ルートシグネチャ

	// フェンス
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue;
	HANDLE fenceEvent;

	static RenderPipeline& Create()
	{
		instance = new RenderPipeline();
		return *instance;
	}

	static RenderPipeline& GetInstance()
	{
		return *instance;
	}

	static void Destroy()
	{
		delete instance;
		instance = nullptr;
	}

	RenderPipeline(const RenderPipeline&) = delete;
	RenderPipeline& operator=(const RenderPipeline&) = delete;
	RenderPipeline(RenderPipeline&&) = delete;
	RenderPipeline& operator=(RenderPipeline&&) = delete;

	void OnInit(HWND hwnd);
	void OnPostInit();
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT heapIndex) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT heapIndex) const;

private:
	RenderPipeline();
	~RenderPipeline() = default;

	static RenderPipeline* instance;

	CD3DX12_VIEWPORT viewPort; // ビューポート
	CD3DX12_RECT scissorRect;  // シザー短形

	void LoadPipeline(HWND hwnd);
	void CreateRootSignature();
};
