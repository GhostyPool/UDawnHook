#pragma once
#include "../minhook/include/MinHook.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include "gui_impl.h"
#include "log.h"
#include "const.h"
#include <unordered_map>

class DX12Hook {
private:
	DX12Hook();
	~DX12Hook();

	uintptr_t presentPtr;
	uintptr_t resizeBuffersPtr;
	uintptr_t executeCommandsPtr;
	uintptr_t createCommandQueuePtr;
	uintptr_t createSwapChainForHwndPtr;
	uintptr_t createSwapChainPtr;


	uintptr_t presentOriginalPtr;
	uintptr_t resizeBuffersOriginalPtr;
	uintptr_t executeCommandsOriginalPtr;
	uintptr_t createCommandQueueOriginalPtr;
	uintptr_t createSwapChainForHwndOriginalPtr;
	uintptr_t createSwapChainOriginalPtr;

	IDXGISwapChain* previousSwapChain;

	std::unordered_map<IDXGISwapChain*, std::pair<ID3D12CommandQueue*, UINT64>> swapChains;

public:
	bool Initialize();
	bool Enable();

	static DX12Hook* Get();

	uintptr_t GetPresentAddress() const;
	uintptr_t GetResizeBuffersAddress() const;
	uintptr_t GetExecuteCommandsAddress() const;
	uintptr_t GetCreateCommandQueueAddress() const;
	uintptr_t GetCreateSwapChainForHwndAddress() const;
	uintptr_t GetCreateSwapChainAddress() const;

	ID3D12CommandQueue* GetCommandQueue(IDXGISwapChain* pSwapChain) const;
	UINT64 GetFenceValue(IDXGISwapChain* pSwapChain) const;
	void SetFenceValue(IDXGISwapChain* pSwapChain, UINT64 value);

	HWND CreateDummyWindow();

	void OnPresent(IDXGISwapChain* pSwapChain);
	void OnBeforeResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	void OnAfterResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	void OnCreateSwapChain(IUnknown* pDevice, IDXGISwapChain* pSwapChain);

	static HRESULT WINAPI Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	static HRESULT WINAPI ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

	static HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd(IDXGIFactory2* pFactory, IUnknown* pDevice, HWND hWnd, DXGI_SWAP_CHAIN_DESC1* pDesc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain);
	static HRESULT STDMETHODCALLTYPE CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
};

DWORD WINAPI DX12Hook_Thread();