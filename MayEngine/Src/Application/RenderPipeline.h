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
 
#include "Component/GameObject.h"

using Microsoft::WRL::ComPtr;
using namespace std;
 
class Renderer;
class Camera;
class Light;
class Material;

class RenderPipeline : public std::enable_shared_from_this<RenderPipeline>
{
public:
	static std::shared_ptr<RenderPipeline> Create(unsigned int width, unsigned int height, const std::wstring& title)
	{
		RenderPipeline::instance =  std::shared_ptr<RenderPipeline>(new RenderPipeline(width, height, title));
		return RenderPipeline::instance;
	}

	void OnInit(HWND hwnd);
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	static const unsigned int kFrameCount = 2;
	static const unsigned int kCbvUrvMax = 3;

	// パイプラインオブジェクト
	ComPtr<ID3D12Device> device_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList> commandList_;
	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<IDXGISwapChain4> swapchain_;
	ComPtr<ID3D12DescriptorHeap> rtvHeap_;              // レンダーターゲットヒープ
	ComPtr<ID3D12DescriptorHeap> dsvHeap_;              // 深度バッファーヒープ
	ComPtr<ID3D12DescriptorHeap> basicHeap_;            // 基本情報の受け渡し用(SRV + CBV)
	ComPtr<ID3D12Resource> renderTargets_[kFrameCount]; // バックバッファー
	ComPtr<ID3D12Resource> depthBuffer_;                // 深度バッファー
	ComPtr<ID3D12PipelineState> pipelinestate_;         // パイプラインステート
	ComPtr<ID3D12RootSignature> rootsignature_;         // ルートシグネチャ

	// リソース
	ComPtr<ID3D12Resource> constMatricesBuffer_;

	// フェンス
	ComPtr<ID3D12Fence> fence_;
	UINT64 fenceValue_;
	HANDLE fenceEvent_;

	int AssignBuffer();
	void SetMatrixBuffer(int handle, const DirectX::XMMATRIX& worldMatrix);
	void SetMatrixBufferPosition(int handle) const;
	void CreateMatrixBufferView(int handle, const DirectX::XMMATRIX& worldMatrix);
	 
	static shared_ptr<RenderPipeline> instance;
private:
	RenderPipeline(unsigned int width, unsigned int height, std::wstring title);

	unsigned int windowWidth_;
	unsigned int windowHeight_;

	const int maxCBufferBlockCount = 1024;
	int usingCBufferCount = 0;

	CD3DX12_VIEWPORT viewport_; // ビューポート
	CD3DX12_RECT scissorrect_;  // シザー短形

	GameObject cameraObject;
	GameObject testObject;
	shared_ptr<Camera> camera;  // カメラ
	shared_ptr<Renderer> renderer;
	shared_ptr<Light> light;  // ライト
	shared_ptr<Material> material;  // マテリアル

	// 3D座標変換用行列
	struct MatricesData
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX viewproj;
	};

	MatricesData* mapMatricesData_;

	void LoadPipeline(HWND hwnd);
	void LoadAssets();
	void CreateRootSignature();
	void CreatePipelineState();
	void CreateMatrixBufferResources();
};