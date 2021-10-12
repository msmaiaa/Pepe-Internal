#include <Windows.h>
#include "dx.h"
#include "helpers.h"

bool dx::GetD3D9Device(void** pTable) {
	if (!pTable) return false;

	IDirect3D9* pD3D{ Direct3DCreate9(D3D_SDK_VERSION) };

	if (!pD3D) return false;

	IDirect3DDevice9* pDummyDevice{ nullptr };

	D3DPRESENT_PARAMETERS d3dpp{};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();
	//d3dpp.hDeviceWindow = FindWindowA("Valve001", nullptr);

	HRESULT dummyDevCreated{ pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice) };

	if (dummyDevCreated != S_OK) {
		d3dpp.Windowed = !d3dpp.Windowed;
		HRESULT dummyDevCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDevCreated != S_OK) {
			pD3D->Release();
			return false;
		}
	}

	memcpy(pTable, *(void***)(pDummyDevice), 119 * sizeof(void*));
	pDummyDevice->Release();
	pD3D->Release();
	return true;
}