#pragma once
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <vector>
#include "const.h"
#include <atomic>

enum GUIImplementationMode {
	GIM_DX11 = 1,
	GIM_DX12
};

class GUIImplementation {
public:
	static bool										ms_bInit;
	static bool										ms_bFailed;
	static int										ms_iWaitingFrames;
	static bool										ms_bShouldReloadFonts;
	static bool										ms_bShouldRefreshSwapChainData;
	static HWND										ms_hWindow;
	static WNDPROC									ms_pWndProc;
	static ID3D11RenderTargetView*					ms_pRenderTarget;
	// dx12
	static ID3D12DescriptorHeap*					g_pd3dRtvDescHeap;
	static ID3D12DescriptorHeap*					g_pd3dSrvDescHeap;
	static ID3D12CommandQueue*						g_pd3dCommandQueue;
	static ID3D12GraphicsCommandList*				g_pd3dCommandList;

	static ID3D12Fence*								g_pFence;
	static HANDLE									g_hFenceEvent;

	struct GFrameContext {
		D3D12_CPU_DESCRIPTOR_HANDLE  mainRenderTargetDescriptor;
		ID3D12CommandAllocator* commandAllocator;
		UINT64 fenceValue;
	};

	static std::vector<GFrameContext> frameContextData;
	static int						  numBuffers;
	static DXGI_FORMAT				  format;

	static void Shutdown();
	static bool ImGui_InitDX12(IDXGISwapChain* pSwapChain, HWND hWindow);
	static bool ImGui_CreateSwapChainResources(ID3D12Device* pDevice, IDXGISwapChain* pSwapChain);
	static void ImGui_DeleleteSwapChainResources();

	static bool ImGui_RecreateAfterResize(ID3D12Device* pDevice, IDXGISwapChain* pSwapChain);

	static void ImGui_SetStyle();
	static void	ImGui_ReloadFont();

	static void WaitForFrames();

	static void OnPresent(IDXGISwapChain3* pSwapChain, bool bShouldResetFenceValues);
	static void OnPresent_GUIStart(IDXGISwapChain* pSwapChain);

	static void ImGui_ProcessDX12(IDXGISwapChain3* pSwapChain);

	static void Gamepad_Process();
	static void Gamepad_Reset();
	static void GUI_Process();

	static void OnBeforeResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	static void OnAfterResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);


	static void  RequestFontReload();

	static LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);