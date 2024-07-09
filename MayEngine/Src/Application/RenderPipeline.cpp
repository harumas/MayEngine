#include "RenderPipeline.h"
#include "Win32Application.h"
#include "Component/Camera.h"
#include "Component/Light.h"
#include "Component/Material.h"
#include "Component/Renderer.h"

RenderPipeline::RenderPipeline(unsigned int width, unsigned int height, std::wstring title)
	: title_(title)
	  , windowWidth_(width)
	  , windowHeight_(height)
	  , viewport_(0.0f, 0.0f, static_cast<float>(windowWidth_), static_cast<float>(windowHeight_))
	  , scissorrect_(0, 0, static_cast<LONG>(windowWidth_), static_cast<LONG>(windowHeight_))
	  , cameraObject("Camera"),testObject("Test"), fenceValue_(0)
	  , fenceEvent_(nullptr)
{
	light = make_shared<Light>();
	material = make_shared<Material>();
}

// 初期化処理
void RenderPipeline::OnInit(HWND hwnd)
{
	LoadPipeline(hwnd);
	LoadAssets();
	 
	camera = cameraObject.AddComponent<Camera>();
	renderer = testObject.AddComponent<Renderer>();
	renderer->LoadMesh("Assets/blocks/grass_block.fbx");
	 
	auto basicHandle = basicHeap_->GetCPUDescriptorHandleForHeapStart();
	basicHandle.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	light->CreateLightingBufferView(basicHandle);
	material->CreateShaderResourceView(basicHandle);
	 
	//FoV: 90
	//nearPlane: 1
	//farPlane: 1000
	camera->Init(XM_PIDIV2, 1.0f, 1000.0f);
	camera->transform->position = Vector3{ 0.0f, 3.0f, -10.0f };
	camera->SetYaw(0.0f);
}

void RenderPipeline::LoadPipeline(HWND hwnd)
{
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	{
		// デバッグレイヤーを有効にする
		ComPtr<ID3D12Debug> debugLayer;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer))))
		{
			debugLayer->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	// DXGIFactoryの初期化
	ComPtr<IDXGIFactory6> dxgiFactory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

	// デバイスの初期化
	CreateD3D12Device(dxgiFactory.Get(), device_.ReleaseAndGetAddressOf());

	// コマンド関連の初期化
	{
		// コマンドアロケータ
		ThrowIfFailed(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.ReleaseAndGetAddressOf())));
		// コマンドリスト
		ThrowIfFailed(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.ReleaseAndGetAddressOf())));
		// コマンドキュー
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // タイムアウト無し
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // コマンドリストと合わせる
		ThrowIfFailed(device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.ReleaseAndGetAddressOf())));
	}

	// スワップチェーンの初期化
	{
		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.BufferCount = kFrameCount;
		swapchainDesc.Width = windowWidth_;
		swapchainDesc.Height = windowHeight_;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.SampleDesc.Count = 1;
		ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
			commandQueue_.Get(),
			hwnd,
			&swapchainDesc,
			nullptr,
			nullptr,
			reinterpret_cast<IDXGISwapChain1**>(swapchain_.ReleaseAndGetAddressOf())));
	}

	// ディスクリプタヒープの初期化
	{
		// レンダーターゲットビュー
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = kFrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(device_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeap_.ReleaseAndGetAddressOf())));

		// 深度ステンシルビュー
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		ThrowIfFailed(device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap_.ReleaseAndGetAddressOf())));

		// 基本情報の受け渡し用
		D3D12_DESCRIPTOR_HEAP_DESC basicHeapDesc = {};
		basicHeapDesc.NumDescriptors = maxCBufferBlockCount + 1;
		basicHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		basicHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(device_->CreateDescriptorHeap(&basicHeapDesc, IID_PPV_ARGS(basicHeap_.ReleaseAndGetAddressOf())));
	}

	// スワップチェーンと関連付けてレンダーターゲットビューを生成
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap_->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < kFrameCount; ++i)
		{
			ThrowIfFailed(swapchain_->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(renderTargets_[i].ReleaseAndGetAddressOf())));
			device_->CreateRenderTargetView(renderTargets_[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		}
	}

	// 深度バッファービュー生成
	{
		// 深度バッファー作成
		D3D12_RESOURCE_DESC depthResDesc = {};
		depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthResDesc.Width = windowWidth_;
		depthResDesc.Height = windowHeight_;
		depthResDesc.DepthOrArraySize = 1;
		depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthResDesc.SampleDesc.Count = 1;
		depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		depthResDesc.MipLevels = 1;
		depthResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthResDesc.Alignment = 0;

		auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		// クリアバリューの設定
		D3D12_CLEAR_VALUE _depthClearValue = {};
		_depthClearValue.DepthStencil.Depth = 1.0f;      //深さ１(最大値)でクリア
		_depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; //32bit深度値としてクリア
		ThrowIfFailed(device_->CreateCommittedResource(
			&depthHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&depthResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, //デプス書き込みに使用
			&_depthClearValue,
			IID_PPV_ARGS(depthBuffer_.ReleaseAndGetAddressOf())));

		// 深度バッファービュー作成
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		device_->CreateDepthStencilView(depthBuffer_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());
	}

	// フェンスの生成
	{
		ThrowIfFailed(device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf())));
		fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	}
}

void RenderPipeline::LoadAssets()
{
	CreateRootSignature();

	CreatePipelineState();

	CreateMatrixBufferResources();

	light->CreateLightingBuffer();

	material->CreateShaderResourceBuffer();
	 
	// 命令のクローズ
	commandList_->Close();

	// コマンドリストの実行
	{
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(1, commandLists);
	}

	// GPU処理の終了を待機
	{
		ThrowIfFailed(commandQueue_->Signal(fence_.Get(), ++fenceValue_));
		if (fence_->GetCompletedValue() < fenceValue_)
		{
			ThrowIfFailed(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));
			WaitForSingleObject(fenceEvent_, INFINITE);
		}
	}
}

void RenderPipeline::CreateRootSignature()
{
	// ルートパラメータの生成
	// ディスクリプタテーブルの実体
	CD3DX12_DESCRIPTOR_RANGE1 discriptorRanges[3];
	discriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC); // CBV
	discriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC); // CBV
	discriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC); // SRV
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];
	rootParameters[0].InitAsDescriptorTable(3, discriptorRanges, D3D12_SHADER_VISIBILITY_ALL); // 同一パラメータで複数指定

	// サンプラーの生成
	// テクスチャデータからどう色を取り出すかを決めるための設定
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// ルートパラメータ、サンプラーからルートシグネチャを生成
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSignatureBlob, &errorBlob));
	ThrowIfFailed(device_->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(rootsignature_.ReleaseAndGetAddressOf())));
}

void RenderPipeline::CreatePipelineState()
{
	// パイプラインステートの生成
	// シェーダーオブジェクトの生成
#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	D3DCompileFromFile(L"Src/Shaders/HalfLambertShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", compileFlags, 0, &vsBlob, nullptr);
	D3DCompileFromFile(L"Src/Shaders/HalfLambertShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", compileFlags, 0, &psBlob, nullptr);

	// 頂点レイアウトの生成
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// 裏面描画にする場合、コメントを外すべし
	auto rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//rasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// パイプラインステートオブジェクト(PSO)を生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootsignature_.Get();
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) }; // 入力レイアウトの設定
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());                       // 頂点シェーダ
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());                       // ピクセルシェーダ
	psoDesc.RasterizerState = rasterizerState;                                // ラスタライザーステート
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);                   // ブレンドステート
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;                           // サンプルマスクの設定
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;   // トポロジタイプ
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;    // ストリップ時のカット設定
	psoDesc.NumRenderTargets = 1;                                             // レンダーターゲット数
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;                       // レンダーターゲットフォーマット
	psoDesc.SampleDesc.Count = 1;                                             // マルチサンプリングの設定

	// 深度ステンシル 
	psoDesc.DepthStencilState.DepthEnable = true;                             // 深度バッファーを使用するか
	psoDesc.DepthStencilState.StencilEnable = false;                          // ステンシルテストを行うか
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;                                // 深度バッファーで使用するフォーマット
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;         // 書き込む
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;    // 小さい方を採用する
	ThrowIfFailed(device_->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pipelinestate_.ReleaseAndGetAddressOf())));
}

void RenderPipeline::CreateMatrixBufferResources()
{
	// 定数バッファーの生成
	const CD3DX12_HEAP_PROPERTIES constHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	constexpr auto unitSize = (sizeof(mapMatricesData_) + 0xff) & ~0xff;
	const CD3DX12_RESOURCE_DESC constDesc = CD3DX12_RESOURCE_DESC::Buffer(unitSize * maxCBufferBlockCount); // 256アライメントでサイズを指定

	ThrowIfFailed(device_->CreateCommittedResource(
		&constHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&constDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constMatricesBuffer_.ReleaseAndGetAddressOf())));
}

int RenderPipeline::AssignBuffer()
{
	//確保しているバッファを超えたらthrow
	if (usingCBufferCount >= maxCBufferBlockCount)
	{
		ThrowMessage("Matrix buffer overflow!");
	}

	int handle = usingCBufferCount;
	usingCBufferCount++;

	return handle;
}

void RenderPipeline::SetMatrixBuffer(int handle, const DirectX::XMMATRIX& worldMatrix)
{
	//バッファを読み取る範囲を設定する
	const CD3DX12_RANGE readRange(0, 0);
	const float aspectRatio = static_cast<float>(windowWidth_) / static_cast<float>(windowHeight_);

	//バッファに書き込む
	constMatricesBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&mapMatricesData_));
	MatricesData* ptr = mapMatricesData_ + handle;
	ptr->world = worldMatrix;
	ptr->viewproj = camera->GetViewMatrix() * camera->GetProjectionMatrix(aspectRatio);
	constMatricesBuffer_->Unmap(0, nullptr);
}

void RenderPipeline::SetMatrixBufferPosition(int handle) const
{
	D3D12_GPU_DESCRIPTOR_HANDLE startAddress = basicHeap_->GetGPUDescriptorHandleForHeapStart();
	startAddress.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * handle;
	commandList_->SetGraphicsRootDescriptorTable(0, startAddress);
}

void RenderPipeline::CreateMatrixBufferView(int handle, const DirectX::XMMATRIX& worldMatrix)
{
	int sizeAligned = (sizeof(mapMatricesData_) + 0xff) & ~0xff;

	SetMatrixBuffer(handle, worldMatrix);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constMatricesBuffer_->GetGPUVirtualAddress() + handle * 0x100;
	cbvDesc.SizeInBytes = sizeAligned;

	D3D12_CPU_DESCRIPTOR_HANDLE ptr = basicHeap_->GetCPUDescriptorHandleForHeapStart();
	ptr.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * handle;
	device_->CreateConstantBufferView(&cbvDesc, ptr);
}

// 更新処理
void RenderPipeline::OnUpdate()
{
}

// 描画処理
void RenderPipeline::OnRender()
{
	// コマンドリストのリセット
	{
		ThrowIfFailed(commandAllocator_->Reset());
		ThrowIfFailed(commandList_->Reset(commandAllocator_.Get(), pipelinestate_.Get()));
	}

	// コマンドリストの生成
	{
		// バックバッファのインデックスを取得
		auto frameIndex = swapchain_->GetCurrentBackBufferIndex();

		// リソースバリアの設定 (PRESENT -> RENDER_TARGET)
		auto startResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets_[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList_->ResourceBarrier(1, &startResourceBarrier);

		// パイプラインステートと必要なオブジェクトを設定
		commandList_->SetPipelineState(pipelinestate_.Get());         // パイプラインステート
		commandList_->SetGraphicsRootSignature(rootsignature_.Get()); // ルートシグネチャ
		commandList_->RSSetViewports(1, &viewport_);                  // ビューポート
		commandList_->RSSetScissorRects(1, &scissorrect_);            // シザー短形
		// ディスクリプタテーブル
		// ルートパラメータとディスクリプタヒープを紐づける
		ID3D12DescriptorHeap* ppHeaps[] = { basicHeap_.Get() };
		commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		commandList_->SetGraphicsRootDescriptorTable(0, basicHeap_->GetGPUDescriptorHandleForHeapStart());

		// レンダーターゲットの設定
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap_->GetCPUDescriptorHandleForHeapStart(), frameIndex, device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		auto dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
		commandList_->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
		float clearColor[] = { 0.390625f, 0.58203125f, 0.92578125f, 1.0f };  // 空色
		commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// 描画処理の設定
		ObjectService::ProcessOnDraw();

		// リソースバリアの設定 (RENDER_TARGET -> PRESENT)
		auto endResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets_[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList_->ResourceBarrier(1, &endResourceBarrier);

		// 命令のクローズ
		commandList_->Close();
	}

	// コマンドリストの実行
	{
		ID3D12CommandList* commandLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(1, commandLists);
		// 画面のスワップ
		ThrowIfFailed(swapchain_->Present(1, 0));
	}

	// GPU処理の終了を待機
	{
		ThrowIfFailed(commandQueue_->Signal(fence_.Get(), ++fenceValue_));
		if (fence_->GetCompletedValue() < fenceValue_)
		{
			ThrowIfFailed(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));
			WaitForSingleObject(fenceEvent_, INFINITE);
		}
	}
}

// 終了処理
void RenderPipeline::OnDestroy()
{
	CloseHandle(fenceEvent_);
}
