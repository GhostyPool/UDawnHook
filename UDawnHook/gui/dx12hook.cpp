#include "dx12hook.h"
#include <iostream>
#include <Windows.h>
#include <assert.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#include "../utils/core.h"

DX12Hook::DX12Hook()
{
	presentPtr = 0;
	resizeBuffersPtr = 0;
	presentOriginalPtr = 0;
	resizeBuffersOriginalPtr = 0;
	executeCommandsPtr = 0;
	executeCommandsOriginalPtr = 0;
	createCommandQueuePtr = 0;
	createSwapChainForHwndPtr = 0;
	createCommandQueueOriginalPtr = 0;
	createSwapChainForHwndOriginalPtr = 0;
	createSwapChainPtr = 0;
	createSwapChainOriginalPtr = 0;
	previousSwapChain = nullptr;
}

DX12Hook::~DX12Hook()
{
	MH_DisableHook((void*)presentPtr);
	MH_DisableHook((void*)resizeBuffersPtr);
	MH_DisableHook((void*)executeCommandsPtr);
	MH_DisableHook((void*)createCommandQueuePtr);
	MH_DisableHook((void*)createSwapChainForHwndPtr);
	MH_DisableHook((void*)createSwapChainPtr);
}

bool DX12Hook::Initialize()
{
	DEBUG_LOG(__FUNCTION__, "INFO: Init");


	bool isDXGILoaded = false;
	auto hDXGI = GetModuleHandleW(L"dxgi.dll");
	if (!hDXGI)
		hDXGI = LoadLibraryW(L"dxgi.dll");
	else
		isDXGILoaded = true;

	if (!hDXGI && !isDXGILoaded)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to obtain dxgi.dll! Error code: %d", GetLastError());
		return false;
	}

	ID3D12Device* device = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	IDXGISwapChain3* swapChain = nullptr;
	IDXGIFactory4* factory = nullptr;
	HWND hWnd = 0;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	auto pCreateDXGIFactory = GetProcAddress(hDXGI, "CreateDXGIFactory");
	if (pCreateDXGIFactory == NULL)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to obtain CreateDXGIFactory! Error code: %d", GetLastError());
		return false;
	}


	HRESULT hResult = D3D12CreateDevice(nullptr, featureLevel, __uuidof(ID3D12Device), (void**)&device);
	if (FAILED(hResult))
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to create D3D12 device. Error code: 0x%X", hResult);
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc;
	ZeroMemory(&queueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = 0;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;


	hResult = device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&commandQueue);

	if (FAILED(hResult))
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to create D3D12 command queue. Error code: 0x%X", hResult);
		return false;
	}


	if (((HRESULT(WINAPI*)(const IID&, void**))(pCreateDXGIFactory))(__uuidof(IDXGIFactory), (void**)&factory) < 0)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to create DXGI factory! Error code: %d", GetLastError());
		return false;
	}

	hWnd = CreateDummyWindow();
	DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
	ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swapChainDescription.BufferCount = NUM_BACK_BUFFERS;
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	
	hResult = factory->CreateSwapChainForHwnd(commandQueue, hWnd, &swapChainDescription, NULL, NULL, (IDXGISwapChain1**)&swapChain);

	if (FAILED(hResult))
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to create swapchain! Error code: 0x%X", hResult);
		return false;
	}

	uintptr_t* swapChainvTable = *(uintptr_t**)(swapChain);
	uintptr_t* queuevTable = *(uintptr_t**)(commandQueue);
	uintptr_t* devicevTable = *(uintptr_t**)(device);
	uintptr_t* factoryvTable = *(uintptr_t**)(factory);
	
	if (swapChainvTable == nullptr)
	{
		eLog::Message(__FUNCTION__, "ERROR: Could not obtain D3D12 SwapChain vTable!");
		return false;
	}

	if (queuevTable == nullptr)
	{
		eLog::Message(__FUNCTION__, "ERROR: Could not obtain D3D12 CommandQueue vTable!");
		return false;
	}

	if (devicevTable == nullptr)
	{
		eLog::Message(__FUNCTION__, "ERROR: Could not obtain D3D12 Device vTable!");
		return false;
	}

	if (factoryvTable == nullptr)
	{
		eLog::Message(__FUNCTION__, "ERROR: Could not obtain D3D12 Factory vTable!");
		return false;
	}

	presentPtr = swapChainvTable[8];
	resizeBuffersPtr = swapChainvTable[13];

	executeCommandsPtr = queuevTable[10];

	createCommandQueuePtr = devicevTable[8];
	createSwapChainForHwndPtr = factoryvTable[15];
	createSwapChainPtr = factoryvTable[10];

	DEBUG_LOG(__FUNCTION__, "INFO: D3D12 SwapChain vTable: 0x%p", swapChainvTable);
	DEBUG_LOG(__FUNCTION__, "INFO: D3D12 CommandQueue vTable: 0x%p", queuevTable);

	DEBUG_LOG(__FUNCTION__, "INFO: D3D12 Present: 0x%p", presentPtr);
	DEBUG_LOG(__FUNCTION__, "INFO: D3D12 ResizeBuffers: 0x%p", resizeBuffersPtr);
	DEBUG_LOG(__FUNCTION__, "INFO: D3D12 CommandQueue ExecuteCommandList: 0x%p", executeCommandsPtr);
	DEBUG_LOG(__FUNCTION__, "INFO: D3D12 Device CreateCommandQueue: 0x%p", createCommandQueuePtr);
	DEBUG_LOG(__FUNCTION__, "INFO: D3D12 Factory CreateSwapChainForHwnd: 0x%p", createSwapChainForHwndPtr);
	DEBUG_LOG(__FUNCTION__, "INFO: D3D12 Factory CreateSwapChain: 0x%p", createSwapChainPtr);
	assert(presentPtr);
	assert(resizeBuffersPtr);
	assert(executeCommandsPtr);
	assert(createCommandQueuePtr);
	assert(createSwapChainForHwndPtr);
	assert(createSwapChainPtr);

	commandQueue->Release();
	swapChain->Release();
	factory->Release();
	device->Release();

	if (g_MHStatus == MH_UNKNOWN)
		MH_Initialize();

	if (!isDXGILoaded)
		FreeLibrary(hDXGI);

	if (hWnd)
		DestroyWindow(hWnd);

	DEBUG_LOG(__FUNCTION__, "INFO: Finished");
	return true;
}

bool DX12Hook::Enable()
{
	DEBUG_LOG(__FUNCTION__, "INFO: Init");

	MH_STATUS s = MH_CreateHook((void*)presentPtr, Present, (void**)&presentOriginalPtr);

	if (s == MH_OK)
	{
		DEBUG_LOG(__FUNCTION__, "INFO: Present hooked!");
		MH_EnableHook((void*)presentPtr);
	}
	else
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to hook Present! Error code: (MH)%d", s);
		return false;
	}

	s = MH_CreateHook((void*)resizeBuffersPtr, ResizeBuffers, (void**)&resizeBuffersOriginalPtr);
	
	if (s == MH_OK)
	{
		DEBUG_LOG(__FUNCTION__, "INFO: ResizeBuffers hooked!");
		MH_EnableHook((void*)resizeBuffersPtr);
	}
	else
	{
		DEBUG_LOG(__FUNCTION__, "ERROR: Failed to hook ResizeBuffers! Error code: (MH)%d", s);
		return false;
	}

	s = MH_CreateHook((void*)createSwapChainForHwndPtr, CreateSwapChainForHwnd, (void**)&createSwapChainForHwndOriginalPtr);

	if (s == MH_OK)
	{
		DEBUG_LOG(__FUNCTION__, "INFO: CreateSwapChainForHwnd hooked!");
		MH_EnableHook((void*)createSwapChainForHwndPtr);
	}
	else
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to hook CreateSwapChainForHwnd! Error code: (MH)%d", s);
		return false;
	}

	s = MH_CreateHook((void*)createSwapChainPtr, CreateSwapChain, (void**)&createSwapChainOriginalPtr);

	if (s == MH_OK)
	{
		DEBUG_LOG(__FUNCTION__, "INFO: CreateSwapChain hooked!");
		MH_EnableHook((void*)createSwapChainPtr);
	}
	else
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to hook CreateSwapChain! Error code: (MH)%d", s);
		return false;
	}


	DEBUG_LOG(__FUNCTION__, "INFO: Finished");

	return true;
}

DX12Hook* DX12Hook::Get()
{
	static DX12Hook DX12Hook;

	return &DX12Hook;
}

uintptr_t DX12Hook::GetPresentAddress() const
{
	return presentOriginalPtr;
}

uintptr_t DX12Hook::GetResizeBuffersAddress() const
{
	return resizeBuffersOriginalPtr;
}

uintptr_t DX12Hook::GetExecuteCommandsAddress() const
{
	return executeCommandsOriginalPtr;
}

uintptr_t DX12Hook::GetCreateCommandQueueAddress() const
{
	return createCommandQueueOriginalPtr;
}

uintptr_t DX12Hook::GetCreateSwapChainForHwndAddress() const
{
	return createSwapChainForHwndOriginalPtr;
}

uintptr_t DX12Hook::GetCreateSwapChainAddress() const
{
	return createSwapChainOriginalPtr;
}

ID3D12CommandQueue* DX12Hook::GetCommandQueue(IDXGISwapChain* pSwapChain) const
{
	auto it = swapChains.find(pSwapChain);
	if (it != swapChains.end())
		return it->second.first;

	return nullptr;
}

UINT64 DX12Hook::GetFenceValue(IDXGISwapChain* pSwapChain) const
{
	auto it = swapChains.find(pSwapChain);
	if (it != swapChains.end())
		return it->second.second;

	return 0;
}

void DX12Hook::SetFenceValue(IDXGISwapChain* pSwapChain, UINT64 value)
{
	auto it = swapChains.find(pSwapChain);
	if (it != swapChains.end())
	{
		it->second.second = value;
	}
	else
		eLog::Message(__FUNCTION__, "ERROR: Failed to set fence value for command queue associated with swap chain: %p!", pSwapChain);
}

HWND DX12Hook::CreateDummyWindow()
{
	WNDCLASS windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASS));

	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.lpszClassName = "DX12HOOKDUMMY";

	RegisterClass(&windowClass);

	return CreateWindow(windowClass.lpszClassName, "DX12HOOKWND", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, windowClass.hInstance, NULL);
}


void DX12Hook::OnPresent(IDXGISwapChain* pSwapChain)
{
	bool bShouldResetFenceValues = false;
	if (previousSwapChain != pSwapChain)
	{
		bShouldResetFenceValues = true;
		previousSwapChain = pSwapChain;
	}

	GUIImplementation::OnPresent((IDXGISwapChain3*)pSwapChain, bShouldResetFenceValues);
}

void DX12Hook::OnBeforeResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	GUIImplementation::OnBeforeResize(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

void DX12Hook::OnAfterResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	GUIImplementation::OnAfterResize(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

void DX12Hook::OnCreateSwapChain(IUnknown* pDevice, IDXGISwapChain* pSwapChain)
{
	auto it = swapChains.find(pSwapChain);
	if (it == swapChains.end())
	{
		swapChains.emplace(pSwapChain, std::make_pair((ID3D12CommandQueue*)pDevice, 0));
		DEBUG_LOG(__FUNCTION__, "Added swapChain: %p associted with command queue: %p", pSwapChain, pDevice);
	}
	else
	{
		it->second = std::make_pair((ID3D12CommandQueue*)pDevice, 0);
		DEBUG_LOG(__FUNCTION__, "Updated swapChain: %p to new command queue: %p", pSwapChain, pDevice);
	}
}

HRESULT __stdcall DX12Hook::Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	DX12Hook* dx12hook = DX12Hook::Get();
	uintptr_t addr = dx12hook->GetPresentAddress();
	assert(addr != 0);

	//DEBUG_LOG(__FUNCTION__, "Swap chain ptr: %p, thread id: %lu", pSwapChain, GetCurrentThreadId());

	dx12hook->OnPresent(pSwapChain);

	HRESULT hr = ((HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT))addr)(pSwapChain, SyncInterval, Flags);

	return hr;
}

HRESULT __stdcall DX12Hook::ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	DX12Hook* dx12hook = DX12Hook::Get();
	uintptr_t addr = dx12hook->GetResizeBuffersAddress();
	assert(addr != 0);

	DEBUG_LOG(__FUNCTION__, "Swap chain: %p, thread id: %lu", pSwapChain, GetCurrentThreadId());

	dx12hook->OnBeforeResize(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	HRESULT result = ((HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT))addr)(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	dx12hook->OnAfterResize(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	return result;
}

HRESULT STDMETHODCALLTYPE DX12Hook::CreateSwapChainForHwnd(IDXGIFactory2* pFactory, IUnknown* pDevice, HWND hWnd, DXGI_SWAP_CHAIN_DESC1* pDesc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
{
	DX12Hook* dx12hook = DX12Hook::Get();
	uintptr_t addr = dx12hook->GetCreateSwapChainForHwndAddress();
	assert(addr != 0);

	HRESULT hr = ((HRESULT(STDMETHODCALLTYPE*)(IDXGIFactory2*, IUnknown*, HWND, DXGI_SWAP_CHAIN_DESC1*, DXGI_SWAP_CHAIN_FULLSCREEN_DESC*, IDXGIOutput*, IDXGISwapChain1**))addr)(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);

	DEBUG_LOG(__FUNCTION__, "Swap chain: %p associated with command queue: %p created, thread: %lu", *ppSwapChain, pDevice, GetCurrentThreadId());

	dx12hook->OnCreateSwapChain(pDevice, *ppSwapChain);

	return hr;
}

HRESULT STDMETHODCALLTYPE DX12Hook::CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	DX12Hook* dx12hook = DX12Hook::Get();
	uintptr_t addr = dx12hook->GetCreateSwapChainAddress();
	assert(addr != 0);

	HRESULT hr = ((HRESULT(STDMETHODCALLTYPE*)(IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**))addr)(pFactory, pDevice, pDesc, ppSwapChain);

	DEBUG_LOG(__FUNCTION__, "Swap chain: %p associated with command queue: %p created, thread: %lu", *ppSwapChain, pDevice, GetCurrentThreadId());

	dx12hook->OnCreateSwapChain(pDevice, *ppSwapChain);

	return hr;
}

DWORD __stdcall DX12Hook_Thread()
{
	DX12Hook* dx12hook = DX12Hook::Get();
	if (!dx12hook->Initialize())
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to initialize DX12Hook\n");
		return FALSE;
	}

	if (!dx12hook->Enable())
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to enable DX12Hook\n");
		return FALSE;
	}

	eLog::Message(__FUNCTION__, "INFO: DX12Hook initialized OK");
	return TRUE;
}
