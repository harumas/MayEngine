#include "RenderPipeline.h"

#include "AppInfo.h"
#include "Win32Application.h"
#include "Component/ObjectService.h"

RenderPipeline* RenderPipeline::instance = nullptr;

RenderPipeline::RenderPipeline()
	: fenceValue(0)
	, fenceEvent(nullptr)
	, viewPort(0.0f, 0.0f, AppInfo::GetWindowWidth(), AppInfo::GetWindowHeight())
	, scissorRect(0, 0, static_cast<LONG>(AppInfo::GetWindowWidth()), static_cast<LONG>(AppInfo::GetWindowHeight()))
{
}

// ����������
void RenderPipeline::OnInit(HWND hwnd)
{
	LoadPipeline(hwnd);
	CreateRootSignature();
}

void RenderPipeline::OnPostInit()
{
	commandList->Close();

	{
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(1, commandLists);
	}

	{
		ThrowIfFailed(commandQueue->Signal(fence.Get(), ++fenceValue));
		if (fence->GetCompletedValue() < fenceValue)
		{
			ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			WaitForSingleObject(fenceEvent, INFINITE);
		}
	}
}

void RenderPipeline::LoadPipeline(HWND hwnd)
{
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	{
		// �f�o�b�O���C���[��L���ɂ���
		ComPtr<ID3D12Debug> debugLayer;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer))))
		{
			debugLayer->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	// DXGIFactory�̏�����
	ComPtr<IDXGIFactory6> dxgiFactory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

	// �f�o�C�X�̏�����
	CreateD3D12Device(dxgiFactory.Get(), device.ReleaseAndGetAddressOf());

	// �R�}���h�֘A�̏�����
	{
		// �R�}���h�A���P�[�^
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.ReleaseAndGetAddressOf())));
		// �R�}���h���X�g
		ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.ReleaseAndGetAddressOf())));
		// �R�}���h�L���[
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // �^�C���A�E�g����
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // �R�}���h���X�g�ƍ��킹��
		ThrowIfFailed(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue.ReleaseAndGetAddressOf())));
	}

	// �X���b�v�`�F�[���̏�����
	{
		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.BufferCount = kFrameCount;
		swapchainDesc.Width = AppInfo::GetWindowWidth();
		swapchainDesc.Height = AppInfo::GetWindowHeight();
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.SampleDesc.Count = 1;
		ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			hwnd,
			&swapchainDesc,
			nullptr,
			nullptr,
			reinterpret_cast<IDXGISwapChain1**>(swapChain.ReleaseAndGetAddressOf())));
	}

	// �f�B�X�N���v�^�q�[�v�̏�����
	{
		// �����_�[�^�[�Q�b�g�r���[
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = kFrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeap.ReleaseAndGetAddressOf())));

		// �[�x�X�e���V���r���[
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.ReleaseAndGetAddressOf())));

		// �V�F�[�_�[���\�[�X�r���[
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 50;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.ReleaseAndGetAddressOf())));
	}

	// �X���b�v�`�F�[���Ɗ֘A�t���ă����_�[�^�[�Q�b�g�r���[�𐶐�
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < kFrameCount; ++i)
		{
			ThrowIfFailed(swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(renderTargets[i].ReleaseAndGetAddressOf())));
			device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		}
	}

	// �[�x�o�b�t�@�[�r���[����
	{
		// �[�x�o�b�t�@�[�쐬
		D3D12_RESOURCE_DESC depthResDesc = {};
		depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthResDesc.Width = AppInfo::GetWindowWidth();
		depthResDesc.Height = AppInfo::GetWindowHeight();
		depthResDesc.DepthOrArraySize = 1;
		depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthResDesc.SampleDesc.Count = 1;
		depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		depthResDesc.MipLevels = 1;
		depthResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthResDesc.Alignment = 0;

		auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		// �N���A�o�����[�̐ݒ�
		D3D12_CLEAR_VALUE _depthClearValue = {};
		_depthClearValue.DepthStencil.Depth = 1.0f;      //�[���P(�ő�l)�ŃN���A
		_depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; //32bit�[�x�l�Ƃ��ăN���A
		ThrowIfFailed(device->CreateCommittedResource(
			&depthHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&depthResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, //�f�v�X�������݂Ɏg�p
			&_depthClearValue,
			IID_PPV_ARGS(depthBuffer.ReleaseAndGetAddressOf())));

		// �[�x�o�b�t�@�[�r���[�쐬
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		device->CreateDepthStencilView(depthBuffer.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	// �t�F���X�̐���
	{
		ThrowIfFailed(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf())));
		fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	}
}

void RenderPipeline::CreateRootSignature()
{
	// ���[�g�p�����[�^�̐���
	// �f�B�X�N���v�^�e�[�u���̎���
	CD3DX12_DESCRIPTOR_RANGE1 discriptorRanges[1];
	discriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC); // SRV (�e�N�X�`��)
	CD3DX12_ROOT_PARAMETER1 rootParameters[5];
	rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL); // CBV (Model + ViewProjection)
	rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL); // CBV (EnvironmentLight)
	rootParameters[2].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL); // CBV (PointLight)
	rootParameters[3].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL); // CBV (Material)
	rootParameters[4].InitAsDescriptorTable(1, discriptorRanges, D3D12_SHADER_VISIBILITY_ALL); // SRV (Texture)

	// �T���v���[�̐���
	// �e�N�X�`���f�[�^����ǂ��F�����o���������߂邽�߂̐ݒ�
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�p�����[�^�A�T���v���[���烋�[�g�V�O�l�`���𐶐�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSignatureBlob, &errorBlob));
	ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf())));
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderPipeline::GetCPUDescriptorHandle(UINT heapIndex) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE startAddress = srvHeap->GetCPUDescriptorHandleForHeapStart();
	startAddress.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * heapIndex;
	return startAddress;
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderPipeline::GetGPUDescriptorHandle(UINT heapIndex) const
{
	D3D12_GPU_DESCRIPTOR_HANDLE startAddress = srvHeap->GetGPUDescriptorHandleForHeapStart();
	startAddress.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * heapIndex;
	return startAddress;
}

// �X�V����
void RenderPipeline::OnUpdate()
{
	ObjectService::ProcessOnUpdate();
}

// �`�揈��
void RenderPipeline::OnRender()
{
	// �R�}���h���X�g�̃��Z�b�g
	{
		ThrowIfFailed(commandAllocator->Reset());
		ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
	}

	// �R�}���h���X�g�̐���
	{
		// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾
		auto frameIndex = swapChain->GetCurrentBackBufferIndex();

		// ���\�[�X�o���A�̐ݒ� (PRESENT -> RENDER_TARGET)
		auto startResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &startResourceBarrier);

		commandList->RSSetViewports(1, &viewPort);                  // �r���[�|�[�g
		commandList->RSSetScissorRects(1, &scissorRect);            // �V�U�[�Z�`
		commandList->SetGraphicsRootSignature(rootSignature.Get()); // ���[�g�V�O�l�`��

		ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// �����_�[�^�[�Q�b�g�̐ݒ�
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		auto dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

		float clearColor[] = { 0.390625f, 0.58203125f, 0.92578125f, 1.0f };  // ��F
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// �`�揈���̐ݒ�
		ObjectService::ProcessOnDraw();

		// ���\�[�X�o���A�̐ݒ� (RENDER_TARGET -> PRESENT)
		auto endResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->ResourceBarrier(1, &endResourceBarrier);

		// ���߂̃N���[�Y
		commandList->Close();
	}

	// �R�}���h���X�g�̎��s
	{
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(1, commandLists);

		// ��ʂ̃X���b�v
		ThrowIfFailed(swapChain->Present(1, 0));
	}

	// GPU�����̏I����ҋ@
	{
		ThrowIfFailed(commandQueue->Signal(fence.Get(), ++fenceValue));
		if (fence->GetCompletedValue() < fenceValue)
		{
			ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			WaitForSingleObject(fenceEvent, INFINITE);
		}
	}
}

// �I������
void RenderPipeline::OnDestroy()
{
	CloseHandle(fenceEvent);
}

