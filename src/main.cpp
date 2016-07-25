

#include <Windows.h>
#include <./Device/WindowDevice.h>
#include <./Device/DirectXDevice.h>

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine,
					 int nCmdShow) {
	
	WindowDevice &window = WindowDevice::getInstance();
	DX11Device &device = DX11Device::getInstance();
	window.Init(hInstance, nCmdShow, TEXT("test window"), 1280, 720, true);
	device.Init(window.getHandle(), window.getWidth(), window.getHeight(), window.getWindowMode());

	int ret = window.MessageLoop([]() { 
		DX11Device &device = DX11Device::getInstance();
		float clear [] = {0.3f,0.3f,0.3f,0.0f};
		device.getContext()->ClearRenderTargetView(device.getRTV(), clear);
		device.getContext()->ClearDepthStencilView(device.getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		device.getSwapChain()->Present(0, 0);
	});
	return ret;
}