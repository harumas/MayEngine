#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXTex.h>

#include <wrl.h>
#include "d3dx12.h"

#include "FbxLoader.h"
#include "DXTexFix.h"
#include "Helper.h"

#include <stdexcept>
#include <vector>

#include <basetsd.h>
#include <dxgi1_5.h>
#include <windef.h>
#include <winnt.h>
#include <wrl/client.h>
#include <memory>
#include <string>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

using Microsoft::WRL::ComPtr;
using namespace std;

class RenderPipeline : public std::enable_shared_from_this<RenderPipeline>
{
public:
	static std::shared_ptr<RenderPipeline> Create()
	{
		RenderPipeline::instance = std::shared_ptr<RenderPipeline>(new RenderPipeline());
		return RenderPipeline::instance;
	}

	void OnInit(HWND hwnd);
	void OnPostInit();
	void OnUpdate();
	void OnRender();
	void OnDestroy();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT heapIndex) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT heapIndex) const;

	static constexpr unsigned int kFrameCount = 2;
	static constexpr unsigned int kCbvUrvMax = 3;
	static shared_ptr<RenderPipeline> instance;

	// パイプラインオブジェクト
	ComPtr<ID3D12Device> device_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList> commandList_;
	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<IDXGISwapChain4> swapchain_;
	ComPtr<ID3D12DescriptorHeap> rtvHeap_;              // レンダーターゲットヒープ
	ComPtr<ID3D12DescriptorHeap> dsvHeap_;              // 深度バッファーヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap_;              // シェーダーリソースヒープ
	ComPtr<ID3D12Resource> renderTargets_[kFrameCount]; // バックバッファー
	ComPtr<ID3D12Resource> depthBuffer_;                // 深度バッファー
	ComPtr<ID3D12RootSignature> rootsignature_;         // ルートシグネチャ

	// フェンス
	ComPtr<ID3D12Fence> fence_;
	UINT64 fenceValue_;
	HANDLE fenceEvent_;

private:
	RenderPipeline();

	CD3DX12_VIEWPORT viewport_; // ビューポート
	CD3DX12_RECT scissorrect_;  // シザー短形

	void LoadPipeline(HWND hwnd);
	void CreateRootSignature();
};
