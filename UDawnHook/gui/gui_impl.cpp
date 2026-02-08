#include "gui_impl.h"
#include "log.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include "font.h"
#include "../plugin/Menu.h"
#include "../plugin/Settings.h"
#include "notifications.h"
#include "../helper/eKeyboardMan.h"
#include "dx12hook.h"

bool GUIImplementation::ms_bInit = false;
bool GUIImplementation::ms_bFailed = false;
bool GUIImplementation::ms_bShouldReloadFonts = false;
int  GUIImplementation::ms_iWaitingFrames = 0;
bool GUIImplementation::ms_bShouldRefreshSwapChainData = false;
HWND GUIImplementation::ms_hWindow = 0;
ID3D11RenderTargetView* GUIImplementation::ms_pRenderTarget = nullptr;
WNDPROC	GUIImplementation::ms_pWndProc = 0;
ID3D12DescriptorHeap* GUIImplementation::g_pd3dRtvDescHeap = nullptr;
ID3D12DescriptorHeap* GUIImplementation::g_pd3dSrvDescHeap = nullptr;
ID3D12CommandQueue* GUIImplementation::g_pd3dCommandQueue = nullptr;
ID3D12GraphicsCommandList* GUIImplementation::g_pd3dCommandList = nullptr;
ID3D12Fence* GUIImplementation::g_pFence = nullptr;
HANDLE GUIImplementation::g_hFenceEvent = nullptr;
std::vector<GUIImplementation::GFrameContext> GUIImplementation::frameContextData;
int GUIImplementation::numBuffers = 0;
DXGI_FORMAT GUIImplementation::format = DXGI_FORMAT_UNKNOWN;

bool GUIImplementation::ImGui_InitDX12(IDXGISwapChain* pSwapChain, HWND hWindow)
{
	if (!ImGui::CreateContext())
	{
		eLog::Message(__FUNCTION__, "Failed to create ImGui context!");
		return false;
	}

	ImGui::GetIO().ConfigFlags  = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	if (!ImGui_ImplWin32_Init(ms_hWindow))
	{
		eLog::Message(__FUNCTION__, "Failed to init Win32 Backend!");
		return false;
	}

	ID3D12Device* pDevice = nullptr;

	HRESULT hResult = pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&pDevice);
	if (FAILED(hResult))
	{
		ms_bFailed = true;
		eLog::Message(__FUNCTION__, "ERROR: Failed to obtain D3D12 device! Error code: 0x%X", hResult);
		return false;
	}

	if (!ImGui_CreateSwapChainResources(pDevice, pSwapChain))
	{
		ms_bFailed = true;
		return false;
	}
	ms_bShouldRefreshSwapChainData = false;

	if (!ImGui_ImplDX12_Init(pDevice, numBuffers,
		format, g_pd3dSrvDescHeap, g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart()))
	{
		eLog::Message(__FUNCTION__, "Failed to init DX12 Backend!");
		return false;
	}

	ms_pWndProc = (WNDPROC)SetWindowLongPtr(ms_hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

	if (!ms_pWndProc)
	{
		eLog::Message(__FUNCTION__, "Failed to set Window Procedure! Error code: %d", GetLastError());
		return false;
	}

	// fence
	{
		HRESULT hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence));
		if (FAILED(hr))
		{
			eLog::Message(__FUNCTION__, "ERROR: Failed to create fence! Error code: 0x%X", hr);
			return false;
		}

		g_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!g_hFenceEvent)
		{
			eLog::Message(__FUNCTION__, "ERROR: Failed to create fence event!");
			return false;
		}
	}

	pDevice->Release();

	ImGui_SetStyle();
	DEBUG_LOG(__FUNCTION__, "INFO: Init OK");
	return true;
}

bool GUIImplementation::ImGui_CreateSwapChainResources(ID3D12Device* pDevice, IDXGISwapChain* pSwapChain)
{
	DEBUG_LOG(__FUNCTION__, "Device: %p", pDevice);

	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		pSwapChain->GetDesc(&desc);
		numBuffers = desc.BufferCount;
		format = desc.BufferDesc.Format;

		DEBUG_LOG(__FUNCTION__, "Num buffers: %d", numBuffers);

		frameContextData.clear();
		frameContextData.resize(numBuffers);
	}

	// SRV
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = numBuffers + 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		HRESULT hResult = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap));

		if (FAILED(hResult))
		{
			eLog::Message(__FUNCTION__, "ERROR: Failed to create g_pd3dSrvDescHeap! Error code: 0x%X", hResult);
			return false;
		}
		DEBUG_LOG(__FUNCTION__, "g_pd3dSrvDescHeap: %p", g_pd3dSrvDescHeap);
	}

	// descriptor heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = numBuffers;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;


		HRESULT hResult = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap));
		if (FAILED(hResult))
		{
			eLog::Message(__FUNCTION__, "ERROR: Failed to create g_pd3dRtvDescHeap! Error code: 0x%X", hResult);
			return false;
		}
		DEBUG_LOG(__FUNCTION__, "g_pd3dRtvDescHeap: %p", g_pd3dRtvDescHeap);
	}

	// command allocator
	for (UINT i = 0; i < numBuffers; ++i)
	{
		HRESULT hr = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameContextData[i].commandAllocator));
		if (FAILED(hr))
		{
			eLog::Message(__FUNCTION__, "ERROR: Failed to create command allocator! Error code: 0x%X", hr);
			return false;
		}
	}

	// command list
	HRESULT hResult = pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameContextData[0].commandAllocator, nullptr, IID_PPV_ARGS(&g_pd3dCommandList));
	if (hResult != S_OK || g_pd3dCommandList->Close() != S_OK)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to create command list! Error code: 0x%X", hResult);
		return false;
	}


	// render targets
	{
		SIZE_T rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

		for (UINT i = 0; i < numBuffers; ++i)
		{
			ID3D12Resource* pBackBuffer = nullptr;
			HRESULT hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			if (FAILED(hr))
			{
				eLog::Message(__FUNCTION__, "ERROR: Failed to get buffer! Error code: 0x%X", hr);
				return false;
			}

			pDevice->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
			pBackBuffer->Release();
			frameContextData[i].mainRenderTargetDescriptor = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	return true;
}

bool GUIImplementation::ImGui_RecreateAfterResize(ID3D12Device* pDevice, IDXGISwapChain* pSwapChain)
{
	DEBUG_LOG(__FUNCTION__, "Device: %p", pDevice);

	bool bNumBuffersChanged = false;
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		pSwapChain->GetDesc(&desc);

		if (numBuffers != desc.BufferCount)
			bNumBuffersChanged = true;

		numBuffers = desc.BufferCount;
		format = desc.BufferDesc.Format;

		DEBUG_LOG(__FUNCTION__, "Num buffers: %d", numBuffers);
	}

	if (bNumBuffersChanged) // recreate SRV if numBuffers changed
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = numBuffers + 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		HRESULT hResult = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap));

		if (FAILED(hResult))
		{
			eLog::Message(__FUNCTION__, "ERROR: Failed to create g_pd3dSrvDescHeap! Error code: 0x%X", hResult);
			return false;
		}
		DEBUG_LOG(__FUNCTION__, "g_pd3dSrvDescHeap: %p", g_pd3dSrvDescHeap);
	}

	// descriptor heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = numBuffers;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;


		HRESULT hResult = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap));
		if (FAILED(hResult))
		{
			eLog::Message(__FUNCTION__, "ERROR: Failed to create g_pd3dRtvDescHeap! Error code: 0x%X", hResult);
			return false;
		}
		DEBUG_LOG(__FUNCTION__, "g_pd3dRtvDescHeap: %p", g_pd3dRtvDescHeap);
	}

	// render targets
	{
		SIZE_T rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

		for (UINT i = 0; i < numBuffers; ++i)
		{
			ID3D12Resource* pBackBuffer = nullptr;
			HRESULT hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			if (FAILED(hr))
			{
				eLog::Message(__FUNCTION__, "ERROR: Failed to get buffer! Error code: 0x%X", hr);
				return false;
			}

			pDevice->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
			pBackBuffer->Release();
			frameContextData[i].mainRenderTargetDescriptor = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	// reinit ImGui if SRV changed
	if (bNumBuffersChanged)
	{
		ImGui_ImplDX12_Shutdown();

		if (!ImGui_ImplDX12_Init(pDevice, numBuffers,
			format, g_pd3dSrvDescHeap, g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart()))
		{
			eLog::Message(__FUNCTION__, "Failed to init DX12 Backend!");
			return false;
		}
	}

	return true;
}

void GUIImplementation::ImGui_DeleleteSwapChainResources()
{
	if (g_pd3dRtvDescHeap)
	{
		g_pd3dRtvDescHeap->Release();
		g_pd3dRtvDescHeap = nullptr;
	}
}

void GUIImplementation::WaitForFrames()
{
	for (auto& frame : frameContextData)
	{
		if (frame.fenceValue != 0 && g_pFence->GetCompletedValue() < frame.fenceValue)
		{
			HRESULT hr = g_pFence->SetEventOnCompletion(frame.fenceValue, g_hFenceEvent);
			if (FAILED(hr))
			{
				eLog::Message(__FUNCTION__, "ERROR: Failed to set event on completion! Error code: 0x%X", hr);
				return;
			}
			WaitForSingleObject(g_hFenceEvent, INFINITE);
		}
	}
}

void GUIImplementation::ImGui_SetStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowRounding = 6.0f;
	style->ItemSpacing = ImVec2(7, 5.5);
	style->FrameRounding = 2.0f;
	style->FramePadding = ImVec2(6, 4.25);
	ImVec4* colors = style->Colors;

	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		ImVec4 col = style->Colors[i];
		if (i == ImGuiCol_Text || i == ImGuiCol_TextDisabled ||
			i == ImGuiCol_WindowBg || i == ImGuiCol_MenuBarBg) continue;

		const float r = col.x * 0.85f;
		const float g = col.y * 0.90f;
		const float b = col.z;

		if (i == ImGuiCol_Button || i == ImGuiCol_FrameBg || i == ImGuiCol_Header || i == ImGuiCol_Tab)
		{
			const float gray = (r + g + b) / 3.0f;
			const float multiplier = 0.4f;
			style->Colors[i] = { r + (gray - r) * multiplier, g + (gray - g) * multiplier, b + (gray - b) * multiplier, col.w};
			continue;
		}
		
		style->Colors[i] = { r, g, b, col.w };
	}

	ImGui_ReloadFont();
}

void GUIImplementation::ImGui_ReloadFont()
{
	float fontSize = 16.0f;
	float fMenuScale = SettingsMgr->fMenuScale;
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	io.Fonts->AddFontFromMemoryCompressedTTF(Font_compressed_data, Font_compressed_size, fontSize * fMenuScale);
	io.Fonts->Build();

	ImGui_ImplDX12_InvalidateDeviceObjects();
}

void GUIImplementation::OnPresent(IDXGISwapChain3* pSwapChain, bool bShouldResetFenceValues)
{
	if (ms_bFailed)
		return;

	if (!ms_bInit)
		OnPresent_GUIStart(pSwapChain);

	if (!ms_bInit)
		return;

	if (bShouldResetFenceValues)
	{
		WaitForFrames();
		for (auto& frame : frameContextData)
			frame.fenceValue = 0;
	}

	if (ms_bShouldRefreshSwapChainData)
	{
		ID3D12Device* pDevice = nullptr;
		HRESULT hr = pSwapChain->GetDevice(IID_PPV_ARGS(&pDevice));
		if (FAILED(hr))
		{
			eLog::Message(__FUNCTION__, "ERROR: Failed to obtain D3D12 device! Error code: 0x%X", hr);
			return;
		}
		
		if (!ImGui_RecreateAfterResize(pDevice, pSwapChain))
			return;

		pDevice->Release();
		ms_bShouldRefreshSwapChainData = false;
	}

	ImGui_ProcessDX12(pSwapChain);
}

void GUIImplementation::OnPresent_GUIStart(IDXGISwapChain* pSwapChain)
{
	ID3D12Device* pDevice = nullptr;

	HRESULT hResult = pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&pDevice);
	if (FAILED(hResult))
	{
		ms_bFailed = true;
		eLog::Message(__FUNCTION__, "ERROR: Failed to obtain D3D12 device! Error code: 0x%X", hResult);
		return;
	}

	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

	pSwapChain->GetDesc(&swapChainDescription);

	ms_hWindow = swapChainDescription.OutputWindow;

	if (!ms_hWindow)
	{
		ms_bFailed = true;
		eLog::Message(__FUNCTION__, "ERROR: Failed to obtain D3D12 device window!");
		return;
	}

	if (ImGui_InitDX12(pSwapChain, ms_hWindow))
	{
		ms_bInit = true;
		ms_bFailed = false;
		DEBUG_LOG(__FUNCTION__, "INFO: Init OK");
	}
}

void GUIImplementation::ImGui_ProcessDX12(IDXGISwapChain3* pSwapChain)
{
	if (!ms_bInit)
		return;

	DX12Hook* dx12hook = DX12Hook::Get();

	g_pd3dCommandQueue = dx12hook->GetCommandQueue(pSwapChain);
	if (!g_pd3dCommandQueue)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to get command queue!");
		return;
	}
	if (!g_pd3dCommandList)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to get command list!");
		return;
	}
	if (!g_pd3dSrvDescHeap)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to get SRV!");
		return;
	}

	if (ms_bShouldReloadFonts)
	{
		if (ms_iWaitingFrames == 0)
		{
			ImGui_ReloadFont();
			ms_bShouldReloadFonts = false;
		}
		else
		{
			--ms_iWaitingFrames;
			return;
		}
	}

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	GUI_Process();
	
	ImGui::EndFrame();

	UINT backBufferIdx = pSwapChain->GetCurrentBackBufferIndex();
	if (backBufferIdx >= frameContextData.size())
	{
		eLog::Message(__FUNCTION__, "ERROR: BackBufferIndex is higher or equal to the size of frame contex data vector!");
		return;
	}

	GFrameContext& frameContext = frameContextData[backBufferIdx];
	if (!frameContext.mainRenderTargetDescriptor.ptr)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to get frame descriptor!");
		return;
	}

	ID3D12CommandAllocator* commandAllocator = nullptr;
	commandAllocator = frameContextData[backBufferIdx].commandAllocator;
	if (!commandAllocator)
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to get command allocator!");
		return;
	}


	WaitForFrames();

	commandAllocator->Reset();


	D3D12_RESOURCE_BARRIER barrier = { };
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	ID3D12Resource* pBackBuffer = nullptr;
	HRESULT hr = pSwapChain->GetBuffer(backBufferIdx, IID_PPV_ARGS(&pBackBuffer));
	if (FAILED(hr))
	{
		eLog::Message(__FUNCTION__, "ERROR: Failed to get buffer! Error code: 0x%X", hr);
		return;
	}

	barrier.Transition.pResource = pBackBuffer;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	g_pd3dCommandList->Reset(commandAllocator, nullptr);
	g_pd3dCommandList->ResourceBarrier(1, &barrier);
	g_pd3dCommandList->OMSetRenderTargets(1, &frameContext.mainRenderTargetDescriptor, FALSE, nullptr);
	g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	g_pd3dCommandList->ResourceBarrier(1, &barrier);
	g_pd3dCommandList->Close();

	pBackBuffer->Release();

	g_pd3dCommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&g_pd3dCommandList));
	
	UINT64 fenceValue = dx12hook->GetFenceValue(pSwapChain);
	UINT64 targetValue = ++fenceValue;
	g_pd3dCommandQueue->Signal(g_pFence, targetValue);
	frameContext.fenceValue = targetValue;
	dx12hook->SetFenceValue(pSwapChain, targetValue);
}

void GUIImplementation::Gamepad_Process()
{

}

void GUIImplementation::Gamepad_Reset()
{

}

void GUIImplementation::OnBeforeResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	DEBUG_LOG(__FUNCTION__, "Deleting resources!");

	WaitForFrames();
	ImGui_DeleleteSwapChainResources();
}

void GUIImplementation::OnAfterResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	ms_bShouldRefreshSwapChainData = true;
}


void GUIImplementation::Shutdown()
{
	if (!ms_bInit)
		return;

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX12_Shutdown();
}

void GUIImplementation::RequestFontReload()
{
	if (ms_iWaitingFrames == 0)
		ms_iWaitingFrames = numBuffers + 1;

	ms_bShouldReloadFonts = true;
}

static void FlushGameKeys(const HWND hWnd)
{
	for (int i = 0; i < VK_KEY_MAX; ++i)
	{
		if (eKeyboardMan::GetKeyState(i) & KEY_HELD)
			CallWindowProc(GUIImplementation::ms_pWndProc, hWnd, WM_KEYUP, i, 0);
	}
}

static bool IsOverlappingKey(WPARAM wParam)
{
	eSettingsManager::Keys::FreeCam freeCamKeys;
	{
		std::lock_guard<std::mutex> lock(SettingsMgr->mtx_keys);
		freeCamKeys = SettingsMgr->keys.freeCam;
	}

	if (wParam == freeCamKeys.iFreeCameraKeyForward
		|| wParam == freeCamKeys.iFreeCameraKeyBackward
		|| wParam == freeCamKeys.iFreeCameraKeyDown
		|| wParam == freeCamKeys.iFreeCameraKeyUp
		|| wParam == freeCamKeys.iFreeCameraKeyLeft
		|| wParam == freeCamKeys.iFreeCameraKeyRight
		|| wParam == freeCamKeys.iFreeCameraKeyYawPlus
		|| wParam == freeCamKeys.iFreeCameraKeyYawMinus
		|| wParam == freeCamKeys.iFreeCameraKeyPitchPlus
		|| wParam == freeCamKeys.iFreeCameraKeyPitchMinus
		|| wParam == freeCamKeys.iFreeCameraKeyRollPlus
		|| wParam == freeCamKeys.iFreeCameraKeyRollMinus)
	{
		return true;
	}

	return false;
}

LRESULT WINAPI GUIImplementation::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KILLFOCUS:
		TheMenu->m_bIsFocused = false;
		eKeyboardMan::OnFocusLost();
		break;
	case WM_SETFOCUS:
		TheMenu->m_bIsFocused = true;
		break;
	case WM_KEYDOWN:
		eKeyboardMan::SetKeyStatus(wParam, true);
		eKeyboardMan::SetLastPressedKey(wParam);

		eSettingsManager::Keys localKeys;
		{
			std::lock_guard<std::mutex> lock(SettingsMgr->mtx_keys);
			localKeys = SettingsMgr->keys;
		}

		if (wParam == localKeys.iHookMenuOpenKey && (eKeyboardMan::GetKeyState(wParam) & KEY_JUST_PRESSED))
		{
			FlushGameKeys(hWnd);
			TheMenu->m_iIsActive.fetch_xor(true, std::memory_order_relaxed);
		}

		if (!TheMenu->m_iIsActive.load(std::memory_order_relaxed))
		{
			if (wParam == localKeys.iToggleFreeCameraKey && (eKeyboardMan::GetKeyState(wParam) & KEY_JUST_PRESSED))
			{
				FlushGameKeys(hWnd);
				{
					std::lock_guard<std::mutex> lock(TheMenu->mtx_firstpersoncamera);
					if (TheMenu->fpCamSharedData.bFPCamEnabled)
						TheMenu->fpCamSharedData.bFPCamEnabled = false;
				}
				{
					std::lock_guard<std::mutex> lock(TheMenu->mtx_freecamera);
					TheMenu->freeCamSharedData.bFreeCamEnabled ^= true;
				}
			}
			else if (wParam == localKeys.freeCam.iFreeCameraKeyReset && (eKeyboardMan::GetKeyState(wParam) & KEY_JUST_PRESSED))
			{
				{
					std::lock_guard<std::mutex> lock(TheMenu->mtx_freecamera);
					TheMenu->freeCamSharedData.bFreeCamShouldReset = true;
				}
			}
			else if (wParam == localKeys.iToggleFirstPersonCamKey && (eKeyboardMan::GetKeyState(wParam) & KEY_JUST_PRESSED) && TheMenu->m_bCharacterValid.load(std::memory_order_relaxed))
			{
				{
					std::lock_guard<std::mutex> lock(TheMenu->mtx_freecamera);
					if (TheMenu->freeCamSharedData.bFreeCamEnabled)
						TheMenu->freeCamSharedData.bFreeCamEnabled = false;
				}
				{
					std::lock_guard<std::mutex> lock(TheMenu->mtx_firstpersoncamera);
					TheMenu->fpCamSharedData.bFPCamEnabled ^= true;
				}
			}
			else if (wParam == localKeys.iToggleFreezeTime && (eKeyboardMan::GetKeyState(wParam) & KEY_JUST_PRESSED))
			{
				{
					std::lock_guard<std::mutex> lock(TheMenu->mtx_speed);
					TheMenu->m_bFreezeTime ^= true;
					TheMenu->m_bEnableChangeSpeed = false;
				}
			}
			else if (wParam == localKeys.iToggleSprinting && (eKeyboardMan::GetKeyState(wParam) & KEY_JUST_PRESSED))
				TheMenu->m_iSprintingEnabled.fetch_xor(true, std::memory_order_relaxed);
		}

		break;
	case WM_KEYUP:
		eKeyboardMan::SetKeyStatus(wParam, false);
		eKeyboardMan::SetLastPressedKey(0);
		break;
	default:
		break;
	}
	if (TheMenu->m_iIsActive.load(std::memory_order_relaxed))
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}
	else
	{
		bool bFreeCamEnabled;
		bool bBlockOverlappingKeys;
		{
			std::lock_guard<std::mutex> lock(TheMenu->mtx_freecamera);
			bFreeCamEnabled = TheMenu->freeCamSharedData.bFreeCamEnabled;
			bBlockOverlappingKeys = TheMenu->freeCamSharedData.bBlockOverlappingKeys;
		}

		if (bFreeCamEnabled && bBlockOverlappingKeys && IsOverlappingKey(wParam))
			return true;
	}

	return CallWindowProc(ms_pWndProc, hWnd, uMsg, wParam, lParam);
}

void GUIImplementation::GUI_Process()
{
	ImGui::GetIO().MouseDrawCursor = false;

	static bool bDrewStartupNotification = false;

	if (TheMenu->m_bMenuReached.load(std::memory_order_relaxed) && !bDrewStartupNotification)
	{
		int iHookMenuOpenKey;
		{
			std::lock_guard<std::mutex> lock(SettingsMgr->mtx_keys);
			iHookMenuOpenKey = SettingsMgr->keys.iHookMenuOpenKey;
		}

		Notifications->SetNotificationTime(8500);
		Notifications->PushNotification("UDawnHook %s is running! Press %s to open the menu. Build date: %s\n", UNTILDAWN_HOOK_VERSION, eKeyboardMan::KeyToString(iHookMenuOpenKey), __DATE__);
		bDrewStartupNotification = true;
	}

	Notifications->Update();
	Notifications->Draw();
	TheMenu->Draw();
}
